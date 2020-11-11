#ifndef SENSORS_HPP
#define SENSORS_HPP

#include <OneWire.h>
#include <DallasTemperature.h>
#include <elapsedMillis.h>
#include "configuration.hpp"
#include "tube_driver.hpp"

// GPIO where the DS18B20 is connected to
#define ONE_WIRE_BUS D2
#define LIGHT_SENSOR_PIN A0

class SensorDriver
{
private:
    TubeDriver* tube_driver;
    OneWire* onewire_instance;
    DallasTemperature* sensors;
    int light_sensor_readings[NUM_OF_READINGS];
    long light_sensor_total = 0;
public:
    SensorDriver(TubeDriver*);

    void loop();

    float get_light_sensor_reading();

    float get_temperature_sensor_reading();

    void reset_readings();
};

#endif