#include "leds.hpp"

LedDriver::LedDriver(TubeDriver *tube_driver, int num_leds, void (**default_pattern)(CRGB *, int, int), int patterns_num)
{
  this->tube_driver = tube_driver;
  leds = new CRGB[num_leds];
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
  pattern = 0;
  brightness = 0;
  patterns = default_pattern;
  this->patterns_num = patterns_num;
}

void LedDriver::set_patterns(LedPatternList patterns, int patterns_num)
{
  this->pattern = 0;
  this->patterns = patterns;
  this->patterns_num = patterns_num;
}

void LedDriver::turn_off()
{
  brightness = 0;
  FastLED.setBrightness(0);
  FastLED.show();
}

void LedDriver::turn_on()
{
  FastLED.setBrightness(brightness);
}

void LedDriver::loop()
{
  static int gHue = 0;
  static elapsedMillis hueDelay;
  static elapsedMillis patternDelay;
  static elapsedMillis renderDelay;

  if (renderDelay > (1000 / FRAMES_PER_SECOND))
  {
    //HACK - esp8266 does not have hardware PWM.
    tube_driver->turn_off();

    auto current_pattern = (void (*)(CRGB *, int, int))patterns[pattern];
    // Call the current pattern function once, updating the 'leds' array
    current_pattern(leds, NUM_LEDS, gHue);
    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    tube_driver->turn_on();
    renderDelay = 0;
  }

  if (hueDelay > HUE_DELAY)
  {
    if ((config.led_configuration == LED_MODE::STATIC))
    {
      gHue = LED_STATIC_HUE_VALUE;
    }
    else
    {
      gHue++;
    }
    hueDelay = 0;
  }

  if (patternDelay > PATTERN_DELAY)
  {
    nextPattern();
    patternDelay = 0;
  }
}

void LedDriver::nextPattern()
{
  pattern = (pattern + 1) % patterns_num;
}

void LedDriver::set_brightness(int brightness)
{
  brightness = map(brightness, 0, PWMRANGE, MIN_LED_BRIGHTNESS, MAX_LED_BRIGHNTESS);
  this->brightness = brightness;
  FastLED.setBrightness(brightness);
}
