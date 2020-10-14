#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
#define ONE_WIRE_BUS D2

// Setup a oneWire instance to communicate with any OneWire devices
static OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
static DallasTemperature sensors(&oneWire);

float last_reading = 0;

void setup_sensors()
{
  pinMode(A0, INPUT);
  sensors.begin();
}

void sensors_loop(bool display)
{
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");
  float temperatureC = sensors.getTempCByIndex(0);
  if (temperatureC != DEVICE_DISCONNECTED_C)
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(temperatureC);
    last_reading = temperatureC;
  }
  else
  { 
    Serial.print("Cached data for Temperature is: ");
    Serial.println(last_reading);
  }
  if (display)
  {
    display_temperature(last_reading);
    Serial.print("Light sensor reading: ");
    Serial.println(analogRead(A0));
  }
}
