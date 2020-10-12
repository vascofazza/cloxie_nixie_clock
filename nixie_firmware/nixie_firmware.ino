#include "leds.h"
#include "wifi.h"
#include "ntp.h"

#define STROBE D1
#define LATCH D4
#define CLOCK D5
#define DATA D3


#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = D2;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  setup_wifi();
  setup_ntp();
  setup_leds();

  pinMode(A0, INPUT);
  pinMode(STROBE, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  sensors.begin();

}

int i = 0;
bool flag = true;
byte num = 0;

void loop() {
  leds_loop();
  ntp_loop();
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
  analogWrite(STROBE, i);
  delay(50);



  // the LEDs don't change while you're sending in bits:
  digitalWrite(LATCH, LOW);
  // shift out the bits:
  shiftOut(DATA, CLOCK, LSBFIRST, (byte)(num << 4));
  //take the latch pin high so the LEDs will light up:
  digitalWrite(LATCH, HIGH);
  num ++;
  num %= 10;
  Serial.println(analogRead(A0));
  float temperatureC = sensors.getTempCByIndex(0);
  if (temperatureC != DEVICE_DISCONNECTED_C)
  {
    sensors.requestTemperatures();
    
    //float temperatureF = sensors.getTempFByIndex(0);
    Serial.print(temperatureC);
    Serial.println("ºC");
  }
}
