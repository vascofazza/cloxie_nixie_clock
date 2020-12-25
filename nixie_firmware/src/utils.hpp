#ifndef UTILS_HPP
#define UTILS_HPP

#include <elapsedMillis.h>
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"

void activeDelay(unsigned long mills);

void send_response(AsyncWebServerRequest *request);

#endif