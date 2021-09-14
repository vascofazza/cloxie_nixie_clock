#include "sensors.hpp"

SensorDriver::SensorDriver()
{
  reset_readings();
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  max_brightness = false;
  // Setup a oneWire instance to communicate with any OneWire devices
  onewire_instance = new OneWire(ONE_WIRE_BUS);
  // Pass our oneWire reference to Dallas Temperature sensor
  sensors = new DallasTemperature(onewire_instance);
  sensors->setResolution(9);
  sensors->begin();
  get_temperature_sensor_reading(false);
  sensors_ticker.attach_ms(ANALOG_READ_INTERVAL, std::bind(&SensorDriver::loop, this));
}

void SensorDriver::loop()
{
  static uint16_t idx = 0;
  int last_reading = max_brightness ? ANALOGRANGE : analogRead(LIGHT_SENSOR_PIN);
  last_reading = map(last_reading, 0, MAX_LIGHT_READING_VAL, 0, ANALOGRANGE);
  light_sensor_total -= light_sensor_readings[idx];
  light_sensor_total += last_reading;
  light_sensor_readings[idx] = last_reading;
  idx = (idx + 1) % NUM_OF_READINGS;

#ifdef DEBUG
  static elapsedMillis deb_mils;
  if (deb_mils > 1000)
  {
    DEBUG_PRINT(F("Light sensor averaged reading: "));
    DEBUG_PRINTLN(light_sensor_total / NUM_OF_READINGS + config.brightness_offset);
    deb_mils = 0;
  }
#endif
}

#define LUX_CALC_SCALAR 12518931
#define LUX_CALC_EXPONENT -1.405
#define REF_RESISTANCE 10000

float SensorDriver::get_lux_reading()
{ 
  int ldrRawData;
	float resistorVoltage, ldrVoltage;
	float ldrResistance;
	float ldrLux;
	// Perform the analog to digital conversion
	ldrRawData = analogRead(LIGHT_SENSOR_PIN);
	// RESISTOR VOLTAGE_CONVERSION
	// Convert the raw digital data back to the voltage that was measured on the analog pin
	resistorVoltage = (float)ldrRawData / ANALOGRANGE * 3.3;
	// voltage across the LDR is the 5V supply minus the 5k resistor voltage
	ldrVoltage = 3.3 - resistorVoltage;
	// LDR_RESISTANCE_CONVERSION
	// resistance that the LDR would have for that voltage
	ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
	// LDR_LUX
	// Change the code below to the proper conversion from ldrResistance to
	// ldrLux
	ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
	// print out the results
	return ldrLux;
}

/*
   Smoothing the analogue sensor reading
*/
float SensorDriver::get_light_sensor_reading()
{
  if (!config.adaptive_brightness)
  {
    if (max_brightness)
      return ANALOGRANGE;
    return DEFAULT_BRIGHTNESS;
  }

  float average_reading = (float)light_sensor_total / NUM_OF_READINGS;
  average_reading += config.brightness_offset;
  return min((float)ANALOGRANGE, max(0.f, average_reading));
}

void SensorDriver::set_max_brightness(bool val)
{
  this->max_brightness = val;
}

void SensorDriver::feed_readings()
{
  DEBUG_PRINT(F("Feeding sensor..."));
  for (int i = 0; i < NUM_OF_READINGS; i++)
  {
    loop();
    activeDelay(ANALOG_READ_INTERVAL);
  }
  DEBUG_PRINTLN(F("Done."));
}

void SensorDriver::reset_readings()
{
  light_sensor_total = 0;
  for (int i = 0; i < NUM_OF_READINGS; i++)
  {
    light_sensor_readings[i] = 0;
  }
}

float SensorDriver::get_temperature_sensor_reading(bool get_last_reading)
{
  static elapsedMillis reading_interval;
  static float last_temp_reading = -1;

  if (!get_last_reading && (last_temp_reading < 0 || reading_interval > TEMP_READ_INTERVAL))
  {
    DEBUG_PRINT(F("Requesting temperatures..."));
    sensors->requestTemperatures();
    activeDelay(10);
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
