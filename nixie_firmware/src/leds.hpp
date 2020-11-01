#ifndef LEDS_HPP
#define LEDS_HPP

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>       //fastled
#include <elapsedMillis.h> //https://github.com/pfeerick/elapsedMillis
#include "configuration.hpp"
#include "tube_driver.hpp"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*LedPatternList[])(CRGB *, int, int);

#define DATA_PIN D7
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

class LedDriver
{
private:
    TubeDriver *tube_driver;
    CRGB *leds;
    int pattern;
    int brightness;
public:
    LedDriver(TubeDriver *, int);

    void turn_off();

    void turn_on();

    void loop();

    void set_brightness(int brightness);

    void nextPattern();
};

#endif