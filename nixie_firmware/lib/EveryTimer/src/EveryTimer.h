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

#ifndef Every_Timer_Arduino_Library
#define Every_Timer_Arduino_Library

#include <Arduino.h>

class EveryTimer
{
public:
  // Default constructor
  EveryTimer();

  // Update function to call in the loop() routine
  void Update();

  // Start executing the callback every specified amount of milliseconds.
  // Return false if error
  bool Every(unsigned long milliseconds, void (*callback)());

  // Start executing the callback every specified amount of milliseconds with ctx.
  // Return false if error
  bool Every(unsigned long milliseconds, void (*callback)(void*), void* ctx);

  // Stop calling the provided callback
  void Stop();

  // Start calling the provided callback, if a stop command was issued
  void Start();
  
protected:
  inline void invokeCallback(){
    // choose between callback with or without context
    if (m_hasContext){
        (*ctxCallback)(m_ctx);
    }else{
        (*Callback)();
    }
  }
  // Callback reference
  void (*Callback)();

  // Context Callback reference
  void (*ctxCallback)(void*);

  // Context for callback
  void* m_ctx;
  
  // Flag to control callback execution
  bool m_running;
  
  // Flag to choose which callback to use
  bool m_hasContext;

  // Callback running period
  unsigned long m_milliseconds;
  
  // Timestamp of last callback run 
  unsigned long m_lastRunTimestamp;
};

#endif // Every_Timer_Arduino_Library