#include "leds.hpp"

LedDriver::LedDriver(TubeDriver *tube_driver, int num_leds, void (**default_pattern)(CRGB *, int, int), int patterns_num)
{
  this->tube_driver = tube_driver;
  leds = new CRGB[num_leds];
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
  pattern = 0;
  brightness = 0;
  status = true;
  patterns = default_pattern;
  this->patterns_num = patterns_num;
}

void LedDriver::set_patterns(LedPatternList patterns, int patterns_num)
{
  this->pattern = 0;
  this->patterns = patterns;
  this->patterns_num = patterns_num;
}

void LedDriver::turn_off(bool fade)
{
  if (!status)
    return;
  if (fade)
  {
    for (int i = brightness; i >= 0; i--)
    {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(50);
    }
  }
  else
  {
    FastLED.setBrightness(0);
    FastLED.show();
  }
  brightness = 0;
  status = false;
}

void LedDriver::turn_on(int brightness)
{
  status = true;
  if (brightness > 0)
  {
    for (int i = 0; i <= brightness; i++)
    {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(50);
    }
  }
  else
  {
    FastLED.setBrightness(this->brightness);
  }
}

void LedDriver::loop()
{
  if (!status)
    return;
  static int gHue = 0;
  static elapsedMillis hueDelay;
  static elapsedMillis patternDelay;
  static elapsedMillis renderDelay;

  if (renderDelay > (1000 / FRAMES_PER_SECOND))
  {
    //HACK - esp8266 does not have hardware PWM.
    tube_driver->turn_off(false);

    auto current_pattern = (void (*)(CRGB *, int, int))patterns[pattern];
    // Call the current pattern function once, updating the 'leds' array
    current_pattern(leds, NUM_LEDS, gHue);
    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    tube_driver->turn_on(-1);
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
  if (!status)
    return;
  brightness = map(brightness, 0, PWMRANGE, MIN_LED_BRIGHTNESS, MAX_LED_BRIGHNTESS);
  this->brightness = brightness;
  FastLED.setBrightness(brightness);
}
