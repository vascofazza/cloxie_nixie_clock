#ifndef WIFI_HPP
#define WIFI_HPP

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <elapsedMillis.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "ota_webserver.hpp"
#include "../configuration.hpp"
#include "../utils.hpp"

static WiFiManager wifiManager;

String getParam(AsyncWebServerRequest*, String);

void getParamsCallback(AsyncWebServerRequest*);

void saveParamsCallback(AsyncWebServerRequest*);

void setup_wifi(void (*)());

void wifi_loop();

void reset_wifi_settings();

void resetWiFi();

void initWiFi();

bool hasIPaddr();

void postSaveFunction();

#endif