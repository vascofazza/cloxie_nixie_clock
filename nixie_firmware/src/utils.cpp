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

void get_timezones(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream(PSTR("text/plain"));
    response->setCode(200);

    auto size = zonedb::kZoneRegistrySize;
    for (int i = 0; i < size; i++)
    {
        auto name = zonedb::kZoneRegistry[i]->name;
        DEBUG_PRINTLN(name);
        response->printf("%d\t%s\n", i, name);
    }
    request->send(response);
}