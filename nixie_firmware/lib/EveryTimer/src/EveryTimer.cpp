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

#include "EveryTimer.h"

//////////////////////////////////////////////////////////////////////////////
// Default constructor
EveryTimer::EveryTimer()
{
  // At the first time, no callback is set so running is unabled.
  m_running = false;
  m_hasContext =false;
  Callback = nullptr;
  ctxCallback = nullptr;
  
  // Starting dummy values
  m_milliseconds = 0;
  m_lastRunTimestamp = 0;
  m_ctx = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Update function to call in the loop() routine
void EveryTimer::Update()
{
  // If running flag is off, exit
  if(!m_running)
  {
    return;
  }
  
  // Read current time
  unsigned long now = millis();
  
  // Call immediately the callback if start command was issued
  // or if elapsed time was greater or equal the specified calling period
  if((m_lastRunTimestamp == 0) ||
  ((now - m_lastRunTimestamp) >= m_milliseconds))
  {
    invokeCallback();
    m_lastRunTimestamp = now;
    return;
  }
  
  // Handle possible overflow of the value given by millis() function
  if(now < m_lastRunTimestamp)
  {
    unsigned long max = 0;
    max--;
    // This works only if:
    // 1) m_milliseconds is at least an order of magnitude smaller than max
    // 2) current Update() method is called with a period smaller than m_milliseconds
    if((max - m_lastRunTimestamp - now) >= m_milliseconds)
    {
      invokeCallback();
      m_lastRunTimestamp = now;
      return;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
// Start executing the callback every specified amount of milliseconds.
// Return false if error
bool EveryTimer::Every(unsigned long milliseconds, void (*callback)())
{
  // Check that callback pointer is valid
  if(callback == nullptr)
  {
    return false;
  }

  // Set milliseconds
  m_milliseconds = milliseconds;

  // Save callback pointer
  Callback = callback;
  
  // remember we choose to use callback without context
  m_hasContext = false;

  // Enable running state
  m_running = true;

  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Start executing the callback every specified amount of milliseconds with context.
// Return false if error
bool EveryTimer::Every(unsigned long milliseconds, void (*callback)(void*), void* ctx)
{
  // Check that callback pointer is valid
  if(callback == nullptr)
  {
    return false;
  }

  // store context
  m_ctx=ctx;

  // Set milliseconds
  m_milliseconds = milliseconds;

  // Save callback pointer
  ctxCallback = callback;

  // remember we choose to use callback with context
  m_hasContext = true;

  // Enable running state
  m_running = true;

  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Stop calling the provided callback
void EveryTimer::Stop()
{
  // Set running flag accordingly
  m_running = false;
}

//////////////////////////////////////////////////////////////////////////////
// Start calling the provided callback, if a stop command was issued
void EveryTimer::Start()
{
  // Set running flag accordingly
  m_running = true;
  
  // Execute the callback now
  (*Callback)();
  m_lastRunTimestamp = millis();
}