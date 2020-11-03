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
    TubeDriver *tube_driver;
    BasicZoneProcessor zoneProcessor;
    NtpClock ntpClock;
    SystemClockLoop *systemClock;
    int blinking;
    bool display_time;
    bool display_date;
    bool display_timer;
    bool timer_running;
    long timer_duration;
    elapsedMillis current_timer_value;
    void print_current_time(ace_time::ZonedDateTime);
    void print_current_date(ace_time::ZonedDateTime);
    void print_timer();
    void blink_dots(void (*)(TubeDriver *, bool, elapsedMillis *));

public:
    ClockDriver(TubeDriver *);

    void loop();

    void show_time(bool);

    void show_date(bool);

    void show_timer(bool);

    void start_timer(long duration);

    void stop_timer();

    void reset_timer();

    bool is_timer_set();

    bool is_timer_running();

    void set_alarm(long offset);
};

typedef void (*BlinkPatternList[])(TubeDriver *, bool, elapsedMillis *);

#endif