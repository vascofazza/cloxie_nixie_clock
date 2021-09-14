#include "configuration.hpp"

Config config{
    -1,
    true,
    true,
    true,
    1,
    true,
    0,
    10,
    60 * 30,
    1,
    0,
    7,
    true,
    true,
    CATHODE_POISONING_TRIGGER_TIME,
    CLOCK_CYCLE,
    "CET",
    "0.0.0",
    TRANSITION_TIME,
    LED_SHUTDOWN_THRESHOLD,
    MIN_TUBE_BRIGHTNESS,
    MAX_TUBE_BRIGHNTESS,
    MIN_LED_BRIGHTNESS,
    MAX_LED_BRIGHNTESS,
};

void printParams()
{
  DEBUG_PRINTLN(F("Configuration:"));
  DEBUG_PRINT(F("\tTimeZone: "));
  DEBUG_PRINTLN(config.timezone);
  DEBUG_PRINT(F("\tTimeZone name: "));
  DEBUG_PRINTLN(config.timezone_name);
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
  DEBUG_PRINT(F("\tGlobal shutdown Threshold: "));
  DEBUG_PRINTLN(config.shutdown_threshold);
  DEBUG_PRINT(F("\tLed shutdown Threshold: "));
  DEBUG_PRINTLN(config.led_off_threshold);
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
  DEBUG_PRINT(F("\tClock cycle interval: "));
  DEBUG_PRINTLN(config.clock_cycle);
  DEBUG_PRINT(F("\tTransition interval: "));
  DEBUG_PRINTLN(config.slot_duration);
  DEBUG_PRINT(F("\tMinimum led brightness: "));
  DEBUG_PRINTLN(config.min_led_brightness);
  DEBUG_PRINT(F("\tMaximum led brightness: "));
  DEBUG_PRINTLN(config.max_led_brightness);
  DEBUG_PRINT(F("\tMinimum tube brightness: "));
  DEBUG_PRINTLN(config.min_tube_brightness);
  DEBUG_PRINT(F("\tMaximum tube brightness: "));
  DEBUG_PRINTLN(config.max_tube_brightness);
}

void check_params()
{
  config.timezone = config.timezone > 300 || config.timezone < 0 ? -1 : config.timezone;
  if (config.timezone < 0 || !String(config.timezone_name).equals(String(zonedb::kZoneRegistry[config.timezone]->name)))
  {
    DEBUG_PRINTLN("Timezone do not match.");
    int timezone_id = get_timezone_id(config.timezone_name);
    DEBUG_PRINT("New timezone ID: ");
    DEBUG_PRINTLN(timezone_id);
    config.timezone = timezone_id < 0 ? 0 : timezone_id;
  }

  bool update = false;

  if (less_than_version(config.version, "1.5.0") && PWMRANGE != ANALOGRANGE)
  {
    DEBUG_PRINTLN(F("UPDATING params to version 1.5.0"));
    config.min_tube_brightness = map(config.min_tube_brightness, 0, ANALOGRANGE, 0, PWMRANGE);
    config.max_tube_brightness = map(config.max_tube_brightness, 0, ANALOGRANGE, 0, PWMRANGE);
    config.brightness_offset = map(config.brightness_offset, 0, ANALOGRANGE, 0, PWMRANGE);
    update = true;
  }

  config.brightness_offset = config.brightness_offset > PWMRANGE || config.brightness_offset < -PWMRANGE ? 0 : config.brightness_offset;
  config.shutdown_threshold = config.shutdown_threshold > ANALOGRANGE || config.shutdown_threshold < 0 ? 0 : config.shutdown_threshold;
  config.shutdown_delay = config.shutdown_delay > (7 * 3600) ? 60 : config.shutdown_delay;
  config.led_configuration = config.led_configuration > 2 || config.led_configuration < 0 ? 0 : config.led_configuration;
  config.blink_mode = config.blink_mode > 3 || config.blink_mode < 0 ? 0 : config.blink_mode;
  config.sleep_hour = config.sleep_hour > 23 || config.sleep_hour < 0 ? 0 : config.sleep_hour;
  config.wake_hour = config.wake_hour > 23 || config.wake_hour < 0 ? 7 : config.wake_hour;
  config.clock_cycle = config.clock_cycle < 60000 ? CLOCK_CYCLE : config.clock_cycle;
  config.depoisoning = config.depoisoning <= CATHODE_POISONING_PREVENTION_TIME ? 0 : config.depoisoning;
  config.min_tube_brightness = min(config.min_tube_brightness, (unsigned int) PWMRANGE);
  config.max_tube_brightness = min(config.max_tube_brightness, (unsigned int) PWMRANGE);
  config.min_led_brightness = min(config.min_led_brightness, 100u);
  config.max_led_brightness = min(config.max_led_brightness, 100u);

  if (less_than_version(config.version, "1.4.3"))
  {
    DEBUG_PRINTLN(F("UPDATING params to version 1.4.3"));
    config.slot_duration = TRANSITION_TIME;
    config.min_tube_brightness = MIN_TUBE_BRIGHTNESS;
    config.max_tube_brightness = MAX_TUBE_BRIGHNTESS;
    config.min_led_brightness = MIN_LED_BRIGHTNESS;
    config.max_led_brightness = MAX_LED_BRIGHNTESS;
    config.led_off_threshold = LED_SHUTDOWN_THRESHOLD;
    update = true;
  }

  if (update)
  {
    strcpy_P(config.version, FIRMWARE_VERSION);
    save_configuration();
  }
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