#include <elapsedMillis.h> //https://github.com/pfeerick/elapsedMillis

#define LEFT_DOT D0
#define RIGHT_DOT D8
#define STROBE D1
#define LATCH D4
#define CLOCK D5
#define DATA D3

void set_tubes(int, int, int, int, int, int);
void set_tube_brightness(int);
void blink_dots();

static int tube_brightness = 1023;
static boolean blinking = false;

void run_test()
{
  elapsedMillis test_time;
  while (test_time < TEST_TIME)
  {
    for (int i = 0; i < 10; i++)
    {
      set_tubes(i, i, i, i, i, i);
      blink_dots();
      delay(250);
    }
    delay(1000);
    set_tubes(-1, -1, -1, -1, -1, -1);
    delay(1000);

    for (int b = 0; b < 1024; b += 100)
    {
      set_tube_brightness(b);
      for (int i = 0; i < 10; i++)
      {
        set_tubes(i, i, i, i, i, i);
        blink_dots();
        delay(50);
      }
    }
    delay(1000);
    set_tubes(-1, -1, -1, -1, -1, -1);
    delay(1000);


    for (int i = 0; i < 10; i++)
    {
      set_tubes(i, i, i, i, i, i);
      for (int b = 0; b < 1024; b += 50)
      {
        set_tube_brightness(b);
        blink_dots();
        delay(25);
      }
      for (int b = 1024; b > 0; b -= 50)
      {
        set_tube_brightness(b);
        blink_dots();
        delay(25);
      }
    }
    delay(1000);
    set_tubes(-1, -1, -1, -1, -1, -1);
    delay(1000);
    set_tube_brightness(1024);
  }
  set_tube_brightness(1024);
}

void setup_tube()
{
  pinMode(STROBE, OUTPUT);
  pinMode(LEFT_DOT, OUTPUT);
  pinMode(RIGHT_DOT, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  run_test();
}

/*
   -1 or digits > 9 turns off the nixie tube
*/
void set_tubes(int h, int hh, int m, int mm, int s, int ss)
{
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
  digitalWrite(LATCH, LOW);
  // shift out the bits:
  shiftOut(DATA, CLOCK, LSBFIRST, payload);
  shiftOut(DATA, CLOCK, LSBFIRST, payload >> 8);
  shiftOut(DATA, CLOCK, LSBFIRST, payload >> 16);
  //take the latch pin high so the LEDs will light up:
  digitalWrite(LATCH, HIGH);
}

void display_time_and_date(int h, int m, int s)
{
  int hour1 = h / 10; //handle 10/24h and tube off on 0
  int hour2 = h % 10;
  int min1 = m / 10;
  int min2 = m % 10;
  int sec1 = s / 10;
  int sec2 = s % 10;

  set_tubes(hour1, hour2, min1, min2, sec1, sec2);
  //handle dots TODO
}

void display_temperature(float temp)
{
  int integer = (int)temp;
  int decimal = (temp - integer) * 100;
  int i1 = integer / 10;
  int i2 = integer % 10;
  int d1 = decimal / 10;
  int d2 = decimal % 10;

  set_tubes(-1, -1, i1, i2, d1, d2);
  //handle dots TODO
}

void blink_dots()
{
  if (blinking)
  {
    digitalWrite(LEFT_DOT, LOW);
    digitalWrite(RIGHT_DOT, LOW);
  }
  else
  {
    analogWrite(LEFT_DOT, tube_brightness);
    analogWrite(RIGHT_DOT, tube_brightness);
  }
  blinking = !blinking;
}

void set_tube_brightness(int brightness)
{
  if (brightness <= 0)
  {
    digitalWrite(STROBE, HIGH);
    digitalWrite(LEFT_DOT, LOW);
    digitalWrite(RIGHT_DOT, LOW);
  }
  else
  {
    tube_brightness = brightness;
    analogWrite(STROBE, 1023 - tube_brightness);
    if (blinking)
    {
      analogWrite(LEFT_DOT, tube_brightness);
      analogWrite(RIGHT_DOT, tube_brightness);
    }
  }
}
