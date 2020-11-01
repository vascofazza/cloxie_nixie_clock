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

#ifndef OneShot_Timer_Arduino_Library
#define OneShot_Timer_Arduino_Library

#include <EveryTimer.h>

class OneShotTimer : protected EveryTimer
{
public:
  // Default constructor
  OneShotTimer();

  // Update function to call in the loop() routine
  void Update();

  // Start immediately to wait for specified amount of milliseconds, 
  // call the callback at timeout. Return false if error
  bool OneShot(unsigned long milliseconds, void (*callback)());
  
  bool OneShot(unsigned long milliseconds, void (*callback)(void*), void* ctx);

  // Stop calling the provided callback
  void Stop();
};

#endif // OneShot_Timer_Arduino_Library