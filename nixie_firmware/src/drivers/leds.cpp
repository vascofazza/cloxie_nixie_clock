#include "leds.hpp"

LedDriver::LedDriver(TubeDriver *tube_driver, SensorDriver *sensor_driver, uint8_t num_leds, void (**default_pattern)(CRGB *, uint8_t, int), uint8_t *pattern_status, uint8_t patterns_num)
{
  this->tube_driver = tube_driver;
  this->sensor_driver = sensor_driver;
  leds = new CRGB[num_leds];
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(0);
  pattern = pattern_status;
  brightness = 0;
  status = true;
  patterns = default_pattern;
  this->patterns_num = patterns_num;
  turn_off(false);
  led_ticker.attach_ms(1000 / FRAMES_PER_SECOND, std::bind(&LedDriver::loop, this));
}

void LedDriver::set_patterns(LedPatternList patterns, uint8_t patterns_num, uint8_t *pattern)
{
  if (patterns == this->patterns)
    return;
  this->pattern = pattern;
  this->patterns = patterns;
  this->patterns_num = patterns_num;

  if (*(this->pattern) >= patterns_num)
    *(this->pattern) = 0;
}

void LedDriver::turn_off(bool fade)
{
  if (!status)
    return;
  status = false;
  if (fade)
  {
    for (int i = brightness; i >= 0; i--)
    {
      FastLED.setBrightness(i);
      //FastLED.show();
      delay(TURN_ON_OFF_TIME / (brightness + 1));
    }
  }
  else
  {
    FastLED.setBrightness(0);
    //FastLED.show();
  }
  brightness = 0;
}

void LedDriver::turn_on(bool fade)
{
  if (status)
    return;
  if (!config.leds)
  {
    turn_off(false);
    return;
  }
  if (!config.leds)
  {
    turn_off(false);
    return;
  }
  if (fade)
  {
    brightness = map(sensor_driver->get_light_sensor_reading(), 0, ANALOGRANGE, config.min_led_brightness, config.max_led_brightness);
    for (int i = 0; i <= brightness; i++)
    {
      FastLED.setBrightness(i);
      //FastLED.show();
      delay(TURN_ON_OFF_TIME / (brightness + 1));
      brightness = map(sensor_driver->get_light_sensor_reading(), 0, ANALOGRANGE, config.min_led_brightness, config.max_led_brightness);
    }
  }
  else
  {
    FastLED.setBrightness(this->brightness);
    //FastLED.show();
  }
  status = true;
}

bool LedDriver::get_status()
{
  return status;
}

void LedDriver::process_pattern(int gHue)
{
  auto current_pattern = (void (*)(CRGB *, int, int))patterns[*pattern];
  // Call the current pattern function once, updating the 'leds' array
  current_pattern(leds, NUM_LEDS, gHue);
  // send the 'leds' array out to the actual LED strip
  FastLED.show();
}

void LedDriver::loop()
{
  static int gHue = 0;
  static elapsedMillis hueDelay;
  static elapsedMillis patternDelay;

  //HACK - esp8266 does not have hardware PWM.
  bool prev_status = tube_driver->get_status();
  if (prev_status)
    tube_driver->turn_off(false);
  process_pattern(gHue);
  if (prev_status)
    tube_driver->turn_on(false);
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

  if (!status)
    return;
  set_brightness(sensor_driver->get_light_sensor_reading());
}

void LedDriver::nextPattern()
{
  *pattern = (*pattern + 1) % patterns_num;
}

void LedDriver::set_brightness(int16_t brightness)
{
  if (!status)
    return;

  brightness = map(brightness, 0, ANALOGRANGE, config.min_led_brightness, config.max_led_brightness);

#ifdef DEBUG
  static elapsedMillis deb_mils;
  if (deb_mils > 1000)
  {
    DEBUG_PRINT(F("LED brightness: "));
    DEBUG_PRINTLN(brightness);
    deb_mils = 0;
  }
#endif

  this->brightness = brightness;
  FastLED.setBrightness(brightness);
}
