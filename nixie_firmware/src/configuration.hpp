#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <Arduino.h>
#include <EEPROM.h>

//CLOCK
#define CLOCK_CYCLE 1800 * 1000 //30m
#define DATE_CYCLE 60 * 1000    //60s
#define TEMP_CYCLE 60 * 1000    //60s
#define TIMER_CYCLE 60 * 1000   //60s

//WIFI
#define HOST_NAME PSTR("cloxie")
#define WIFI_SSID PSTR("Cloxie")
#define WIFI_PASSWORD PSTR("cloxieclock")
#define PORTAL_TIMEOUT 600              //in seconds
#define WIFI_RECONNECT_DELAY 600 * 1000 //in millis
#define WIFI_RECONNECT_ATTEMPTS 6       //before a full reset

//TUBES
#define MIN_TUBE_BRIGHTNESS 80
#define MAX_TUBE_BRIGHNTESS 700
#define MIN_DOT_BRIGHTNESS 100
#define MAX_DOT_BRIGHNTESS 1000
#define CATHODE_POISONING_PREVENTION_TIME 60 * 1000   //60 sec
#define CATHODE_POISONING_TRIGGER_TIME 3600 + 30 * 60 //1h30m0s

//LEDS
#define NUM_LEDS 6
#define FRAMES_PER_SECOND 24
#define PATTERN_DELAY 600000
#define HUE_DELAY 250
#define MIN_LED_BRIGHTNESS 5
#define MAX_LED_BRIGHNTESS 100

//SENSORS
#define NUM_OF_READINGS 100
#define ANALOG_READ_INTERVAL 50

//MISC
#define TEST_TIME 10
#define DEFAULT_BRIGHTNESS PWMRANGE / 2
#define TRANSITION_TIME 10 * 1000 //5s

//GITHUB
#define GHOTA_USER PSTR("vascofazza")
#define GHOTA_REPO PSTR("smart_nixie_clock")
#define GHOTA_CURRENT_TAG PSTR("0.0.1")
#define GHOTA_BIN_FILE PSTR("firmware.bin")
#define GHOTA_ACCEPT_PRERELEASE 0
#define GHOTA_INTERVAL 3600 * 6 * 1000 //6h


struct Config
{
  int timezone = 0;
  bool h24 = true;
  bool celsius = true;
  char google_token[30] = "";
  int led_configuration = 0;
  bool adaptive_brightness = true;
  int brightness_offset = 0;  // light sensor offset
  int shutdown_threshold = 0; // light sensor threshold for turning off the tubes
  int shutdown_delay = 0;     // seconds before shutdown
  int blink_mode = 0;
};

extern Config config;

void printParams();

void check_params();

void setup_configuration();

void save_configuration();

#endif