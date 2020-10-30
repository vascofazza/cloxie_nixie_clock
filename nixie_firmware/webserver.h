#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer httpServer(8080);
ESP8266HTTPUpdateServer httpUpdater;

/*
   Remote sketch update server
*/
void setup_webserver(void) {
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println(F("WiFi failed, aborting."));
    return;
  }

  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 8080);
  Serial.printf_P(PSTR("UpdateServer ready! Open http://%s.local:8080/update in your browser\n"), HOST_NAME);
}

void webserver_loop(void) {
  if (WiFi.status() != WL_CONNECTED) {
    httpServer.handleClient();
    MDNS.update();
  }
}
