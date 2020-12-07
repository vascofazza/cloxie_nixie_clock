#ifndef SENSORS_HPP
#define SENSORS_HPP

#define REQUIRESALARMS false
#define REQUIRESNEW false

#include <OneWire.h>
#include <DallasTemperature.h>
#include <elapsedMillis.h>
#include "configuration.hpp"
#include <Ticker.h>
#include "utils.hpp"

// GPIO where the DS18B20 is connected to
#define ONE_WIRE_BUS D2
#define LIGHT_SENSOR_PIN A0

class SensorDriver
{
private:
    OneWire* onewire_instance;
    DallasTemperature* sensors;
    uint16_t light_sensor_readings[NUM_OF_READINGS];
    long light_sensor_total = 0;
    bool max_brightness = false;
    Ticker sensors_ticker;

    void feed_readings();
    void reset_readings();
public:
    SensorDriver();

    void loop();

    void set_max_brightness(bool);

    float get_light_sensor_reading();

    float get_temperature_sensor_reading();

};

#endif