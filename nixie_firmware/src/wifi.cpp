#include "wifi.hpp"

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
WiFiManagerParameter *sleep_hour;
WiFiManagerParameter *wake_hour;

void (*custom_callback)(void) = nullptr;

void setup_wifi(void (*callback)(void))
{
  wifiManager.setFirmwareVersion(FIRMWARE_VERSION);
  if (callback != nullptr)
  {
    custom_callback = callback;
  }

  timezone_field = new WiFiManagerParameter(F("<br/><label for='timezone_field'>TimeZone: </label><select name='timezone_field'><option value='163'>GMT</option><option value='164'>GMT+1</option><option value='165'>GMT+10</option><option value='166'>GMT+11</option><option value='167'>GMT+12</option><option value='168'>GMT+2</option><option value='169'>GMT+3</option><option value='170'>GMT+4</option><option value='171'>GMT+5</option><option value='172'>GMT+6</option><option value='173'>GMT+7</option><option value='174'>GMT+8</option><option value='175'>GMT+9</option><option value='176'>GMT-1</option><option value='177'>GMT-10</option><option value='178'>GMT-11</option><option value='179'>GMT-12</option><option value='180'>GMT-13</option><option value='181'>GMT-14</option><option value='182'>GMT-2</option><option value='183'>GMT-3</option><option value='184'>GMT-4</option><option value='185'>GMT-5</option><option value='186'>GMT-6</option><option value='187'>GMT-7</option><option value='188'>GMT-8</option><option value='189'>GMT-9</option><option value='190'>UTC</option></select>")); // custom html input
  wifiManager.addParameter(timezone_field);

  auto h24_radio_str = F("<br/><p>Time display mode:</p><input type='radio' name='h24_field' value='1' checked><label for='1'>24H</label><br><input type='radio' name='h24_field' value='0'><label for='0'>12H</label><br>");
  h24_field = new WiFiManagerParameter(h24_radio_str);
  wifiManager.addParameter(h24_field);

  auto blink_radio_str = F("<br/><p>Blink mode:</p><input type='radio' name='blink_field' value='0'><label for='0'>off</label><br><input type='radio' name='blink_field' value='1' checked><label for='1'>static</label><br><input type='radio' name='blink_field' value='2'><label for='2'>double static</label><br><input type='radio' name='blink_field' value='3'><label for='3'>fade out</label><br>");
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

  brightness_offset = new WiFiManagerParameter(F("brightness_offset"), F("brightness_offset"), String(config.brightness_offset).c_str(), 10);
  wifiManager.addParameter(brightness_offset);

  shutdown_threshold = new WiFiManagerParameter(F("shutdown_threshold"), F("shutdown_threshold"), String(config.shutdown_threshold).c_str(), 10);
  wifiManager.addParameter(shutdown_threshold);

  sleep_hour = new WiFiManagerParameter(F("sleep_hour"), F("sleep_hour"), String(config.sleep_hour).c_str(), 3);
  wifiManager.addParameter(sleep_hour);

  wake_hour = new WiFiManagerParameter(F("wake_hour"), F("wake_hour"), String(config.wake_hour).c_str(), 3);
  wifiManager.addParameter(wake_hour);

  shutdown_delay = new WiFiManagerParameter(F("shutdown_delay"), F("shutdown_delay"), String(config.shutdown_delay).c_str(), 10);
  wifiManager.addParameter(shutdown_delay);

  google_token = new WiFiManagerParameter(F("google_token"), F("google_token"), config.google_token, 40);
  wifiManager.addParameter(google_token);

  wifiManager.setSaveParamsCallback(saveParamsCallback);

  std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart"};
  wifiManager.setMenu(menu);

#ifdef DEBUG
  wifiManager.setDebugOutput(true);
#else
  wifiManager.setDebugOutput(false);
#endif

  // Configuration portal stays up for this amount of time on powerup
  wifiManager.setConfigPortalTimeout(PORTAL_TIMEOUT);

  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(false);

  wifiManager.setConfigPortalBlocking(false);

  wifiManager.setSaveConfigCallback(postSaveFunction);

  //tries to connect to last known settings
  //if it does not connect it starts an access point and goes into a blocking loop awaiting configuration
  DEBUG_PRINTLN(F("Connecting to AP"));
  if (!wifiManager.autoConnect(WIFI_SSID, WIFI_PASSWORD))
  {
    DEBUG_PRINTLN(F("Starting config portal."));
  }
  MDNS.begin(HOST_NAME);
  MDNS.addService(PSTR("http"), PSTR("tcp"), 80);
}

void wifi_loop()
{
  static elapsedSeconds reconnectionDelay; //declare global if you don't want it reset every time loop runs
  static int reconnection_attempt = WIFI_RECONNECT_ATTEMPTS;
  if ((WiFi.status() != WL_CONNECTED || !hasIPaddr()) && !wifiManager.isConfigPortalActive())
  {
#ifdef DEBUG
    static elapsedMillis deb_mils;
    if (deb_mils > 1000)
    {
      DEBUG_PRINT(F("WiFi connection lost. Reconnecting in... "));
      DEBUG_PRINTLN(WIFI_RECONNECT_DELAY - reconnectionDelay);
      deb_mils = 0;
    }
#endif
    if (reconnectionDelay > WIFI_RECONNECT_DELAY)
    {
      DEBUG_PRINTLN(F("WiFi connection issue, resetting module."));
      resetWiFi();
      wifi_free_resources();
      activeDelay(1000);
      setup_wifi(nullptr);
      activeDelay(1000);

      if (WiFi.waitForConnectResult() == WL_CONNECTED && hasIPaddr())
      {
        DEBUG_PRINTLN(F("WiFi connection restored."));
        reconnection_attempt = WIFI_RECONNECT_ATTEMPTS;
      }
      else if (reconnection_attempt-- == 0)
      {
        DEBUG_PRINTLN(F("AP Error Resetting ESP8266"));
        activeDelay(3000);
        ESP.reset();
        activeDelay(5000);
      }
      reconnectionDelay = 0;
    }
  }

  //handle disconnection - reboot and AP setup
  if (wifiManager.isConfigPortalActive() || wifiManager.isWebPortalActive())
  {
    wifiManager.process();
  }
  else
  {
    DEBUG_PRINTLN(F("Starting Portal"));
    wifiManager.startWebPortal();
  }
  MDNS.update();
}

void saveParamsCallback(AsyncWebServerRequest *request)
{
  DEBUG_PRINT(F("PARAM google_token = "));
  DEBUG_PRINTLN(google_token->getValue());
  DEBUG_PRINT(F("PARAM timezone_field = "));
  DEBUG_PRINTLN(getParam(request, F("timezone_field")));
  DEBUG_PRINT(F("PARAM h24_field = "));
  DEBUG_PRINTLN(getParam(request, F("h24_field")));
  DEBUG_PRINT(F("PARAM blink_field = "));
  DEBUG_PRINTLN(getParam(request, F("blink_field")));
  DEBUG_PRINT(F("PARAM temp_field = "));
  DEBUG_PRINTLN(getParam(request, F("temp_field")));
  DEBUG_PRINT(F("PARAM adaptive_field = "));
  DEBUG_PRINTLN(getParam(request, F("adaptive_field")));
  DEBUG_PRINT(F("PARAM brightness_offset = "));
  DEBUG_PRINTLN(brightness_offset->getValue());
  DEBUG_PRINT(F("PARAM sleep_hour = "));
  DEBUG_PRINTLN(sleep_hour->getValue());
  DEBUG_PRINT(F("PARAM wake_hour = "));
  DEBUG_PRINTLN(wake_hour->getValue());
  DEBUG_PRINT(F("PARAM shutdown_threshold = "));
  DEBUG_PRINTLN(shutdown_threshold->getValue());
  DEBUG_PRINT(F("PARAM shutdown_delay = "));
  DEBUG_PRINTLN(shutdown_delay->getValue());
  DEBUG_PRINT(F("PARAM leds = "));
  DEBUG_PRINTLN(getParam(request, F("leds_field")));
  DEBUG_PRINT(F("PARAM leds_mode = "));
  DEBUG_PRINTLN(getParam(request, F("leds_mode_field")));

  strcpy(config.google_token, google_token->getValue());
  config.timezone = getParam(request, F("timezone_field")).toInt();
  config.h24 = (bool)getParam(request, F("h24_field")).toInt();
  config.blink_mode = getParam(request, F("blink_field")).toInt();
  config.celsius = (bool)getParam(request, F("temp_field")).toInt();
  config.adaptive_brightness = (bool)getParam(request, F("adaptive_field")).toInt();
  config.brightness_offset = String(brightness_offset->getValue()).toInt();
  config.shutdown_delay = (unsigned int)String(shutdown_delay->getValue()).toInt();
  config.shutdown_threshold = String(shutdown_threshold->getValue()).toInt();
  config.leds = (bool)getParam(request, F("leds_field")).toInt();
  config.led_configuration = getParam(request, F("leds_mode_field")).toInt();
  config.sleep_hour = String(sleep_hour->getValue()).toInt();
  config.wake_hour = String(wake_hour->getValue()).toInt();
  save_configuration();
  if (custom_callback != nullptr)
  {
    custom_callback();
  }
}

String getParam(AsyncWebServerRequest *request, String name)
{
  //read parameter from server, for customhmtl input
  String value;
  if (request->hasArg(name.c_str()))
  {
    value = request->arg(name.c_str());
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

void postSaveFunction()
{
  ESP.reset();
}

// workaround for https://github.com/esp8266/Arduino/issues/7432
void initWiFi()
{
  WiFi.persistent(true);
  WiFi.setAutoReconnect(false);
  // The WiFi.disconnect() ensures that the WiFi is working correctly. If this is not done before receiving WiFi connections,
  // those WiFi connections will take a long time to make or sometimes will not work at all.
  wifiManager.disconnect();
  WiFi.mode(WIFI_OFF);
}

// ********************************************************************************
// Disconnect from Wifi AP
// ********************************************************************************
void _WifiDisconnect()
{
  wifiManager.disconnect();
  /* ETS_UART_INTR_DISABLE();
  wifi_station_disconnect();
  ETS_UART_INTR_ENABLE(); */
}

void resetWiFi()
{
  _WifiDisconnect();
  initWiFi();
}

bool hasIPaddr()
{
  bool configured = false;

  for (auto addr : addrList)
  {
    if ((configured = (!addr.isLocal() && (addr.ifnumber() == STATION_IF))))
    {
      break;
    }
  }
  return configured;
}