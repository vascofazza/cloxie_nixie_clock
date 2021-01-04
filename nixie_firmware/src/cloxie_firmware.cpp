#include <Arduino.h>
#include <OneShotTimer.h>
#include "configuration.hpp"
#include "drivers/tube_driver.hpp"
#include "drivers/clock.hpp"
#include "drivers/leds_patterns.hpp"
#include "drivers/leds.hpp"
#include "drivers/sensors.hpp"
#include "network/wifi.hpp"
#include "network/ota_github.h"

ClockDriver *clock_driver;
TubeDriver *tube_driver;
LedDriver *led_driver;
SensorDriver *sensor_driver;
#ifdef DEBUG
#include "serial_parser.h"
#endif

OneShotTimer cycle_handler;
EveryTimer ota_handler;

#define NUM_CYCLES 6

enum CYCLE
{
  NONE = 0,
  CLOCK = 1,
  DATE = 2,
  TEMPERATURE = 3,
  TIMER = 4,
  STOPWATCH = 5,
};

int8_t cycle = CYCLE::CLOCK;
bool sleeping = false;

LedPatternList clock_patterns = {lava, lava_beat};
LedPatternList random_patterns = {rainbow, confetti, juggle, pacifica, pride};
LedPatternList date_patterns = {rainbowWithGlitter};
LedPatternList temp_patterns = {pulse};
LedPatternList timer_patterns = {sinelon};
LedPatternList notify_patterns = {pulse};

uint8_t pattern_status[6] = {0};

// rainbow, confetti clock
// sinelon supercar timer
// rainbowWithGlitter date
// juggle random
// bpm temp

void update_config_callback();
void set_led_patterns(uint8_t);
void next_cycle();
void light_sensor_self_calibration(AsyncWebServerRequest*);

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
#endif

  DEBUG_PRINTLN(F("Cloxie Nixie Clock"));
  DEBUG_PRINT(F("FW version: "));
  DEBUG_PRINTLN(GHOTA_CURRENT_TAG);

#ifdef DEBUG
  setup_serial_parser();
#endif
  setup_configuration();
  initWiFi();

  sensor_driver = new SensorDriver();
  tube_driver = new TubeDriver(sensor_driver);
  led_driver = new LedDriver(tube_driver, sensor_driver, NUM_LEDS, clock_patterns, &(pattern_status[0]), ARRAY_SIZE(clock_patterns));
  clock_driver = new ClockDriver(tube_driver);

  setup_wifi(clock_driver, update_config_callback, light_sensor_self_calibration);

  if (isConnected())
  {
    clock_driver->show_time(true);
    cycle_handler.OneShot(config.clock_cycle, next_cycle);
  }
  else
  {
    cycle = CYCLE::NONE;
  }

  // cold start
  set_led_patterns(cycle);
  for (int i = 0; i < 100; i++)
    led_driver->process_pattern(0);
  led_driver->turn_on(true);
  tube_driver->turn_on(true);

  ota_handler.Every(GHOTA_INTERVAL, check_for_updates);
  ota_handler.Start();
}

void next_cycle()
{
  int old_cycle = cycle;
  cycle = (cycle + 1) % NUM_CYCLES;

  switch (cycle)
  {
  case CYCLE::DATE:
    cycle = CYCLE::DATE;
    if (config.date)
    {
      DEBUG_PRINTLN(F("CYCLE DATE"));
      cycle_handler.OneShot(DATE_CYCLE, next_cycle);
      break;
    }
  case CYCLE::TEMPERATURE:
    cycle = CYCLE::TEMPERATURE;
    if (config.termometer)
    {
      DEBUG_PRINTLN(F("CYCLE TEMP"));
      cycle_handler.OneShot(TEMP_CYCLE, next_cycle);
      break;
    }
  case CYCLE::TIMER:
    cycle = CYCLE::TIMER;
    if (clock_driver->is_timer_set())
    {
      DEBUG_PRINTLN(F("CYCLE TIMER"));
      cycle_handler.OneShot(TIMER_CYCLE, next_cycle);
      break;
    }
  case CYCLE::STOPWATCH:
    cycle = CYCLE::STOPWATCH;
    if (clock_driver->is_stopwatch_set())
    {
      DEBUG_PRINTLN(F("CYCLE STOPWATCH"));
      cycle_handler.OneShot(STOPWATCH_CYCLE, next_cycle);
      break;
    }
  case CYCLE::NONE:
  case CYCLE::CLOCK:
  default:
    DEBUG_PRINTLN(F("CYCLE CLOCK"));
    cycle = CYCLE::CLOCK;
    cycle_handler.OneShot(config.clock_cycle, next_cycle);
  }
  if (old_cycle != cycle)
  {
    set_led_patterns(cycle);
    tube_driver->cathode_poisoning_prevention(TRANSITION_TIME);
  }
}

void handle_loop()
{
  static bool timer_running = false;

  if (!isConnected())
  {
    cycle_handler.Stop();
    cycle = CYCLE::NONE;
    set_led_patterns(cycle);
  }
  else if (cycle == CYCLE::NONE)
  {
    next_cycle();
  }

  if (clock_driver->is_timer_running() || clock_driver->is_stopwatch_running())
  {
    cycle_handler.Stop();
    cycle = clock_driver->is_timer_running() ? CYCLE::TIMER : CYCLE::STOPWATCH;
    set_led_patterns(cycle);
    timer_running = true;
  }
  else if (timer_running)
  {
    timer_running = false;
    cycle_handler.OneShot(clock_driver->is_timer_running() ? TIMER_CYCLE : STOPWATCH_CYCLE, next_cycle);
  }
  else
  {
    cycle_handler.Update();
  }

  switch (cycle)
  {
  case CYCLE::NONE:
  case CYCLE::CLOCK:
    clock_driver->show_time(true);
    break;
  case CYCLE::DATE:
    clock_driver->show_date(true);
    break;
  case CYCLE::TEMPERATURE:
  {
    clock_driver->show_time(false);
    clock_driver->show_date(false);
    clock_driver->show_timer(false);
    auto temp = sensor_driver->get_temperature_sensor_reading();
    tube_driver->display_temperature(temp);
    break;
  }
  case CYCLE::TIMER:
    clock_driver->show_timer(true);
    break;
  case CYCLE::STOPWATCH:
    clock_driver->show_stopwatch(true);
    break;
  }
}

void loop()
{
  static elapsedSeconds shutdown_delay;

  clock_driver->loop();

  ota_handler.Update();
  wifi_loop();

  bool hour_check = clock_driver->is_night_hours();

  if ((config.adaptive_brightness && sensor_driver->get_light_sensor_reading() < config.shutdown_threshold) || hour_check)
  {
    if (shutdown_delay > config.shutdown_delay)
    {
      if (!sleeping)
      {
        DEBUG_PRINTLN(F("Sleeping..."));
        tube_driver->turn_off(true);
        tube_driver->shutdown();
        led_driver->turn_off(true);
        sleeping = true;
      }
      return;
    }
#ifdef DEBUG
    static elapsedMillis deb_mils;
    if (deb_mils > 1000)
    {
      DEBUG_PRINT(F("Going to sleep in: "));
      DEBUG_PRINTLN(config.shutdown_delay - shutdown_delay);
      deb_mils = 0;
    }
#endif
  }
  else
  {
    if (sleeping)
    {
      DEBUG_PRINTLN(F("Waking up."));
      led_driver->turn_on(true);
      tube_driver->turn_on(true);
      sleeping = false;
    }
    shutdown_delay = 0;
  }

  tube_driver->loop();
  handle_loop();

#ifdef DEBUG
  serial_parser_loop();
#endif
}

void set_led_patterns(uint8_t cycle)
{
  switch (cycle)
  {
  case CYCLE::NONE:
    led_driver->set_patterns(notify_patterns, ARRAY_SIZE(date_patterns), &(pattern_status[cycle]));
    break;
  case CYCLE::DATE:
    led_driver->set_patterns(date_patterns, ARRAY_SIZE(date_patterns), &(pattern_status[cycle]));
    break;
  case CYCLE::TEMPERATURE:
    led_driver->set_patterns(temp_patterns, ARRAY_SIZE(temp_patterns), &(pattern_status[cycle]));
    break;
  case CYCLE::STOPWATCH:
  case CYCLE::TIMER:
    led_driver->set_patterns(timer_patterns, ARRAY_SIZE(timer_patterns), &(pattern_status[cycle]));
    break;
  case CYCLE::CLOCK:
  default:
    if (config.led_configuration == LED_MODE::RANDOM)
    {
      led_driver->set_patterns(random_patterns, ARRAY_SIZE(random_patterns), &(pattern_status[cycle]));
    }
    else
    {
      led_driver->set_patterns(clock_patterns, ARRAY_SIZE(clock_patterns), &(pattern_status[cycle]));
    }
  }
}

void update_config_callback()
{
  if (config.leds && !led_driver->get_status() && !sleeping)
  {
    led_driver->turn_on(false);
  }
  else if (!config.leds && led_driver->get_status())
  {
    led_driver->turn_off(false);
  }
  if (cycle == CYCLE::CLOCK)
  {
    cycle_handler.OneShot(config.clock_cycle, next_cycle);
  }
  set_led_patterns(cycle);
}

void light_sensor_self_calibration(AsyncWebServerRequest* request)
{
  DEBUG_PRINTLN("Calibrating...");
  config.brightness_offset = 0;
  config.shutdown_threshold = 0;
  send_response(request);
  //set offset
  tube_driver->turn_off(false);
  led_driver->turn_off(false);
  activeDelay(10000);

  config.brightness_offset = -(int)sensor_driver->get_light_sensor_reading();
  DEBUG_PRINT("brightness_offset: ");
  DEBUG_PRINTLN(config.brightness_offset);

  tube_driver->turn_on(false);
  led_driver->turn_on(false);
  activeDelay(10000);

  config.shutdown_threshold = (int)sensor_driver->get_light_sensor_reading() + 5;
  DEBUG_PRINT("shutdown_threshold: ");
  DEBUG_PRINTLN(config.shutdown_threshold);
}
