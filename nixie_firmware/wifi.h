#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <elapsedMillis.h> //https://github.com/pfeerick/elapsedMillis

elapsedMillis reconnectionDelay; //declare global if you don't want it reset every time loop runs
int reconnection_attempt = WIFI_RECONNECT_ATTEMPTS;

WiFiManager wifiManager;
bool portalRunning = false;

WiFiManagerParameter google_token;
WiFiManagerParameter timezone_field; // global param ( for non blocking w params )

static const char timezone_str[] PROGMEM = "<br/><label for='timezone_field'>TimeZone</label><select name='timezone_field'><option value='163'>GMT</option><option value='164'>GMT+1</option><option value='165'>GMT+10</option><option value='166'>GMT+11</option><option value='167'>GMT+12</option><option value='168'>GMT+2</option><option value='169'>GMT+3</option><option value='170'>GMT+4</option><option value='171'>GMT+5</option><option value='172'>GMT+6</option><option value='173'>GMT+7</option><option value='174'>GMT+8</option><option value='175'>GMT+9</option><option value='176'>GMT-1</option><option value='177'>GMT-10</option><option value='178'>GMT-11</option><option value='179'>GMT-12</option><option value='180'>GMT-13</option><option value='181'>GMT-14</option><option value='182'>GMT-2</option><option value='183'>GMT-3</option><option value='184'>GMT-4</option><option value='185'>GMT-5</option><option value='186'>GMT-6</option><option value='187'>GMT-7</option><option value='188'>GMT-8</option><option value='189'>GMT-9</option><option value='190'>UTC</option></select>";

String getParam(String name) {
  //read parameter from server, for customhmtl input
  String value;
  if (wifiManager.server->hasArg(name)) {
    value = wifiManager.server->arg(name);
  }
  return value;
}

void saveParamsCallback () {
  Serial.println("Get Params:");
  Serial.print(google_token.getID());
  Serial.print(" : ");
  Serial.println(google_token.getValue());
  Serial.println("PARAM timezone_field = " + getParam("timezone_field"));
  strcpy(config.google_token, google_token.getValue());
  config.timezone = getParam("timezone_field").toInt();
  //TODO handle other parameters
  save_configuration();
}

void setup_wifi()
{
  new (&google_token) WiFiManagerParameter("google_token", "google_token", config.google_token, 40);
  wifiManager.addParameter(&google_token);

  new (&timezone_field) WiFiManagerParameter(timezone_str); // custom html input

  wifiManager.addParameter(&timezone_field);

  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setSaveParamsCallback(saveParamsCallback);

  std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
  wifiManager.setMenu(menu);

  //wifiManager.setHostname(HOST_NAME);
  wifiManager.setDebugOutput(true);
  // Configuration portal stays up for this amount of time on powerup
  wifiManager.setConfigPortalTimeout(PORTAL_TIMEOUT);

  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);

  //tries to connect to last known settings
  //if it does not connect it starts an access point and goes into a blocking loop awaiting configuration
  Serial.println("Connecting to AP");
  if (!wifiManager.autoConnect(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("AP Error Resetting ESP8266");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  MDNS.begin(HOST_NAME);
  MDNS.addService("http", "tcp", 80);
}

void wifi_loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    if (reconnectionDelay > WIFI_RECONNECT_DELAY)
    {
      if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        Serial.println("WiFi connection restored.");
        reconnection_attempt = WIFI_RECONNECT_ATTEMPTS;
        return;
      }
      else if (reconnection_attempt-- == 0)
      {
        Serial.println("AP Error Resetting ESP8266");
        delay(3000);
        ESP.reset();
        delay(5000);
      }
      reconnectionDelay = 0;
    }
    Serial.println("WiFi is not connected, aborting.");
    return;
  }

  //handle disconnection - reboot and AP setup
  if (portalRunning) {
    wifiManager.process();
  }
  else
  {
    Serial.println("Button Pressed, Starting Portal");
    wifiManager.startWebPortal();
    portalRunning = true;

  }
  MDNS.update();
}

void reset_wifi_settings()
{
  wifiManager.resetSettings();
  ESP.reset();
}
