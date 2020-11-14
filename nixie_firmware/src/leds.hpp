#ifndef LEDS_HPP
#define LEDS_HPP

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <elapsedMillis.h>
#include "configuration.hpp"
#include "tube_driver.hpp"
#include "sensors.hpp"

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*LedPatternList[])(CRGB *, int, int);

#define DATA_PIN D7
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

enum LED_MODE
{
  STATIC = 0,
  CYCLYNG = 1,
  RANDOM = 2,
};

class LedDriver
{
private:
    TubeDriver *tube_driver;
    SensorDriver *sensor_driver;
    CRGB *leds;
    void (**patterns)(CRGB*, int, int);
    int pattern;
    int brightness;
    int patterns_num;
    bool status;

public:
    LedDriver(TubeDriver *, SensorDriver *, int, void (**patterns)(CRGB*, int, int), int);

    void turn_off(bool);

    void turn_on(int);

    bool get_status();

    void loop();

    void set_brightness(int brightness);

    void set_patterns(LedPatternList, int);

    void nextPattern();
};

#endif