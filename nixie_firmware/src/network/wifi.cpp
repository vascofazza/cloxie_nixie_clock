#include "wifi.hpp"

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
WiFiManagerParameter *termometer;
WiFiManagerParameter *date;
WiFiManagerParameter *depoisoning;
WiFiManagerParameter *clock_cycle;

void (*custom_callback)(void) = nullptr;

void (*calibration_callback)(AsyncWebServerRequest*) = nullptr;

static ClockDriver *_clock_driver = nullptr;

void setup_wifi(ClockDriver *clock, void (*callback)(void), void (*calib_callback)(AsyncWebServerRequest*))
{
  _clock_driver = clock;
  wifiManager.WiFiManagerInit();
  wifiManager.setFirmwareVersion(FIRMWARE_VERSION);
  if (callback != nullptr)
  {
    custom_callback = callback;
  }

  if (calib_callback != nullptr)
  {
    calibration_callback = calib_callback;
  }

  auto timezone_str = F("<br/><a href=\"/timezones\">TimeZone</a> (click for the full list)<input id='timezone_field' name='timezone_field' maxlength='20' value=''>");
  timezone_field = new WiFiManagerParameter(timezone_str, 30);
  wifiManager.addParameter(timezone_field);

  auto h24_radio_str = F("<br/><p>Time display mode:</p><input type='radio' name='h24_field' value='1' checked><label for='1'>24H</label><br><input type='radio' name='h24_field' value='0'><label for='0'>12H</label><br>");
  h24_field = new WiFiManagerParameter(h24_radio_str);
  wifiManager.addParameter(h24_field);

  auto blink_radio_str = F("<br/><p>Blink mode:</p><input type='radio' name='blink_field' value='0'><label for='0'>off</label><br><input type='radio' name='blink_field' value='1' checked><label for='1'>static</label><br><input type='radio' name='blink_field' value='2'><label for='2'>double static</label><br><input type='radio' name='blink_field' value='3'><label for='3'>fade out</label><br>");
  blink_field = new WiFiManagerParameter(blink_radio_str);
  wifiManager.addParameter(blink_field);

  auto temp_radio_str = F("<br/><p>Temperature scale:</p><input type='radio' name='temp_field' value='1' checked> C°<br><input type='radio' name='temp_field' value='0'> F°<br>");
  temp_field = new WiFiManagerParameter(temp_radio_str);
  wifiManager.addParameter(temp_field);

  auto adaptive_radio_str = F("<br/><p>Adaptive Brightness:</p><input type='radio' name='adaptive_field' value='1' checked> ON<br><input type='radio' name='adaptive_field' value='0'> OFF<br>");
  adaptive_field = new WiFiManagerParameter(adaptive_radio_str);
  wifiManager.addParameter(adaptive_field);

  auto termometer_str = F("<br/><p>Termometer:</p><input type='radio' name='termometer_field' value='1' checked><label for='1'>ON</label><br><input type='radio' name='termometer_field' value='0'><label for='0'>OFF</label><br>");
  termometer = new WiFiManagerParameter(termometer_str);
  wifiManager.addParameter(termometer);

  auto date_str = F("<br/><p>Date:</p><input type='radio' name='date_field' value='1' checked><label for='1'>ON</label><br><input type='radio' name='date_field' value='0'><label for='0'>OFF</label><br>");
  date = new WiFiManagerParameter(date_str);
  wifiManager.addParameter(date);

  auto leds_str = F("<br/><p>Leds:</p><input type='radio' name='leds_field' value='1' checked><label for='1'>ON</label><br><input type='radio' name='leds_field' value='0'><label for='0'>OFF</label><br>");
  leds = new WiFiManagerParameter(leds_str);
  wifiManager.addParameter(leds);

  auto leds_mode_str = F("<br/><p>Leds mode:</p><input type='radio' name='leds_mode_field' value='0' checked><label for='0'>static</label><br><input type='radio' name='leds_mode_field' value='1'><label for='1'>rotating</label><br><input type='radio' name='leds_mode_field' value='2'><label for='2'>random</label><br><br>");
  leds_mode = new WiFiManagerParameter(leds_mode_str);
  wifiManager.addParameter(leds_mode);

  brightness_offset = new WiFiManagerParameter(F("brightness_offset"), F("Brightness offset"), String(config.brightness_offset).c_str(), 4);
  wifiManager.addParameter(brightness_offset);

  shutdown_threshold = new WiFiManagerParameter(F("shutdown_threshold"), F("Shutdown threshold"), String(config.shutdown_threshold).c_str(), 4);
  wifiManager.addParameter(shutdown_threshold);

  sleep_hour = new WiFiManagerParameter(F("sleep_hour"), F("Sleep hour"), String(config.sleep_hour).c_str(), 3);
  wifiManager.addParameter(sleep_hour);

  wake_hour = new WiFiManagerParameter(F("wake_hour"), F("Wake hour"), String(config.wake_hour).c_str(), 3);
  wifiManager.addParameter(wake_hour);

  shutdown_delay = new WiFiManagerParameter(F("shutdown_delay"), F("Shutdown delay (s)"), String(config.shutdown_delay).c_str(), 7);
  wifiManager.addParameter(shutdown_delay);

  depoisoning = new WiFiManagerParameter(F("depoisoning_field"), F("Depoisoning trigger time (m)"), String(config.depoisoning / 60000).c_str(), 7);
  wifiManager.addParameter(depoisoning);

  clock_cycle = new WiFiManagerParameter(F("clock_cycle"), F("Clock cycle time (m)"), String(config.clock_cycle / 60000).c_str(), 7);
  wifiManager.addParameter(clock_cycle);

  wifiManager.setSaveParamsCallback(saveParamsCallback);
  wifiManager.setGetParameterCallback(getParamsCallback);

  std::vector<const char *> menu = {"wifi", "info", "param", "sep", "update", "restart"};
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

  wifiManager.setWebServerCallback(setup_additional_hooks);

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

void setup_additional_hooks()
{
  setup_ota_webserver(wifiManager.server.get());

  wifiManager.server.get()->on(PSTR("/timer_start"), std::bind(&start_timer, _clock_driver, std::placeholders::_1));
  wifiManager.server.get()->on(PSTR("/timer_pause"), std::bind(&pause_timer, _clock_driver, std::placeholders::_1));
  wifiManager.server.get()->on(PSTR("/timer_stop"), std::bind(&stop_timer, _clock_driver, std::placeholders::_1));

  wifiManager.server.get()->on(PSTR("/stopwatch_start"), std::bind(&start_stopwatch, _clock_driver, std::placeholders::_1));
  wifiManager.server.get()->on(PSTR("/stopwatch_pause"), std::bind(&pause_stopwatch, _clock_driver, std::placeholders::_1));
  wifiManager.server.get()->on(PSTR("/stopwatch_stop"), std::bind(&stop_stopwatch, _clock_driver, std::placeholders::_1));

  wifiManager.server.get()->on(PSTR("/timezones"), &get_timezones);
  wifiManager.server.get()->on(PSTR("/calibrate"), calibration_callback);
}

bool isConnected()
{
  return !(WiFi.status() != WL_CONNECTED || !hasIPaddr());
}

void wifi_loop()
{
  ota_webserver_loop();
  static elapsedSeconds reconnectionDelay; //declare global if you don't want it reset every time loop runs
  static int reconnection_attempt = WIFI_RECONNECT_ATTEMPTS;
  if (!isConnected())
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
      if (!wifiManager.isConfigPortalActive())
      {
        resetWiFi();
        activeDelay(1000);
      }

      if (wifiManager.autoConnect(false) == WL_CONNECTED && hasIPaddr())
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

  // connection was restored after starting config portal upon reboot
  if (WiFi.status() == WL_CONNECTED && wifiManager.isConfigPortalActive())
  {
    DEBUG_PRINTLN(F("Connection restored, stopping Config Portal."));
    wifiManager.stopConfigPortal();
    wifiManager.server.get()->end();
    wifiManager.server.get()->begin();
    wifiManager.startWebPortal();
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

void getParamsCallback(AsyncWebServerRequest *request)
{
  AsyncJsonResponse *response = new AsyncJsonResponse();
  JsonObject root = response->getRoot();
  root[F("timezone")] = String(config.timezone_name);
  root[F("h24")] = (int)config.h24;
  root[F("blink_mode")] = config.blink_mode;
  root[F("celsius")] = (int)config.celsius;
  root[F("adaptive_brightness")] = (int)config.adaptive_brightness;
  root[F("brightness_offset")] = config.brightness_offset;
  root[F("shutdown_delay")] = config.shutdown_delay;
  root[F("shutdown_threshold")] = config.shutdown_threshold;
  root[F("leds")] = (int)config.leds;
  root[F("led_configuration")] = config.led_configuration;
  root[F("sleep_hour")] = config.sleep_hour;
  root[F("wake_hour")] = config.wake_hour;
  root[F("termometer")] = (int)config.termometer;
  root[F("date")] = (int)config.date;
  root[F("depoisoning_field")] = config.depoisoning / 60000;
  root[F("clock_cycle")] = config.clock_cycle / 60000;
  root[F("uptime")] = wifiManager.getUpTime();
  root[F("fw_ver")] = String(FIRMWARE_VERSION);

  response->setLength();
  request->send(response);
}

void saveParamsCallback(AsyncWebServerRequest *request)
{
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
  DEBUG_PRINT(F("PARAM termometer_field = "));
  DEBUG_PRINTLN(getParam(request, F("termometer_field")));
  DEBUG_PRINT(F("PARAM date_field = "));
  DEBUG_PRINTLN(getParam(request, F("date_field")));
  DEBUG_PRINT(F("PARAM depoisoning_field = "));
  DEBUG_PRINTLN(depoisoning->getValue());
  DEBUG_PRINT(F("PARAM clock_cycle = "));
  DEBUG_PRINTLN(clock_cycle->getValue());

  int timezone_id = get_timezone_id(getParam(request, F("timezone_field")).c_str());
  if (timezone_id > 0)
  {
    DEBUG_PRINTLN(F("Changing timezone"));
    strcpy(config.timezone_name, getParam(request, F("timezone_field")).c_str());
    DEBUG_PRINTLN(config.timezone_name);
    config.timezone = timezone_id;
  }
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
  config.termometer = (bool)getParam(request, F("termometer_field")).toInt();
  config.date = (bool)getParam(request, F("date_field")).toInt();
  config.depoisoning = String(depoisoning->getValue()).toInt() * 60 * 1000; //minutes to millis
  config.clock_cycle = String(clock_cycle->getValue()).toInt() * 60 * 1000; //minutes to millis
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

void start_timer(ClockDriver *clock, AsyncWebServerRequest *request)
{
  auto arg_name = "interval";
  if (request->hasArg(arg_name))
  {
    String value = request->arg(arg_name);
    clock->start_timer(value.toInt() * 1000);
  }
  send_response(request);
}

void stop_timer(ClockDriver *clock, AsyncWebServerRequest *request)
{
  clock->reset_timer();
  send_response(request);
}

void pause_timer(ClockDriver *clock, AsyncWebServerRequest *request)
{
  clock->stop_timer();
  send_response(request);
}

void start_stopwatch(ClockDriver *clock, AsyncWebServerRequest *request)
{
  clock->start_stopwatch();
  send_response(request);
}

void pause_stopwatch(ClockDriver *clock, AsyncWebServerRequest *request)
{
  clock->stop_stopwatch();
  send_response(request);
}

void stop_stopwatch(ClockDriver *clock, AsyncWebServerRequest *request)
{
  clock->reset_stopwatch();
  send_response(request);
}