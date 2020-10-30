#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 1
#include <FastLED.h> //fastled
#include <elapsedMillis.h> //https://github.com/pfeerick/elapsedMillis

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup_leds()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.setMaxRefreshRate(FRAMES_PER_SECOND);
  FastLED.setBrightness(80); //TODO parameter
}

void set_led_brightness(int brightness)
{
  LEDS.setBrightness(brightness);
}


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

#include "leds_patterns.h"

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };


void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void leds_loop() {
  static elapsedMillis hueDelay;
  static elapsedMillis patternDelay;
  static elapsedMillis renderDelay;

  if (renderDelay > (1000 / FRAMES_PER_SECOND)) {
    //HACK - esp8266 does not have hardware PWM.
    set_tube_brightness(-1);

    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();
    // send the 'leds' array out to the actual LED strip
    FastLED.show();
    set_tube_brightness(tube_brightness);
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
