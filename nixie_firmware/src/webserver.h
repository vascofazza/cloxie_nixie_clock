#ifndef WEBSERVER_OTA_H
#define WEBSERVER_OTA_H

#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"

AsyncWebServer server(8080);

//flag to use from web update to reboot the ESP
bool shouldReboot = false;

void onRequest(AsyncWebServerRequest *request)
{
  //Handle Unknown Request
  request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  //Handle body
}

void start_webserver(void)
{
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    DEBUG_PRINTLN(F("WiFi failed, aborting."));
    return;
  }

  // respond to GET requests on URL /heap
  server.on(PSTR("/heap"), HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, PSTR("text/plain"), String(ESP.getFreeHeap()));
  });

  // Simple Firmware Update Form
  server.on(PSTR("/update"), HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, PSTR("text/html"), PSTR("<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"));
  });
  server.on(
      PSTR("/update"), HTTP_POST, [](AsyncWebServerRequest *request) {
    shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, PSTR("text/plain"), shouldReboot?PSTR("OK"):PSTR("FAIL"));
    response->addHeader(PSTR("Connection"), PSTR("close"));
    request->send(response); }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if(!index){
      DEBUG_PRINTF(PSTR("Update Start: %s\n"), filename.c_str());
      Update.runAsync(true);
      if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
        Update.printError(Serial);
      }
    }
    if(!Update.hasError()){
      if(Update.write(data, len) != len){
        Update.printError(Serial);
      }
    }
    if(final){
      if(Update.end(true)){
        DEBUG_PRINTF(PSTR("Update Success: %uB\n"), index+len);
      } else {
        Update.printError(Serial);
      }
    } });

  // attach filesystem root at URL /fs
  server.serveStatic(PSTR("/fs"), SPIFFS, PSTR("/"));

  // Catch-All Handlers
  // Any request that can not find a Handler that canHandle it
  // ends in the callbacks below.
  server.onNotFound(onRequest);
  server.onRequestBody(onBody);

  server.begin();
}

void stop_webserver()
{
  server.end();
}

void webserver_loop(void)
{
  if (shouldReboot)
  {
    DEBUG_PRINTLN(F("Rebooting..."));
    delay(100);
    ESP.restart();
  }
}

#endif