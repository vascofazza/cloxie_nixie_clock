#include <Arduino.h>
#include <OneShotTimer.h>
#include "configuration.hpp"
#include "tube_driver.hpp"
#include "clock.hpp"
#include "leds_patterns.hpp"
#include "leds.hpp"
#include "wifi.hpp"
#include "webserver.h"
#include "sensors.hpp"
#include "ota.h"

ClockDriver *clock_driver;
TubeDriver *tube_driver;
LedDriver *led_driver;
SensorDriver *sensor_driver;

#include "serial_parser.h"

OneShotTimer cycle_handler;
EveryTimer ota_handler;

#define NUM_CYCLES 4

enum CYCLE
{
  NONE = -1,
  CLOCK = 0,
  DATE = 1,
  TEMPERATURE = 2,
  TIMER = 3,
};

int cycle = CYCLE::NONE;

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

void next_cycle();

void setup()
{
  Serial.begin(9600);

  DEBUG_PRINTLN("Cloxie Nixie Clock");
  DEBUG_PRINT("FW version: ");
  DEBUG_PRINTLN(GHOTA_CURRENT_TAG);

  setup_serial_parser();
  setup_configuration();
  initWiFi();
  setup_wifi(&next_cycle);
  start_webserver();

  tube_driver = new TubeDriver();
  clock_driver = new ClockDriver(tube_driver);
  led_driver = new LedDriver(tube_driver, NUM_LEDS, clock_patterns, ARRAY_SIZE(clock_patterns));
  sensor_driver = new SensorDriver(tube_driver);

  ota_handler.Every(GHOTA_INTERVAL, check_for_updates);

  cycle_handler.OneShot(0, next_cycle);

  // test the tubes
  tube_driver->run_test();
}

void next_cycle()
{
  tube_driver->cathode_poisoning_prevention(TRANSITION_TIME);
  cycle = (cycle + 1) % NUM_CYCLES;

  switch (cycle)
  {
  case CYCLE::DATE:
    DEBUG_PRINTLN(F("CYCLE DATE"));
    cycle_handler.OneShot(DATE_CYCLE, next_cycle);
    led_driver->set_patterns(date_patterns, ARRAY_SIZE(date_patterns));
    break;
  case CYCLE::TEMPERATURE:
    DEBUG_PRINTLN(F("CYCLE TEMP"));
    cycle_handler.OneShot(TEMP_CYCLE, next_cycle);
    led_driver->set_patterns(temp_patterns, ARRAY_SIZE(temp_patterns));
    break;
  case CYCLE::TIMER:
    DEBUG_PRINTLN(F("CYCLE TIMER"));
    if (clock_driver->is_timer_set())
    {
      cycle_handler.OneShot(TIMER_CYCLE, next_cycle);
      led_driver->set_patterns(timer_patterns, ARRAY_SIZE(timer_patterns));
      break;
    }
  case CYCLE::CLOCK:
  default:
    DEBUG_PRINTLN(F("CYCLE CLOCK"));
    cycle = CYCLE::CLOCK;
    cycle_handler.OneShot(CLOCK_CYCLE, next_cycle);
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

void handle_loop()
{
  static bool timer_running = false;

  if (!config.adaptive_brightness)
  {
    tube_driver->set_brightness(DEFAULT_BRIGHTNESS);
    if (config.leds)
    {
      led_driver->set_brightness(DEFAULT_BRIGHTNESS);
    }
  }
  else
  {
    float light_reading = sensor_driver->get_light_sensor_reading();
    if (config.leds)
    {
      led_driver->set_brightness(light_reading);
    }
    tube_driver->set_brightness(light_reading);
  }

  if (clock_driver->is_timer_running())
  {
    cycle_handler.Stop();
    cycle = CYCLE::TIMER;
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

  clock_driver->loop();

  wifi_loop();
  webserver_loop();
  serial_parser_loop();
  ota_handler.Update();

  bool hour_check = clock_driver->is_night_hours();

  //DEBUG_PRINTLN(sensor_driver->get_light_sensor_reading());
  //DEBUG_PRINTLN(analogRead(LIGHT_SENSOR_PIN));
  if ((config.adaptive_brightness && sensor_driver->get_light_sensor_reading() < config.shutdown_threshold) || hour_check)
  {
    if (shutdown_delay > config.shutdown_delay)
    {
      DEBUG_PRINTLN(F("Sleeping..."));
      tube_driver->turn_off();
      led_driver->turn_off();
      clock_driver->show_time(false);
      clock_driver->show_date(false);
      clock_driver->show_timer(false);
      yield();
      return;
    }
    DEBUG_PRINT(F("Going to sleep in: "));
    DEBUG_PRINTLN(config.shutdown_delay - shutdown_delay);
  }
  else
  {
    if (shutdown_delay > config.shutdown_delay)
    {
      DEBUG_PRINTLN(F("Waking up."));
      tube_driver->turn_on();
      led_driver->turn_on();
    }
    shutdown_delay = 0;
  }

  if (config.leds)
  {
    led_driver->loop();
  }
  else
  {
    led_driver->turn_off();
  }

  tube_driver->loop();
  handle_loop();
  yield();
}
