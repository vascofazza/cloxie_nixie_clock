#include "utils.hpp"

void activeDelay(unsigned long mills)
{
    elapsedMillis elapsed = 0;
    while (elapsed < mills)
    {
        system_soft_wdt_feed();
        delay(0);
    }
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
        response->printf("%s\n", name);
        system_soft_wdt_feed();
    }
    request->send(response);
}

int get_timezone_id(const char *timezone)
{
    String s_timezone = String(timezone);
    DEBUG_PRINT("Looking for timezone ");
    DEBUG_PRINTLN(s_timezone);
    auto size = zonedb::kZoneRegistrySize;
    for (int i = 0; i < size; i++)
    {
        auto name = zonedb::kZoneRegistry[i]->name;
        String s_name = String(name);
        if (s_timezone.equals(s_name))
            return i;
        system_soft_wdt_feed();
    }
    return -1;
}

void _parse_version(int result[3], const char* input)
{
    //1.4.2
    for(int idx = 0; idx < 3*2; idx+=2)
    {
        result[idx/2] = atoi(input + idx);
    }
}

bool less_than_version(const char* a,const char* b)
{
    int parsedA[3], parsedB[3];
    _parse_version(parsedA, a);
    _parse_version(parsedB, b);
    return std::lexicographical_compare(parsedA, parsedA + 3, parsedB, parsedB + 3);
}