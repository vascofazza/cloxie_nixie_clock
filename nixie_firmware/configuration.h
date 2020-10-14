#include <EEPROM.h>

//WIFI
#define HOST_NAME "clonixie"
#define WIFI_SSID  "CloNixie"
#define WIFI_PASSWORD  "clonixie"
#define PORTAL_TIMEOUT  600 //in seconds
#define WIFI_RECONNECT_DELAY 600 * 1000 //in millis
#define WIFI_RECONNECT_ATTEMPTS 6 //before a full reset

//LEDS
#define NUM_LEDS 6

//SENSORS
#define NUM_OF_READINGS 100

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
};

static Config config;

void printParams()
{
  Serial.println("Configuration:");
  Serial.print("\tTimeZone: ");
  Serial.println(config.timezone);
  Serial.print("\tH24/12: ");
  Serial.println(config.h24);
  Serial.print("\tC°/F°: ");
  Serial.println(config.celsius);
  Serial.print("\tGoogle Token: ");
  Serial.println(config.google_token);
  Serial.print("\tAdaptive Brightness: ");
  Serial.println(config.adaptive_brightness);
  Serial.print("\tBrightness Offset: ");
  Serial.println(config.brightness_offset);
  Serial.print("\tShutdown Threshold: ");
  Serial.println(config.shutdown_threshold);
  Serial.print("\Shutdown Delay: ");
  Serial.println(config.shutdown_delay);
}

void check_params()
{
  config.timezone = config.timezone > 267 || config.timezone < 0 ? 0 : config.timezone;
  //TODO add default values
}

void setup_configuration()
{
  EEPROM.begin(512);
  EEPROM.get(0, config);
  check_params();
  printParams();
}

void save_configuration()
{
  EEPROM.put(0, config);
  if (EEPROM.commit()) {
    Serial.println("EEPROM successfully committed");
  } else {
    Serial.println("ERROR! EEPROM commit failed");
  }
}
