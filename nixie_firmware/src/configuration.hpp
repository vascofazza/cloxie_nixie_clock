#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <Arduino.h>
#include <EEPROM.h>

//CLOCK
#define CLOCK_CYCLE 40 * 1000//3600 //1h
#define DATE_CYCLE  10 * 1000//60
#define TEMP_CYCLE  10 * 1000//60

//WIFI
#define HOST_NAME "cloxie"
#define WIFI_SSID  "Cloxie"
#define WIFI_PASSWORD  "cloxieclock"
#define PORTAL_TIMEOUT  600 //in seconds
#define WIFI_RECONNECT_DELAY 600 * 1000 //in millis
#define WIFI_RECONNECT_ATTEMPTS 6 //before a full reset

//TUBES
#define MIN_TUBE_BRIGHTNESS 80
#define MAX_TUBE_BRIGHNTESS 700
#define MIN_DOT_BRIGHTNESS 100
#define MAX_DOT_BRIGHNTESS 1000
#define CATHODE_POISONING_PREVENTION_TIME 10 * 1000 //60 sec
#define CATHODE_POISONING_TRIGGER_TIME 60 //37200 //1h2m0s

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
#define DEFAULT_BRIGHTNESS PWMRANGE/2
#define TRANSITION_TIME 3 * 1000 //s

struct Config {
  int timezone = 0;
  bool h24 = true;
  bool celsius = true;
  char google_token[30] = "";
  int led_configuration = 0;
  bool adaptive_brightness = true;
  int brightness_offset = 0; // light sensor offset
  int shutdown_threshold = 0; // light sensor threshold for turning off the tubes
  int shutdown_delay = 0; // seconds before shutdown
  int blink_mode = 0;
};

extern Config config;

void printParams();

void check_params();

void setup_configuration();

void save_configuration();

#endif