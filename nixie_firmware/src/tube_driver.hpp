#ifndef TUBE_DRIVER_HPP
#define TUBE_DRIVER_HPP

#include <elapsedMillis.h>
#include <EveryTimer.h>
#include "configuration.hpp"
#include "sensors.hpp"

#define LEFT_DOT D0
#define RIGHT_DOT D8
#define STROBE D1
#define SHF_LATCH D4
#define SHF_CLOCK D5
#define SHF_DATA D3
#define SHUTDOWN_PIN D6

class TubeDriver
{
private:
    int16_t brightness;
    int16_t l_dot_brightness;
    int16_t r_dot_brightness;
    bool status;
    EveryTimer cathode_poisoning_cycle;
    SensorDriver *sensor_driver;

    void set_tube_brightness(int16_t, int16_t, int16_t);

public:
    TubeDriver(SensorDriver *);

    void run_test();

    void loop();

    void set_tubes(int8_t, int8_t, int8_t, int8_t, int8_t, int8_t);

    void set_brightness(int16_t);

    void set_dots_brightness(int16_t, int16_t);

    void display_time_and_date(int8_t, int8_t, int8_t, bool);

    void display_temperature(float);

    void shutdown();

    void turn_off(bool);

    void turn_on(int16_t);

    void cathode_poisoning_prevention(unsigned long);
};

#endif