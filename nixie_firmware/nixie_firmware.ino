#include "configuration.h"
#include "tube_driver.h"
#include "clock.h"
#include "leds.h"
#include "wifi.h"
#include "webserver.h"
#include "sensors.h"
#include "serial_parser.h"

void setup() {
  Serial.begin(9600);
  setup_configuration();
  setup_wifi();
  setup_webserver();
  setup_clock();
  setup_leds();
  setup_tube();
  setup_sensors();
  setup_serial_parser();
}

int i = 0;
bool flag = true;

void loop() {
  leds_loop();
  clock_loop(flag);
  sensors_loop(!flag);
  wifi_loop();
  webserver_loop();
  serial_parser_loop();
  if (i < 1024 and flag)
  {
    i += 100;
  }
  else
  {
    flag = false;
    i -= 100;
    if (i == 100)
    {
      flag = true;
    }
  }
  set_brightness(i);
  delay(50);
}
