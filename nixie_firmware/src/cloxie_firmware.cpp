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

#define NUM_CYCLES 4

enum CYCLE
{
  CLOCK = 0,
  DATE = 1,
  TEMPERATURE = 2,
  TIMER = 3,
};

int8_t cycle = CYCLE::CLOCK;

LedPatternList clock_patterns = {lava, lava_beat};
LedPatternList random_patterns = {rainbow, confetti, juggle, sinelon};
LedPatternList date_patterns = {rainbowWithGlitter};
LedPatternList temp_patterns = {bpm};
LedPatternList timer_patterns = {sinelon};
// rainbow, confetti clock
// sinelon supercar timer
// rainbowWithGlitter date
// juggle random
// bpm temp

void update_config_callback();
void set_led_patterns(uint8_t);
void next_cycle();

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
  setup_wifi(update_config_callback);

  sensor_driver = new SensorDriver();
  tube_driver = new TubeDriver(sensor_driver);
  led_driver = new LedDriver(tube_driver, sensor_driver, NUM_LEDS, clock_patterns, ARRAY_SIZE(clock_patterns));
  clock_driver = new ClockDriver(tube_driver);

  ota_handler.Every(GHOTA_INTERVAL, check_for_updates);
  ota_handler.Start();

  // cold start
  set_led_patterns(cycle);
  for (int i = 0; i < 100; i++)
    led_driver->process_pattern(0);
  led_driver->turn_on(true);
  tube_driver->turn_on(true);

  clock_driver->show_time(true);
  cycle_handler.OneShot(CLOCK_CYCLE, next_cycle);
}

void next_cycle()
{
  int old_cycle = cycle;
  cycle = (cycle + 1) % NUM_CYCLES;

  switch (cycle)
  {
  case CYCLE::DATE:
    if (config.date)
    {
      DEBUG_PRINTLN(F("CYCLE DATE"));
      cycle_handler.OneShot(DATE_CYCLE, next_cycle);
      break;
    }
  case CYCLE::TEMPERATURE:
    if (config.termometer)
    {
      DEBUG_PRINTLN(F("CYCLE TEMP"));
      cycle_handler.OneShot(TEMP_CYCLE, next_cycle);
      break;
    }
  case CYCLE::TIMER:
    DEBUG_PRINTLN(F("CYCLE TIMER"));
    if (clock_driver->is_timer_set())
    {
      cycle_handler.OneShot(TIMER_CYCLE, next_cycle);
      break;
    }
  case CYCLE::CLOCK:
  default:
    DEBUG_PRINTLN(F("CYCLE CLOCK"));
    cycle = CYCLE::CLOCK;
    cycle_handler.OneShot(CLOCK_CYCLE, next_cycle);
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

  if (clock_driver->is_timer_running())
  {
    cycle_handler.Stop();
    cycle = CYCLE::TIMER;
    set_led_patterns(cycle);
    timer_running = true;
  }
  else if (timer_running)
  {
    timer_running = false;
    cycle_handler.OneShot(TIMER_CYCLE, next_cycle);
  }
  else
  {
    cycle_handler.Update();
  }

  switch (cycle)
  {
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
  }
}

void loop()
{
  static elapsedSeconds shutdown_delay;
  static bool sleeping = false;

  clock_driver->loop();

  wifi_loop();
  ota_handler.Update();

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
  case CYCLE::DATE:
    led_driver->set_patterns(date_patterns, ARRAY_SIZE(date_patterns));
    break;
  case CYCLE::TEMPERATURE:
    led_driver->set_patterns(temp_patterns, ARRAY_SIZE(temp_patterns));
    break;
  case CYCLE::TIMER:
    led_driver->set_patterns(timer_patterns, ARRAY_SIZE(timer_patterns));
    break;
  case CYCLE::CLOCK:
  default:
    if (config.led_configuration == LED_MODE::RANDOM)
    {
      led_driver->set_patterns(random_patterns, ARRAY_SIZE(random_patterns));
    }
    else
    {
      led_driver->set_patterns(clock_patterns, ARRAY_SIZE(clock_patterns));
    }
  }
}

void update_config_callback()
{
  if (config.leds && !led_driver->get_status())
  {
    led_driver->turn_on(false);
  }
  else if (!config.leds && led_driver->get_status())
  {
    led_driver->turn_off(false);
  }
  set_led_patterns(cycle);
}
