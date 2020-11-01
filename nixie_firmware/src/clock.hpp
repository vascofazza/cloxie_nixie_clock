#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <AceTime.h>
#include "configuration.hpp"
#include "tube_driver.hpp"

using namespace ace_time;
using namespace ace_time::clock;

class ClockDriver
{
private:
    TubeDriver* tube_driver;
    BasicZoneProcessor zoneProcessor;
    NtpClock ntpClock;
    SystemClockLoop *systemClock;
    bool display_time;
    bool display_date;
    void print_current_time(ace_time::ZonedDateTime);
    void print_current_date(ace_time::ZonedDateTime);

public:

ClockDriver(TubeDriver*);

void loop();

void show_time(bool);

void show_date(bool);

};

#endif