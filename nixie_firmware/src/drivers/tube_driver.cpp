#include "tube_driver.hpp"

TubeDriver::TubeDriver(SensorDriver *sensor_driver)
{
  this->sensor_driver = sensor_driver;
  status = false;
  int_status = false;
  brightness = MIN_TUBE_BRIGHTNESS;
  l_dot_brightness = 0;
  r_dot_brightness = 0;

  pinMode(STROBE, OUTPUT);
  pinMode(LEFT_DOT, OUTPUT);
  pinMode(RIGHT_DOT, OUTPUT);
  pinMode(SHF_LATCH, OUTPUT);
  pinMode(SHF_CLOCK, OUTPUT);
  pinMode(SHF_DATA, OUTPUT);
  pinMode(SHUTDOWN_PIN, INPUT);
  cathode_poisoning_cycle.Every(config.depoisoning, std::bind(&TubeDriver::cathode_poisoning_prevention, this, CATHODE_POISONING_PREVENTION_TIME));
  run_test();
  set_tubes(0, 0, 0, 0, 0, 0);
  turn_off(false);
  shutdown();
  tube_ticker.attach_ms(BRIGTHNESS_REFRESH, std::bind(&TubeDriver::adjust_brightness, this));
}

void TubeDriver::run_test()
{
  DEBUG_PRINT(F("Running tube tests..."));

  set_tube_brightness(PWMRANGE, PWMRANGE, PWMRANGE);
  for (int i = 0; i < 10; i++)
  {
    set_tubes(i, i, i, i, i, i);
    for (int b = 0; b < 1024; b += 100)
    {
      set_tube_brightness(b, b, b);
      activeDelay(25);
    }
    for (int b = 1024; b >= 0; b -= 100)
    {
      set_tube_brightness(b, b, b);
      activeDelay(25);
    }
  }
  set_tube_brightness(0, 0, 0);
  DEBUG_PRINTLN(F("Done!"));
}

/*
   -1 or digits > 9 turns off the nixie tube
*/
void TubeDriver::set_tubes(int8_t h, int8_t hh, int8_t m, int8_t mm, int8_t s, int8_t ss)
{
  static int8_t prev_h = -1;
  static int8_t prev_hh = -1;
  static int8_t prev_m = -1;
  static int8_t prev_mm = -1;
  static int8_t prev_s = -1;
  static int8_t prev_ss = -1;

  if (h == prev_h && hh == prev_hh && m == prev_m && mm == prev_mm && s == prev_s && ss == prev_ss)
    return;

  prev_h = h;
  prev_hh = hh;
  prev_m = m;
  prev_mm = mm;
  prev_s = s;
  prev_ss = ss;

  if (h >= 0)
  {
    if (h == 0)
      h = 1;
    else if (h == 1)
      h = 0;
    else
      h = (11 - h);
  }
  if (hh >= 0)
  {
    if (hh == 0)
      hh = 1;
    else if (hh == 1)
      hh = 0;
    else
      hh = (11 - hh);
  }
  if (m >= 0)
  {
    if (m == 0)
      m = 1;
    else if (m == 1)
      m = 0;
    else
      m = (11 - m);
  }
  if (mm >= 0)
  {
    if (mm == 0)
      mm = 1;
    else if (mm == 1)
      mm = 0;
    else
      mm = (11 - mm);
  }
  if (s >= 0)
  {
    if (s == 0)
      s = 1;
    else if (s == 1)
      s = 0;
    else
      s = (11 - s);
  }
  if (ss >= 0)
  {
    if (ss == 0)
      ss = 1;
    else if (ss == 1)
      ss = 0;
    else
      ss = (11 - ss);
  }

  //prepare the data payload
  unsigned int payload = (ss & 0xF) | ((s & 0xF) << 4) | ((mm & 0xF) << 8) | ((m & 0xF) << 12) | ((hh & 0xF) << 16) | ((h & 0xF) << 20);
  // the LEDs don't change while you're sending in bits:
  digitalWrite(SHF_LATCH, LOW);
  // shift out the bits:
  shiftOut(SHF_DATA, SHF_CLOCK, LSBFIRST, payload);
  shiftOut(SHF_DATA, SHF_CLOCK, LSBFIRST, payload >> 8);
  shiftOut(SHF_DATA, SHF_CLOCK, LSBFIRST, payload >> 16);
  //take the latch pin high so the LEDs will light up:
  digitalWrite(SHF_LATCH, HIGH);
}

void TubeDriver::adjust_brightness()
{
  if (!int_status)
    return;
  set_brightness(sensor_driver->get_light_sensor_reading());
}

void TubeDriver::loop()
{
  if (config.depoisoning > 0)
    cathode_poisoning_cycle.Update();
  else
  {
    cathode_poisoning_cycle.Stop();
  }
}

void TubeDriver::display_time_and_date(int8_t h, int8_t m, int8_t s, bool show_zeros)
{
  int8_t hour1 = h / 10; //handle 10/24h and tube off on 0
  int8_t hour2 = h % 10;
  if (hour1 == 0 && hour2 != 0 && !show_zeros)
    hour1 = -1;

  int8_t min1 = m / 10;
  int8_t min2 = m % 10;
  int8_t sec1 = s / 10;
  int8_t sec2 = s % 10;

  set_tubes(hour1, hour2, min1, min2, sec1, sec2);
}

void TubeDriver::display_temperature(float temp)
{
  int integer = (int)temp;
  int decimal = (temp - integer) * 100;
  int i1 = integer / 10;
  int i2 = integer % 10;
  int d1 = decimal / 10;
  int d2 = decimal % 10;

  set_tubes(-1, -1, i1, i2, d1, d2);
  set_dots_brightness(0, PWMRANGE);
}

void TubeDriver::set_tube_brightness(int16_t brightness, int16_t l_dot_brightness, int16_t r_dot_brightness)
{
  analogWrite(STROBE, PWMRANGE - brightness);
  analogWrite(LEFT_DOT, l_dot_brightness);
  analogWrite(RIGHT_DOT, r_dot_brightness);
}

int scale(int val)
{
  float log_scale = 2 - 2 / (1 + (val / (float)PWMRANGE));
  val = log_scale * PWMRANGE;
  return val;
}

void TubeDriver::set_brightness(int16_t brightness)
{
  if (!status)
    return;
  this->brightness = brightness < MIN_TUBE_BRIGHTNESS ? this->brightness : scale(brightness);
  int dot_brightness = map(this->brightness, 0, PWMRANGE, 0, MAX_DOT_BRIGHNTESS);
  int left = map(l_dot_brightness, 0, PWMRANGE, 0, dot_brightness);
  int right = map(r_dot_brightness, 0, PWMRANGE, 0, dot_brightness);
  brightness = map(this->brightness, MIN_TUBE_BRIGHTNESS, PWMRANGE, MIN_TUBE_BRIGHTNESS, MAX_TUBE_BRIGHNTESS);
  set_tube_brightness(brightness, left, right);
}

void TubeDriver::set_dots_brightness(int16_t left, int16_t right)
{
  l_dot_brightness = left;
  r_dot_brightness = right;
}

void TubeDriver::shutdown()
{
  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH);
}

void TubeDriver::turn_off(bool fade)
{
  if (!status)
    return;
  if (fade)
  {
    int_status = false;
    for (int i = brightness; i >= 0; i--)
    {
      set_tube_brightness(i, i, i);
      delay(TURN_ON_OFF_TIME / (brightness + 1));
    }
    int_status = true;
  }
  digitalWrite(STROBE, HIGH);
  digitalWrite(LEFT_DOT, LOW);
  digitalWrite(RIGHT_DOT, LOW);
  status = false;
}

void TubeDriver::turn_on(bool fade)
{
  if (status)
    return;
  status = true;
  pinMode(SHUTDOWN_PIN, INPUT);
  if (fade)
  {
    int_status = false;
    for (int i = 0; i <= scale(max(sensor_driver->get_light_sensor_reading(), (float)MIN_TUBE_BRIGHTNESS)); i++)
    {
      set_tube_brightness(i, i, i);
      delay(TURN_ON_OFF_TIME / sensor_driver->get_light_sensor_reading());
    }
    int_status = true;
  }
  else
  {
    set_brightness(-1);
  }
}

bool TubeDriver::get_status()
{
  return status;
}

void TubeDriver::cathode_poisoning_prevention(unsigned long time)
{
  // if depoisoning is in progress, lets max out the brightness
  sensor_driver->set_max_brightness(true);
  unsigned long tot = 0;
  int iterations = 0;
  while (tot < time)
  {
    tot += map(tot, 0, time, 10, 1000);
    iterations++;
  }

  elapsedMillis timeout = 0;

  int i = 10 - iterations % 10;
  while (timeout < time)
  {
    unsigned long delay_val = map(timeout, 0, time, 10, 1000);
    i = i % 10;
    set_tubes(i, i, i, i, i, i);
    set_dots_brightness((i % 2) * PWMRANGE, ((i + 1) % 2) * PWMRANGE);
    activeDelay(delay_val);
    i++;
  }
  sensor_driver->set_max_brightness(false);
}
