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

#include <EveryTimer.h>

#define LED_PIN 8
#define PERIOD_MS 1000

EveryTimer timer;
bool active = true;

//////////////////////////////////////////////////////////////////////////////
// Setup routine
void setup()
{
  // Connect an LED on pin 8
  pinMode(LED_PIN, OUTPUT);
  
  // Call the callback every 1000 milliseconds
  timer.Every(PERIOD_MS, action);
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
void action()
{
  digitalWrite(LED_PIN, active ? HIGH : LOW);
  active = !active; 
}