#include "wifi.hpp"

static bool portalRunning = false;

WiFiManagerParameter *google_token;
WiFiManagerParameter *timezone_field;
WiFiManagerParameter *h24_field;
WiFiManagerParameter *blink_field;
WiFiManagerParameter *temp_field;
WiFiManagerParameter *adaptive_field;
WiFiManagerParameter *brightness_offset;
WiFiManagerParameter *shutdown_threshold;
WiFiManagerParameter *shutdown_delay;
WiFiManagerParameter *leds;
WiFiManagerParameter *leds_mode;

void setup_wifi()
{
  timezone_field = new WiFiManagerParameter(F("<br/><label for='timezone_field'>TimeZone: </label><select name='timezone_field'><option value='163'>GMT</option><option value='164'>GMT+1</option><option value='165'>GMT+10</option><option value='166'>GMT+11</option><option value='167'>GMT+12</option><option value='168'>GMT+2</option><option value='169'>GMT+3</option><option value='170'>GMT+4</option><option value='171'>GMT+5</option><option value='172'>GMT+6</option><option value='173'>GMT+7</option><option value='174'>GMT+8</option><option value='175'>GMT+9</option><option value='176'>GMT-1</option><option value='177'>GMT-10</option><option value='178'>GMT-11</option><option value='179'>GMT-12</option><option value='180'>GMT-13</option><option value='181'>GMT-14</option><option value='182'>GMT-2</option><option value='183'>GMT-3</option><option value='184'>GMT-4</option><option value='185'>GMT-5</option><option value='186'>GMT-6</option><option value='187'>GMT-7</option><option value='188'>GMT-8</option><option value='189'>GMT-9</option><option value='190'>UTC</option></select>")); // custom html input
  wifiManager.addParameter(timezone_field);

  auto h24_radio_str = F("<br/><p>Time display mode:</p><input type='radio' name='h24_field' value='1' checked><label for='1'>24H</label><br><input type='radio' name='h24_field' value='0'><label for='0'>12H</label><br>");
  h24_field = new WiFiManagerParameter(h24_radio_str);
  wifiManager.addParameter(h24_field);

  auto blink_radio_str = F("<br/><p>Blink mode:</p><input type='radio' name='blink_field' value='0' checked><label for='0'>off</label><br><input type='radio' name='blink_field' value='1'><label for='1'>static</label><br><input type='radio' name='blink_field' value='2'><label for='2'>double static</label><br><input type='radio' name='blink_field' value='3'><label for='3'>fade out</label><br>");
  blink_field = new WiFiManagerParameter(blink_radio_str);
  wifiManager.addParameter(blink_field);

  auto temp_radio_str = F("<br/><label for='temp_field'>Temperature scale:</label><input type='radio' name='temp_field' value='1' checked>C°<br><input type='radio' name='temp_field' value='0'>F°<br>");
  temp_field = new WiFiManagerParameter(temp_radio_str);
  wifiManager.addParameter(temp_field);

  auto adaptive_radio_str = F("<br/><label for='adaptive_field'>Adaptive Brightness:</label><input type='radio' name='adaptive_field' value='1' checked>ON<br><input type='radio' name='adaptive_field' value='0'>OFF<br>");
  adaptive_field = new WiFiManagerParameter(adaptive_radio_str);
  wifiManager.addParameter(adaptive_field);

  auto leds_str = F("<br/><p>Leds:</p><input type='radio' name='leds_field' value='1' checked><label for='1'>ON</label><br><input type='radio' name='leds_field' value='0'><label for='0'>OFF</label><br>");
  leds = new WiFiManagerParameter(leds_str);
  wifiManager.addParameter(leds);

  auto leds_mode_str = F("<br/><p>Leds:</p><input type='radio' name='leds_mode_field' value='0' checked><label for='0'>static</label><br><input type='radio' name='leds_mode_field' value='1'><label for='1'>rotating</label><br><input type='radio' name='leds_mode_field' value='2'><label for='2'>random</label><br>");
  leds_mode = new WiFiManagerParameter(leds_mode_str);
  wifiManager.addParameter(leds_mode);

  shutdown_threshold = new WiFiManagerParameter(F("shutdown_threshold"), F("shutdown_threshold"), String(config.shutdown_threshold).c_str(), 10);
  wifiManager.addParameter(shutdown_threshold);

  brightness_offset = new WiFiManagerParameter(F("brightness_offset"), F("brightness_offset"), String(config.brightness_offset).c_str(), 10);
  wifiManager.addParameter(brightness_offset);

  shutdown_delay = new WiFiManagerParameter(F("shutdown_delay"), F("shutdown_delay"), String(config.shutdown_delay).c_str(), 10);
  wifiManager.addParameter(shutdown_delay);

  google_token = new WiFiManagerParameter(F("google_token"), F("google_token"), config.google_token, 40);
  wifiManager.addParameter(google_token);

  wifiManager.setSaveParamsCallback(saveParamsCallback);

  std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
  wifiManager.setMenu(menu);

  wifiManager.setDebugOutput(false);
  // Configuration portal stays up for this amount of time on powerup
  wifiManager.setConfigPortalTimeout(PORTAL_TIMEOUT);

  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(false);

  wifiManager.setConfigPortalBlocking(true);

  //tries to connect to last known settings
  //if it does not connect it starts an access point and goes into a blocking loop awaiting configuration
  Serial.println(F("Connecting to AP"));
  if (!wifiManager.autoConnect(WIFI_SSID, WIFI_PASSWORD))
  {
    Serial.println(F("AP Error Resetting ESP8266"));
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  MDNS.begin(HOST_NAME);
  MDNS.addService(PSTR("http"), PSTR("tcp"), 80);
  wifiManager.setConfigPortalBlocking(false);
}

void wifi_loop()
{
  static elapsedMillis reconnectionDelay; //declare global if you don't want it reset every time loop runs
  static int reconnection_attempt = WIFI_RECONNECT_ATTEMPTS;
  if (WiFi.status() != WL_CONNECTED)
  {
    if (reconnectionDelay > WIFI_RECONNECT_DELAY)
    {
      if (WiFi.waitForConnectResult() == WL_CONNECTED)
      {
        Serial.println(F("WiFi connection restored."));
        reconnection_attempt = WIFI_RECONNECT_ATTEMPTS;
      }
      else if (reconnection_attempt-- == 0)
      {
        Serial.println(F("AP Error Resetting ESP8266"));
        delay(3000);
        ESP.reset();
        delay(5000);
      }
      reconnectionDelay = 0;
    }
    Serial.print(WiFi.status());
    Serial.println(F("WiFi is not connected, aborting."));
  }

  //handle disconnection - reboot and AP setup
  if (portalRunning)
  {
    wifiManager.process();
  }
  else
  {
    Serial.println(F("Starting Portal"));
    wifiManager.startWebPortal();
    portalRunning = true;
  }
  MDNS.update();
}

void saveParamsCallback()
{
  Serial.print(F("PARAM google_token = "));
  Serial.println(google_token->getValue());
  Serial.print(F("PARAM timezone_field = "));
  Serial.println(getParam(F("timezone_field")));
  Serial.print(F("PARAM h24_field = "));
  Serial.println(getParam(F("h24_field")));
  Serial.print(F("PARAM blink_field = "));
  Serial.println(getParam(F("blink_field")));
  Serial.print(F("PARAM temp_field = "));
  Serial.println(getParam(F("temp_field")));
  Serial.print(F("PARAM adaptive_field = "));
  Serial.println(getParam(F("adaptive_field")));
  Serial.print(F("PARAM brightness_offset = "));
  Serial.println(brightness_offset->getValue());
  Serial.print(F("PARAM shutdown_threshold = "));
  Serial.println(shutdown_threshold->getValue());
  Serial.print(F("PARAM shutdown_delay = "));
  Serial.println(shutdown_delay->getValue());
  Serial.print(F("PARAM leds = "));
  Serial.println(getParam(F("leds_field")));
  Serial.print(F("PARAM leds_mode = "));
  Serial.println(getParam(F("leds_mode_field")));

  strcpy(config.google_token, google_token->getValue());
  config.timezone = getParam(F("timezone_field")).toInt();
  config.h24 = (bool)getParam(F("h24_field")).toInt();
  config.blink_mode = getParam(F("blink_field")).toInt();
  config.celsius = (bool)getParam(F("temp_field")).toInt();
  config.adaptive_brightness = (bool)getParam(F("adaptive_field")).toInt();
  config.brightness_offset = String(brightness_offset->getValue()).toInt();
  config.shutdown_delay = String(shutdown_delay->getValue()).toInt();
  config.shutdown_threshold = String(shutdown_threshold->getValue()).toInt();
  config.leds = (bool)getParam(F("leds_field")).toInt();
  config.led_configuration = getParam(F("leds_mode_field")).toInt();
  save_configuration();
}

String getParam(String name)
{
  //read parameter from server, for customhmtl input
  String value;
  if (wifiManager.server->hasArg(name))
  {
    value = wifiManager.server->arg(name);
  }
  return value;
}

void reset_wifi_settings()
{
  wifiManager.resetSettings();
  ESP.reset();
}

void wifi_free_resources()
{
  wifiManager.stopWebPortal();
  free(google_token);
  free(timezone_field);
  free(h24_field);
  free(blink_field);
  free(temp_field);
  free(adaptive_field);
  free(brightness_offset);
  free(shutdown_threshold);
  free(shutdown_delay);
  free(leds);
  free(leds_mode);
}
