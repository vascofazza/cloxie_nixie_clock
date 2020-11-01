#include "clock.hpp"

ClockDriver::ClockDriver(TubeDriver *tube_driver)
{
  this->tube_driver = tube_driver;
  display_time = false;
  display_date = false;
  ntpClock.setup();
  systemClock = new SystemClockLoop((Clock *)&ntpClock /*reference*/, nullptr /*backup*/);
  systemClock->setup();
  systemClock->setNow(0);
}

void ClockDriver::loop()
{
  static acetime_t prevNow = systemClock->getNow();
  systemClock->loop();
  acetime_t now = systemClock->getNow();

  auto time_zone = TimeZone::forZoneInfo(zonedb::kZoneRegistry[config.timezone], &zoneProcessor);
  auto current_time = ZonedDateTime::forEpochSeconds(now, time_zone);

  if (display_time && now - prevNow >= 1)
  {
    print_current_time(current_time);
    prevNow = now;
  }
  else if (display_date)
  {
    print_current_date(current_time);
  }
}

void ClockDriver::print_current_time(ace_time::ZonedDateTime current_time)
{
  current_time.printTo(Serial);
  Serial.println();

  auto hour = current_time.hour();
  if (!config.h24)
  {
    hour %= 12;
  }
  tube_driver->display_time_and_date(hour, current_time.minute(), current_time.second());
  tube_driver->blink_dots();
}

void ClockDriver::print_current_date(ace_time::ZonedDateTime current_time)
{
  auto day = current_time.day();
  auto month = current_time.month();
  auto year = current_time.yearTiny();

  tube_driver->display_time_and_date(day, month, year);
  tube_driver->set_dots_brightness(PWMRANGE, PWMRANGE);
}

void ClockDriver::show_time(bool show)
{
  display_time = show;
  if (show)
    show_date(false);
}

void ClockDriver::show_date(bool show)
{
  display_date = show;
  if (show)
    show_time(false);
}
