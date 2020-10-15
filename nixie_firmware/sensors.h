#include <OneWire.h>
#include <DallasTemperature.h> // dallas DS18B20

// GPIO where the DS18B20 is connected to
#define ONE_WIRE_BUS D2
#define LIGHT_SENSOR_PIN A0

// Setup a oneWire instance to communicate with any OneWire devices
static OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature sensor
static DallasTemperature sensors(&oneWire);

int light_sensor_readings[NUM_OF_READINGS];

float last_temp_reading = 0;

void setup_sensors()
{
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  sensors.begin();
}

/*
 * Smoothing the analogue sensor reading
 */
float get_light_sensor_reading()
{
  static int idx = 0;
  static long total = 0;
  auto last_reading = analogRead(LIGHT_SENSOR_PIN);

  total -= light_sensor_readings[idx];
  total += last_reading;
  light_sensor_readings[idx] = last_reading;
  idx = (idx + 1) % NUM_OF_READINGS;
  
  float average_reading = (float)total / NUM_OF_READINGS;
  return average_reading;
}

void sensors_loop(bool display)
{
  Serial.print(F("Requesting temperatures..."));
  sensors.requestTemperatures();
  Serial.println(F("DONE"));
  auto light_sensor_reading = get_light_sensor_reading();
  float temperature_reading = config.celsius? sensors.getTempCByIndex(0): sensors.getTempFByIndex(0);
  if (temperature_reading != DEVICE_DISCONNECTED_C)
  {
    Serial.print(F("Temperature for the device 1 (index 0) is: "));
    Serial.println(temperature_reading);
    last_temp_reading = temperature_reading;
  }
  else
  {
    Serial.print(F("Cached data for Temperature is: "));
    Serial.println(last_temp_reading);
  }
  if (display)
  {
    display_temperature(last_temp_reading);
    Serial.print(F("Light sensor reading: "));
    Serial.println(light_sensor_reading);
  }
}
