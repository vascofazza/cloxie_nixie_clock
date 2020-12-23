#include "configuration.hpp"

Config config{
    163,
    true,
    true,
    true,
    //"",
    1,
    true,
    0,
    10,
    60 * 30, // 30mins shutdown delay
    1,
    0,
    7,
    true,
    true,
    CATHODE_POISONING_TRIGGER_TIME
    };

void printParams()
{
  DEBUG_PRINTLN(F("Configuration:"));
  DEBUG_PRINT(F("\tTimeZone: "));
  DEBUG_PRINTLN(config.timezone);
  DEBUG_PRINT(F("\tH24/12: "));
  DEBUG_PRINTLN(config.h24);
  DEBUG_PRINT(F("\tC°/F°: "));
  DEBUG_PRINTLN(config.celsius);
  DEBUG_PRINT(F("\tAdaptive Brightness: "));
  DEBUG_PRINTLN(config.adaptive_brightness);
  DEBUG_PRINT(F("\tBrightness Offset: "));
  DEBUG_PRINTLN(config.brightness_offset);
  DEBUG_PRINT(F("\tSleep Hour: "));
  DEBUG_PRINTLN(config.sleep_hour);
  DEBUG_PRINT(F("\tWake Hour: "));
  DEBUG_PRINTLN(config.wake_hour);
  DEBUG_PRINT(F("\tShutdown Threshold: "));
  DEBUG_PRINTLN(config.shutdown_threshold);
  DEBUG_PRINT(F("\tShutdown Delay: "));
  DEBUG_PRINTLN(config.shutdown_delay);
  DEBUG_PRINT(F("\tBlink Mode: "));
  DEBUG_PRINTLN(config.blink_mode);
  DEBUG_PRINT(F("\tLeds: "));
  DEBUG_PRINTLN(config.leds);
  DEBUG_PRINT(F("\tLed Configuration: "));
  DEBUG_PRINTLN(config.led_configuration);
  DEBUG_PRINT(F("\tDepoisoning interval: "));
  DEBUG_PRINTLN(config.depoisoning);
}

void check_params()
{
  config.timezone = config.timezone > 267 || config.timezone < 0 ? 0 : config.timezone;
  config.brightness_offset = config.brightness_offset > PWMRANGE || config.brightness_offset < -PWMRANGE ? 0 : config.brightness_offset;
  config.shutdown_threshold = config.shutdown_threshold > PWMRANGE || config.shutdown_threshold < 0 ? 0 : config.shutdown_threshold;
  config.shutdown_delay = config.shutdown_delay > (7 * 3600) ? (60 * 30) : config.shutdown_delay;
  config.led_configuration = config.led_configuration > 2 || config.led_configuration < 0 ? 0 : config.led_configuration;
  config.blink_mode = config.blink_mode > 3 || config.blink_mode < 0 ? 0 : config.blink_mode;
  config.sleep_hour = config.sleep_hour > 23 || config.sleep_hour < 0 ? 0 : config.sleep_hour;
  config.wake_hour = config.wake_hour > 23 || config.wake_hour < 0 ? 7 : config.wake_hour;
  config.depoisoning = config.depoisoning < 60000 ? CATHODE_POISONING_TRIGGER_TIME : config.depoisoning;
}

void setup_configuration()
{
  EEPROM.begin(512);
  if (EEPROM.read(0) != MAGIC_NUMBER)
  {
    EEPROM.write(0, MAGIC_NUMBER);
    save_configuration();
  }
  EEPROM.get(1, config);
  check_params();
  printParams();
}

void save_configuration()
{
  check_params();
  EEPROM.put(1, config);
  if (EEPROM.commit())
  {
    DEBUG_PRINTLN(F("EEPROM successfully committed."));
  }
  else
  {
    DEBUG_PRINTLN(F("ERROR! EEPROM commit failed."));
  }
}