#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer httpServer(8080);
ESP8266HTTPUpdateServer httpUpdater;

/*
 * Remote sketch update server
 */
void setup_webserver(void) {
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi failed, aborting.");
    return;
  }

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  Serial.printf("UpdateServer ready! Open http://%s.local:8080/update in your browser\n", HOST_NAME);
}

void webserver_loop(void) {
  httpServer.handleClient();
}
