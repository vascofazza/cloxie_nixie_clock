#include "leds.hpp"
#include "leds_patterns.hpp"

LedPatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm};

LedDriver::LedDriver(TubeDriver *tube_driver, int num_leds)
{
  this->tube_driver = tube_driver;
  leds = new CRGB[num_leds];
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
  pattern = 0;
  brightness = 0;
}

void LedDriver::turn_off()
{
  FastLED.setBrightness(0);
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

    // Call the current pattern function once, updating the 'leds' array
    gPatterns[pattern](leds, NUM_LEDS, gHue);
    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    tube_driver->turn_on();
    renderDelay = 0;
  }

  if (hueDelay > HUE_DELAY)
  {
    gHue++;
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
  pattern = (pattern + 1) % ARRAY_SIZE(gPatterns);
}

void LedDriver::set_brightness(int brightness)
{
  brightness = map(brightness, 0, PWMRANGE, MIN_LED_BRIGHTNESS, MAX_LED_BRIGHNTESS);
  this->brightness = brightness;
  LEDS.setBrightness(brightness);
}
