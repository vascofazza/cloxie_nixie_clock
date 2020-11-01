#include <Arduino.h>
#include <OneShotTimer.h>
#include "configuration.hpp"
#include "tube_driver.hpp"
#include "clock.hpp"
#include "leds.hpp"
#include "wifi.hpp"
#include "webserver.h"
#include "sensors.hpp"

ClockDriver *clock_driver;
TubeDriver *tube_driver;
LedDriver *led_driver;
SensorDriver *sensor_driver;

#include "serial_parser.h"

OneShotTimer cycle_handler;

#define NUM_CYCLES 4

enum CYCLE
{
  NONE = -1,
  CLOCK = 0,
  DATE = 1,
  TIMER = 2,
  TEMPERATURE = 3
};

int cycle = CYCLE::NONE;

void next_cycle();

void setup()
{
  Serial.begin(9600);
  setup_serial_parser();
  setup_configuration();
  setup_wifi();
  setup_webserver();
  tube_driver = new TubeDriver();
  clock_driver = new ClockDriver(tube_driver);
  led_driver = new LedDriver(tube_driver, NUM_LEDS);
  sensor_driver = new SensorDriver(tube_driver);

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
  case CYCLE::CLOCK:
    cycle_handler.OneShot(CLOCK_CYCLE, next_cycle);
    break;
  case CYCLE::DATE:
    cycle_handler.OneShot(DATE_CYCLE, next_cycle);
    break;
  case CYCLE::TIMER:
    if (!clock_driver->is_timer_set())
      next_cycle();
    cycle_handler.OneShot(TIMER_CYCLE, next_cycle);
    break;
  case CYCLE::TEMPERATURE:
    cycle_handler.OneShot(TEMP_CYCLE, next_cycle);
    break;
  }
   Serial.println("CYCLE");
}

void handle_loop()
{
  static bool timer_running = false;

  if (!config.adaptive_brightness)
  {
    led_driver->set_brightness(DEFAULT_BRIGHTNESS);
    tube_driver->set_brightness(DEFAULT_BRIGHTNESS);
  }
  else
  {
    float light_reading = sensor_driver->get_light_sensor_reading();
    led_driver->set_brightness(light_reading);
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
    cycle = CYCLE::TIMER - 1;
    next_cycle();
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
  case CYCLE::TIMER:
    clock_driver->show_timer(true);
    break;
  case CYCLE::TEMPERATURE:
    clock_driver->show_date(false);
    clock_driver->show_time(false);
    auto temp = sensor_driver->get_temperature_sensor_reading();
    tube_driver->display_temperature(temp);
    break;
  }
}

void loop()
{
  led_driver->loop();
  tube_driver->loop();
  clock_driver->loop();

  wifi_loop();
  webserver_loop();
  serial_parser_loop();

  handle_loop();
}
