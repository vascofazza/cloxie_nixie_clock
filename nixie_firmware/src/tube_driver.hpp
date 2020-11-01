#ifndef TUBE_DRIVER_HPP
#define TUBE_DRIVER_HPP

#include <elapsedMillis.h>
#include <EveryTimer.h>
#include "configuration.hpp"

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
    int brightness;
    int l_dot_brightness;
    int r_dot_brightness;
    EveryTimer cathode_poisoning_cycle;

    void set_tube_brightness(int, int, int);
public:
    TubeDriver();

    void run_test();

    void loop();

    void set_tubes(int, int, int, int, int, int);

    void set_brightness(int);

    void set_dots_brightness(int, int);

    void display_time_and_date(int, int, int, bool);

    void display_temperature(float);

    void shutdown();

    void turn_off();

    void turn_on();

    void cathode_poisoning_prevention(unsigned long);
};

#endif