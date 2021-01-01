#ifndef UTILS_HPP
#define UTILS_HPP

#include <elapsedMillis.h>
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <AceTime.h>
#include "configuration.hpp"

using namespace ace_time;
using namespace ace_time::clock;

void activeDelay(unsigned long mills);

void send_response(AsyncWebServerRequest*);

void get_timezones(AsyncWebServerRequest*);

#endif