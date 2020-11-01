#ifndef WIFI_HPP
#define WIFI_HPP

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <elapsedMillis.h> //https://github.com/pfeerick/elapsedMillis
#include "configuration.hpp"

static WiFiManager wifiManager;

String getParam(String);

void saveParamsCallback ();

void setup_wifi();

void wifi_loop();

void reset_wifi_settings();

#endif