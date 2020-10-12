#include <ESP8266WiFi.h>


#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char * ssid = STASSID; // your network SSID (name)
const char * pass = STAPSK;  // your network password

void setup_wifi()
{
  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected!");
}
