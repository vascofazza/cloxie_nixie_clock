#include "clock.hpp"
#include "blink_patterns.hpp"

BlinkPatternList blink_patterns = {no_blink, static_blink, double_static_blink, fade_out_blink, static_on};

ClockDriver::ClockDriver(TubeDriver *tube_driver)
{
  this->tube_driver = tube_driver;
  display_time = false;
  display_date = false;
  display_timer = false;
  ntpClock.setup();
  systemClock = new SystemClockLoop((Clock *)&ntpClock /*reference*/, nullptr /*backup*/);
  systemClock->setup();
  systemClock->setNow(0);
  blinking = false;
  timer_running = false;
  timer_duration = -1;
  current_timer_value = 0;
}

void ClockDriver::loop()
{
  static acetime_t prevNow = systemClock->getNow();
  systemClock->loop();
  acetime_t now = systemClock->getNow();

  auto time_zone = TimeZone::forZoneInfo(zonedb::kZoneRegistry[config.timezone], &zoneProcessor);
  auto current_time = ZonedDateTime::forEpochSeconds(now, time_zone);

  if (display_timer)
  {
    if (timer_running)
    {
      blink_dots(double_static_blink);
    }
    else if (is_timer_set())
    {
      blink_dots(static_blink);
    }
    else
    {
      blink_dots(static_on);
    }
    
    print_timer();
  }
  else if (display_time)
  {
    blink_dots(blink_patterns[config.blink_mode]);
    print_current_time(current_time);
  }
  else if (display_date)
  {
    print_current_date(current_time);
  }

  if (now - prevNow >= 1)
  {
    blinking = true;
    prevNow = now;
    current_time.printTo(Serial);
    Serial.println();
  }
}

void ClockDriver::blink_dots(void (*pattern)(TubeDriver *, bool, elapsedMillis *))
{
  static elapsedMillis elapsed;
  pattern(tube_driver, blinking, &elapsed);
  blinking = false;
}

void ClockDriver::print_current_time(ace_time::ZonedDateTime current_time)
{
  auto hour = current_time.hour();
  if (!config.h24)
  {
    hour %= 12;
  }
  tube_driver->display_time_and_date(hour, current_time.minute(), current_time.second(), false);
}

void ClockDriver::print_current_date(ace_time::ZonedDateTime current_time)
{
  auto day = current_time.day();
  auto month = current_time.month();
  auto year = current_time.yearTiny();

  tube_driver->display_time_and_date(day, month, year, false);
  tube_driver->set_dots_brightness(PWMRANGE, PWMRANGE);
}

void ClockDriver::print_timer()
{
  long val = timer_running ? timer_duration - (long)current_timer_value : timer_duration;
  if (val <= 0)
    stop_timer();

  unsigned long durCS = (val % 1000) / 10;   //Cent-seconds
  unsigned long durSS = (val / 1000) % 60;    //Seconds
  unsigned long durMM = (val / (60000)) % 60; //Minutes
  unsigned long durHH = (val / (3600000));    //HoursÒ
  durHH = durHH % 24;

  if (durHH > 0)
  {
    tube_driver->display_time_and_date(-durHH, durMM, durSS, true);
  }
  else
  {
    tube_driver->display_time_and_date(-durMM, durSS, durCS, true);
  }
}

void ClockDriver::show_time(bool show)
{
  display_time = show;
  if (show)
  {
    show_date(false);
    show_timer(false);
  }
}

void ClockDriver::show_date(bool show)
{
  display_date = show;
  if (show)
  {
    show_time(false);
    show_timer(false);
  }
}

void ClockDriver::show_timer(bool show)
{
  display_timer = show;
  if (show)
  {
    show_time(false);
    show_date(false);
  }
}

void ClockDriver::start_timer(long duration)
{
  if (duration > 0)
  {
    timer_duration = duration;
    timer_running = true;
    current_timer_value = 0;
  }
  else
  {
    if (is_timer_set())
    {
      timer_running = true;
      current_timer_value = 0;
    }
  }
}

void ClockDriver::stop_timer()
{
  timer_running = false;
  timer_duration = max(timer_duration - (long)current_timer_value, 0L);
}

void ClockDriver::reset_timer()
{
  timer_running = false;
  timer_duration = -1;
}

bool ClockDriver::is_timer_set()
{
  return timer_duration > 0;
}

bool ClockDriver::is_timer_running()
{
  return timer_running;
}
