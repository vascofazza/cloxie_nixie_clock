#define LEFT_DOT D0
#define RIGHT_DOT D8
#define STROBE D1
#define LATCH D4
#define CLOCK D5
#define DATA D3

void setup_tube()
{
  pinMode(STROBE, OUTPUT);
  pinMode(LEFT_DOT, OUTPUT);
  pinMode(RIGHT_DOT, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
}

/*
 * -1 or digits > 9 turns off the nixie tube
 */
void set_tubes(byte h, byte hh, byte m, byte mm, byte s, byte ss) 
{
  //prepare the data payload
  unsigned int payload = ss + (s << 4) + (mm << 8) + (m << 12) + (hh << 16) + (h << 20);
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

void set_brightness(unsigned int brightness)
{
  //TODO
  analogWrite(STROBE, brightness);
  analogWrite(LEFT_DOT, brightness);
  analogWrite(RIGHT_DOT, brightness);
}
