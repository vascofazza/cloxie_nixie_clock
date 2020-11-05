#include "configuration.hpp"

Config config
{
    163,
    true,
    true,
    true,
    "",
    1,
    true,
    0,
    10,
    60 * 30 * 1000, // 30mins shutdown delay
    1
};

void printParams()
{
  Serial.println(F("Configuration:"));
  Serial.print(F("\tTimeZone: "));
  Serial.println(config.timezone);
  Serial.print(F("\tH24/12: "));
  Serial.println(config.h24);
  Serial.print(F("\tC°/F°: "));
  Serial.println(config.celsius);
  Serial.print(F("\tGoogle Token: "));
  Serial.println(config.google_token);
  Serial.print(F("\tAdaptive Brightness: "));
  Serial.println(config.adaptive_brightness);
  Serial.print(F("\tBrightness Offset: "));
  Serial.println(config.brightness_offset);
  Serial.print(F("\tShutdown Threshold: "));
  Serial.println(config.shutdown_threshold);
  Serial.print(F("\tShutdown Delay: "));
  Serial.println(config.shutdown_delay);
  Serial.print(F("\tBlink Mode: "));
  Serial.println(config.blink_mode);
  Serial.print(F("\tLeds: "));
  Serial.println(config.leds);
  Serial.print(F("\tLed Configuration: "));
  Serial.println(config.led_configuration);
}

void check_params()
{
  config.timezone = config.timezone > 267 || config.timezone < 0 ? 0 : config.timezone;
  config.brightness_offset = config.brightness_offset > PWMRANGE || config.brightness_offset < -PWMRANGE ? 0 : config.brightness_offset;
  config.shutdown_threshold = config.shutdown_threshold > PWMRANGE || config.shutdown_threshold < 0 ? 0 : config.shutdown_threshold;
  config.shutdown_delay = config.shutdown_delay > 10000 || config.shutdown_delay < -1 ? 3600 : config.shutdown_delay;
  config.led_configuration = config.led_configuration > 2 || config.led_configuration < 0 ? 0 : config.led_configuration;
  config.blink_mode = config.blink_mode > 3 || config.blink_mode < 0 ? 0 : config.blink_mode;
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
  EEPROM.put(1, config);
  if (EEPROM.commit())
  {
    Serial.println(F("EEPROM successfully committed"));
  }
  else
  {
    Serial.println(F("ERROR! EEPROM commit failed"));
  }
}