#include <EEPROM.h>

struct Config {
  int timezone = 0;
  bool h24 = true;
  bool celsius = true;
  char google_token[30] = "";
};

static Config config;

void setup_configuration()
{
  EEPROM.begin(512);
  EEPROM.get(0, config);
  Serial.println();
  Serial.println(config.timezone);
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

//WIFI
#define HOST_NAME "clonixie"
#define WIFI_SSID  "CloNixie"
#define WIFI_PASSWORD  "clonixie"
#define PORTAL_TIMEOUT  600
