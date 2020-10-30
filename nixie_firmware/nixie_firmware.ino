#include "configuration.h"
#include "tube_driver.h"
#include "clock.h"
#include "leds.h"
#include "wifi.h"
#include "webserver.h"
#include "OTA.h"
#include "sensors.h"
#include "serial_parser.h"

void setup() {
  Serial.begin(9600);
  setup_configuration();
  setup_wifi();
  setup_OTA();
  setup_webserver();
  setup_clock();
  setup_leds();
  setup_tube();
  setup_sensors();
  setup_serial_parser();
}

bool flag = true;

void loop() {
  leds_loop();
  clock_loop(flag);
  //sensors_loop(!flag);
  wifi_loop();
  OTA_loop();
  webserver_loop();
  serial_parser_loop();

  float light_reading = get_light_sensor_reading() / 1.2;
  //Serial.println(light_reading);
  set_led_brightness(map(light_reading, 0, 1023, 0, 100));
  set_tube_brightness(light_reading);
}
