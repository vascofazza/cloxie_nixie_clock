#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <Arduino.h>
#include <EEPROM.h>

//CLOCK
#define CLOCK_CYCLE (1800 * 1000) //30m
#define DATE_CYCLE (60 * 1000)    //60s
#define TEMP_CYCLE (60 * 1000)    //60s
#define TIMER_CYCLE (60 * 1000)   //60s
#define NIGHT_TIME_HOUR_START 0
#define NIGHT_TIME_HOUR_END 7

//WIFI
#define HOST_NAME PSTR("cloxie")
#define WIFI_SSID PSTR("Cloxie")
#define WIFI_PASSWORD PSTR("cloxieclock")
#define PORTAL_TIMEOUT 600                //in seconds
#define WIFI_RECONNECT_DELAY (600 * 1000) //in millis
#define WIFI_RECONNECT_ATTEMPTS 6         //before a full reset

//TUBES
#define MIN_TUBE_BRIGHTNESS 80
#define MAX_TUBE_BRIGHNTESS 800
#define MIN_DOT_BRIGHTNESS 100
#define MAX_DOT_BRIGHNTESS 1000
#define CATHODE_POISONING_PREVENTION_TIME (120 * 1000)     //120 sec
#define CATHODE_POISONING_TRIGGER_TIME (60 * 60 + 30 * 60) //1h30m0s

//LEDS
#define NUM_LEDS 6
#define FRAMES_PER_SECOND 24
#define PATTERN_DELAY 600000
#define HUE_DELAY 250
#define MIN_LED_BRIGHTNESS 15
#define MAX_LED_BRIGHNTESS 100
#define LED_STATIC_HUE_VALUE 19

//SENSORS
#define NUM_OF_READINGS 100
#define ANALOG_READ_INTERVAL 50
#define TEMP_READ_INTERVAL 5000
#define MAX_LIGHT_READING_VAL 800

//MISC
#define TEST_TIME 10
#define DEFAULT_BRIGHTNESS PWMRANGE / 2
#define TRANSITION_TIME (10 * 1000) //10s
#define MAGIC_NUMBER 42
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//GITHUB
//#define GHOTA_DO_NOT_USE_ARDUINO_JSON
#define GHOTA_USER PSTR("vascofazza")
#define GHOTA_REPO PSTR("cloxie_nixie_clock")
#define GHOTA_CURRENT_TAG PSTR("0.0.3")
#define GHOTA_BIN_FILE PSTR("cloxie_d1_mini.bin")
#define GHOTA_ACCEPT_PRERELEASE 0
#define GHOTA_INTERVAL (3600 * 6 * 1000) //6h

struct Config
{
  int timezone;
  bool h24;
  bool leds;
  bool celsius;
  char google_token[30];
  int led_configuration;
  bool adaptive_brightness;
  int brightness_offset;  // light sensor offset
  int shutdown_threshold; // light sensor threshold for turning off the tubes
  unsigned int shutdown_delay;     // seconds before shutdown
  int blink_mode;
};

extern Config config;

void printParams();

void check_params();

void setup_configuration();

void save_configuration();

#endif