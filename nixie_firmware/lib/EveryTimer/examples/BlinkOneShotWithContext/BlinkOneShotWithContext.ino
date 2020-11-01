/*****************************************************************************
Arduino library providing the possibility to call a function at specific time
intervals

MIT License

Copyright (c) 2018 Alessio Leoncini

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*****************************************************************************/

#include <OneShotTimer.h>

#define LED_PIN 8

OneShotTimer timer;
bool global_active = true;

//////////////////////////////////////////////////////////////////////////////
// Setup routine
void setup()
{
  // Connect an LED on pin 8
  pinMode(LED_PIN, OUTPUT);
  
  // Led off
  digitalWrite(LED_PIN, LOW);
  
  // Call the callback one single time after 10000 milliseconds
  timer.OneShot(10000, action,&global_active);
} 

//////////////////////////////////////////////////////////////////////////////
// Main loop routine
void loop()
{
  // Update method needed 
  timer.Update();
}

//////////////////////////////////////////////////////////////////////////////
// Callback called by timer
void action(void *ctx)
{
  bool *active=(bool*)ctx;
  // Led on
  digitalWrite(LED_PIN, *active);
  // Wait a second
  delay(1000);
  // Led off
  digitalWrite(LED_PIN, !*active);
}