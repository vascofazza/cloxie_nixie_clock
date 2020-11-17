#ifndef LEDS_HPP
#define LEDS_HPP

#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <elapsedMillis.h>
#include "configuration.hpp"
#include "tube_driver.hpp"
#include "sensors.hpp"
#include <Ticker.h>
#include "utils.hpp"

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*LedPatternList[])(CRGB *, uint8_t, int);

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
    void (**patterns)(CRGB*, uint8_t, int);
    uint8_t pattern;
    int16_t brightness;
    uint8_t patterns_num;
    bool status;
    Ticker led_ticker;
    void loop();

public:
    LedDriver(TubeDriver *, SensorDriver *, uint8_t, void (**patterns)(CRGB*, uint8_t, int), uint8_t);

    void process_pattern(int);

    void turn_off(bool);

    void turn_on(bool);

    bool get_status();

    void set_brightness(int16_t brightness);

    void set_patterns(LedPatternList, uint8_t);

    void nextPattern();
};

#endif