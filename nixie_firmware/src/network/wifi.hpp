#ifndef WIFI_HPP
#define WIFI_HPP

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <elapsedMillis.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"
#include "ota_webserver.hpp"
#include "../drivers/clock.hpp"
#include "../configuration.hpp"
#include "../utils.hpp"

static WiFiManager wifiManager;

String getParam(AsyncWebServerRequest *, String);

void getParamsCallback(AsyncWebServerRequest *);

void saveParamsCallback(AsyncWebServerRequest *);

void setup_wifi(ClockDriver *, void (*)());

void wifi_loop();

void reset_wifi_settings();

void resetWiFi();

void initWiFi();

bool hasIPaddr();

bool isConnected();

void postSaveFunction();

void setup_additional_hooks();

void start_timer(ClockDriver *, AsyncWebServerRequest *);

void pause_timer(ClockDriver *, AsyncWebServerRequest *);

void stop_timer(ClockDriver *, AsyncWebServerRequest *);

void start_stopwatch(ClockDriver *, AsyncWebServerRequest *);

void pause_stopwatch(ClockDriver *, AsyncWebServerRequest *);

void stop_stopwatch(ClockDriver *, AsyncWebServerRequest *);

#endif