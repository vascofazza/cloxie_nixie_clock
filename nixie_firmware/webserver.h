#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer httpServer(8080);
ESP8266HTTPUpdateServer httpUpdater;

void setup_webserver(void) {
  //WiFi.mode(WIFI_AP_STA);
  //WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi failed, aborting.");
    return;
  }

  //MDNS.begin(HOST_NAME);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  //MDNS.addService("http", "tcp", 8080);
  Serial.printf("UpdateServer ready! Open http://%s.local:8080/update in your browser\n", HOST_NAME);
}

void webserver_loop(void) {
  httpServer.handleClient();
  //MDNS.update();
}
