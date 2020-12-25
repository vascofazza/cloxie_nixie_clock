#include "utils.hpp"

void activeDelay(unsigned long mills)
{
    elapsedMillis elapsed = 0;
    while (elapsed < mills)
        delay(0);
}

void send_response(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response = request->beginResponse(200, PSTR("text/plain"), PSTR("OK"));
    response->addHeader(PSTR("Connection"), PSTR("close"));
    request->send(response);
}