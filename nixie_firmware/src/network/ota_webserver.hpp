#ifndef OTA_WEBSERVER_HPP
#define OTA_WEBSERVER_HPP

#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "../configuration.hpp"
#include "../utils.hpp"

void setup_ota_webserver(AsyncWebServer *server);

void ota_webserver_loop(void);

#endif