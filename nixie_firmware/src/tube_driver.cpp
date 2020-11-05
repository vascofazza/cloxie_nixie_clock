#include "tube_driver.hpp"

TubeDriver::TubeDriver()
{
  brightness = 0;
  l_dot_brightness = 0;
  r_dot_brightness = 0;

  pinMode(STROBE, OUTPUT);
  pinMode(LEFT_DOT, OUTPUT);
  pinMode(RIGHT_DOT, OUTPUT);
  pinMode(SHF_LATCH, OUTPUT);
  pinMode(SHF_CLOCK, OUTPUT);
  pinMode(SHF_DATA, OUTPUT);
  pinMode(SHUTDOWN_PIN, OUTPUT);

  turn_on();
}

void TubeDriver::run_test()
{
  Serial.print(F("Running tube tests..."));
  elapsedSeconds test_time;
  while (test_time < TEST_TIME)
  {
    set_tube_brightness(PWMRANGE, PWMRANGE, PWMRANGE);
    for (int i = 0; i < 10; i++)
    {
      set_tubes(i, i, i, i, i, i);
      delay(250);
    }
    delay(1000);
    turn_off();
    delay(1000);

    for (int b = 0; b < 1024; b += 200)
    {
      set_tube_brightness(b, b, b);
      for (int i = 0; i < 10; i++)
      {
        set_tubes(i, i, i, i, i, i);
        delay(50);
      }
    }
    delay(1000);
    turn_off();
    delay(1000);

    for (int i = 0; i < 10; i++)
    {
      set_tubes(i, i, i, i, i, i);
      for (int b = 0; b < 1024; b += 100)
      {
        set_tube_brightness(b, b, b);
        delay(25);
      }
      for (int b = 1024; b >= 0; b -= 100)
      {
        set_tube_brightness(b, b, b);
        delay(25);
      }
    }
    delay(1000);
    turn_off();
    delay(1000);
  }
  turn_off();
  Serial.println(F("Done!"));
}

/*
   -1 or digits > 9 turns off the nixie tube
*/
void TubeDriver::set_tubes(int h, int hh, int m, int mm, int s, int ss)
{
  static int prev_h = -1;
  static int prev_hh = -1;
  static int prev_m = -1;
  static int prev_mm = -1;
  static int prev_s = -1;
  static int prev_ss = -1;

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

void TubeDriver::loop()
{
  static elapsedSeconds running_time; 

  if (running_time > CATHODE_POISONING_TRIGGER_TIME)
  {
    cathode_poisoning_prevention(CATHODE_POISONING_PREVENTION_TIME);
    running_time = 0;
  }
}

void TubeDriver::display_time_and_date(int h, int m, int s, bool show_zeros)
{
  int hour1 = h / 10; //handle 10/24h and tube off on 0
  int hour2 = h % 10;
  if (hour1 == 0 && hour2 != 0 && !show_zeros)
    hour1 = -1;

  int min1 = m / 10;
  int min2 = m % 10;
  int sec1 = s / 10;
  int sec2 = s % 10;

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

void TubeDriver::set_tube_brightness(int brightness, int l_dot_brightness, int r_dot_brightness)
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

void TubeDriver::set_brightness(int brightness)
{
  this->brightness = brightness < 0 ? this->brightness : brightness;
  brightness = scale(this->brightness);
  brightness = map(brightness, 0, PWMRANGE, MIN_TUBE_BRIGHTNESS, MAX_TUBE_BRIGHNTESS);
  int dot_brightness = map(brightness, 0, PWMRANGE, MIN_DOT_BRIGHTNESS, MAX_DOT_BRIGHNTESS);
  int left = map(dot_brightness, 0, PWMRANGE, 0, l_dot_brightness);
  int right = map(dot_brightness, 0, PWMRANGE, 0, r_dot_brightness);
  set_tube_brightness(brightness, left, right);
}

void TubeDriver::set_dots_brightness(int left, int right)
{
  l_dot_brightness = left;
  r_dot_brightness = right;
  set_brightness(-1);
}

void TubeDriver::shutdown()
{
  digitalWrite(SHUTDOWN_PIN, HIGH);
}

void TubeDriver::turn_off()
{
  digitalWrite(STROBE, HIGH);
  digitalWrite(LEFT_DOT, LOW);
  digitalWrite(RIGHT_DOT, LOW);
}

void TubeDriver::turn_on()
{
  digitalWrite(SHUTDOWN_PIN, LOW);
  set_brightness(-1);
}

void TubeDriver::cathode_poisoning_prevention(unsigned long time)
{
  unsigned long tot = 0;
  int iterations = 0;
  while (tot < time)
  {
    tot += map(tot, 0, time, 10, 1000);
    iterations++;
  }

  elapsedMillis timeout = 0;

  //set_tube_brightness(PWMRANGE, PWMRANGE, PWMRANGE);
  int i = 10 - iterations % 10;
  while (timeout < time)
  {
    auto delay_val = map(timeout, 0, time, 10, 1000);
    i = i % 10;
    set_tubes(i, i, i, i, i, i);
    set_dots_brightness((i % 2) * PWMRANGE, ((i + 1) % 2) * PWMRANGE);
    delay(delay_val);
    i++;
  }

  set_brightness(-1);
}
