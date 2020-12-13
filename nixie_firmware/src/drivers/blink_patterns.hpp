#ifndef TUBE_PATTERNS_HPP
#define TUBE_PATTERNS_HPP

#include <elapsedMillis.h>
#include "tube_driver.hpp"

void no_blink(TubeDriver *driver, bool sync_pulse, elapsedMillis *elapsed)
{
    driver->set_dots_brightness(0, 0);
}

void static_on(TubeDriver *driver, bool sync_pulse, elapsedMillis *elapsed)
{
    driver->set_dots_brightness(PWMRANGE, PWMRANGE);
}

void static_blink(TubeDriver *driver, bool sync_pulse, elapsedMillis *elapsed)
{
    static bool state = false;
    if (sync_pulse)
    {
        if (!state)
        {
            driver->set_dots_brightness(0, 0);
        }
        else
        {
            driver->set_dots_brightness(PWMRANGE, PWMRANGE);
        }
        state = !state;
    }
}

void double_static_blink(TubeDriver *driver, bool sync_pulse, elapsedMillis *elapsed)
{
    if (sync_pulse)
    {
        *elapsed = 0;
    }
    int val = PWMRANGE;
    if ((*elapsed > 100 && *elapsed < 200) || *elapsed > 300)
        val = 0;

    driver->set_dots_brightness(val, val);
}

void fade_out_blink(TubeDriver *driver, bool sync_pulse, elapsedMillis *elapsed)
{
    static bool state = false;
    static int val = 0;
    if (sync_pulse)
    {
        val = PWMRANGE;
        state = !state;
        *elapsed = 0;
    }
    val = PWMRANGE - map(*elapsed, 0, 800, 0, PWMRANGE);

    driver->set_dots_brightness(val, val);
}

#endif