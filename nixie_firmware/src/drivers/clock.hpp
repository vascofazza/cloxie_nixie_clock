#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <AceTime.h>
#include "../configuration.hpp"
#include "tube_driver.hpp"
#include <Ticker.h>

using namespace ace_time;
using namespace ace_time::clock;

class ClockDriver
{
private:
    TubeDriver *tube_driver;
    BasicZoneProcessor zoneProcessor;
    NtpClock ntpClock;
    SystemClockLoop *systemClock;
    bool blinking;
    bool display_time;
    bool display_date;
    bool display_timer;
    bool display_stopwatch;
    bool timer_running;
    bool stopwatch_running;
    long timer_duration;
    long stopwatch_value;
    long current_stopwatch_value;
    elapsedMillis current_timer_value;
    Ticker clock_ticker;

    void print_current_time(ace_time::ZonedDateTime);
    void print_current_date(ace_time::ZonedDateTime);
    void print_timer_stopwatch();
    void blink_dots(void (*)(TubeDriver *, bool, elapsedMillis *));

public:
    ClockDriver(TubeDriver *);

    void loop();

    ace_time::ZonedDateTime get_current_time();

    void show_time(bool);

    void show_date(bool);

    void show_timer(bool);

    void show_stopwatch(bool);

    void start_timer(long duration);

    void stop_timer();

    void reset_timer();

    void start_stopwatch();

    void stop_stopwatch();

    void reset_stopwatch();

    bool is_timer_set();

    bool is_stopwatch_set();

    bool is_timer_running();

    bool is_stopwatch_running();

    bool is_night_hours();

    void set_alarm(long epoch);
};

typedef void (*BlinkPatternList[])(TubeDriver *, bool, elapsedMillis *);

#endif