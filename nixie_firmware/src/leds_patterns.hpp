#ifndef LEDS_PATTERNS_HPP
#define LEDS_PATTERNS_HPP

#include <FastLED.h>

void rainbow(CRGB *leds, int num_leds, int gHue)
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, num_leds, gHue, 7);
}

void addGlitter(CRGB *leds, int num_leds, fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(num_leds)] += CRGB::White;
  }
}

void rainbowWithGlitter(CRGB *leds, int num_leds, int gHue)
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(leds, num_leds, gHue);
  addGlitter(leds, num_leds, 80);
}

void confetti(CRGB *leds, int num_leds, int gHue)
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, num_leds, 10);
  int pos = random16(num_leds);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon(CRGB *leds, int num_leds, int gHue)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, num_leds, 20);
  int pos = beatsin16(13, 0, num_leds - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm(CRGB *leds, int num_leds, int gHue)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < num_leds; i++)
  { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle(CRGB *leds, int num_leds, int gHue)
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, num_leds, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, num_leds - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

#endif