#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <Arduino.h>
#include <EEPROM.h>

#define FIRMWARE_VERSION PSTR("1.0.0")

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(x, y) Serial.printf(x, y)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(x, y)
#endif

//CLOCK
#define CLOCK_CYCLE (1800 * 1000) //30m
#define DATE_CYCLE (60 * 1000)    //60s
#define TEMP_CYCLE (60 * 1000)    //60s
#define TIMER_CYCLE (60 * 1000)   //60s

//WIFI
#define HOST_NAME PSTR("cloxie")
#define WIFI_SSID PSTR("Cloxie")
#define WIFI_PASSWORD PSTR("cloxieclock")
#define PORTAL_TIMEOUT 600        //in seconds
#define WIFI_RECONNECT_DELAY 600  //10mins
#define WIFI_RECONNECT_ATTEMPTS 6 //before a full reset

//TUBES
#define MIN_TUBE_BRIGHTNESS 80
#define MAX_TUBE_BRIGHNTESS 700
#define MAX_DOT_BRIGHNTESS 1000
#define BRIGTHNESS_REFRESH 100
#define CATHODE_POISONING_PREVENTION_TIME (60 * 5 * 1000)         //5m
#define CATHODE_POISONING_TRIGGER_TIME (60 * 60 + 30 * 60) * 1000 //1h30m0s

//LEDS
#define NUM_LEDS 6
#define FRAMES_PER_SECOND 30
#define PATTERN_DELAY 600000
#define HUE_DELAY 250
#define MIN_LED_BRIGHTNESS 15
#define MAX_LED_BRIGHNTESS 100
#define LED_STATIC_HUE_VALUE 19

//SENSORS
#define NUM_OF_READINGS 100
#define ANALOG_READ_INTERVAL 50
#define TEMP_READ_INTERVAL 1000
#define MAX_LIGHT_READING_VAL 800

//MISC
#define DEFAULT_BRIGHTNESS PWMRANGE / 2
#define TRANSITION_TIME (10 * 1000) //10s
#define TURN_ON_OFF_TIME (5 * 1000) //5s
#define MAGIC_NUMBER 42
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//GITHUB
#define GHOTA_DO_NOT_USE_ARDUINO_JSON
#define GHOTA_USER PSTR("vascofazza")
#define GHOTA_REPO PSTR("cloxie_nixie_clock")
#define GHOTA_CURRENT_TAG FIRMWARE_VERSION
#define GHOTA_BIN_FILE PSTR("cloxie_d1_mini.bin")
#define GHOTA_ACCEPT_PRERELEASE 0
#define GHOTA_INTERVAL 60000 //(3600 * 6 * 1000) //6h

struct Config
{
  int timezone;
  bool h24;
  bool leds;
  bool celsius;
  //char google_token[30];
  int led_configuration;
  bool adaptive_brightness;
  int brightness_offset;       // light sensor offset
  int shutdown_threshold;      // light sensor threshold for turning off the tubes
  unsigned int shutdown_delay; // seconds before shutdown
  int blink_mode;
  int sleep_hour;
  int wake_hour;
  bool termometer;
  bool date;
};

extern Config config;

void printParams();

void check_params();

void setup_configuration();

void save_configuration();

#endif