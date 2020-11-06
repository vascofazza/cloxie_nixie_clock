#include "sensors.hpp"

SensorDriver::SensorDriver(TubeDriver *tube_driver)
{
  this->tube_driver = tube_driver;
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  // Setup a oneWire instance to communicate with any OneWire devices
  onewire_instance = new OneWire(ONE_WIRE_BUS);
  // Pass our oneWire reference to Dallas Temperature sensor
  sensors = new DallasTemperature(onewire_instance);
  sensors->begin();
}

void SensorDriver::loop()
{
  get_light_sensor_reading();
}

/*
   Smoothing the analogue sensor reading
*/
float SensorDriver::get_light_sensor_reading()
{
  static int light_sensor_readings[NUM_OF_READINGS];
  static int idx = 0;
  static long total = 0;
  static elapsedMillis reading_interval;

  if (reading_interval > ANALOG_READ_INTERVAL)
  {
    auto last_reading = analogRead(LIGHT_SENSOR_PIN);
    last_reading = map(last_reading, 0, MAX_LIGHT_READING_VAL, 0, PWMRANGE);
    total -= light_sensor_readings[idx];
    total += last_reading;
    light_sensor_readings[idx] = last_reading;
    idx = (idx + 1) % NUM_OF_READINGS;
    reading_interval = 0;
  }

  float average_reading = (float)total / NUM_OF_READINGS;

  average_reading += config.brightness_offset;

  return min((float)PWMRANGE, max(0.f, average_reading));
}

float SensorDriver::get_temperature_sensor_reading()
{
  static elapsedMillis reading_interval;
  static float last_temp_reading = -1;

  if (last_temp_reading < 0 || reading_interval > TEMP_READ_INTERVAL)
  {
    DEBUG_PRINT(F("Requesting temperatures..."));
    sensors->requestTemperatures();
    DEBUG_PRINTLN(F("DONE"));
    float temperature_reading = config.celsius ? sensors->getTempCByIndex(0) : sensors->getTempFByIndex(0);
    if (temperature_reading != DEVICE_DISCONNECTED_C)
    {
      last_temp_reading = temperature_reading;
    }
    DEBUG_PRINT(F("Temperature for the device 1 (index 0) is: "));
    DEBUG_PRINTLN(last_temp_reading);
    reading_interval = 0;
  }
  return last_temp_reading;
}
