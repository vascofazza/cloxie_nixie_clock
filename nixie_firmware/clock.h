#include <AceTime.h>
using namespace ace_time;
using namespace ace_time::clock; //ace time library

// ZoneProcessor instance should be created statically at initialization time.
static BasicZoneProcessor zoneProcessor;

static NtpClock ntpClock;
static SystemClockLoop systemClock((Clock*) &ntpClock /*reference*/, nullptr /*backup*/);

void setup_clock() {

  ntpClock.setup();
  systemClock.setup();

  systemClock.setNow(0);
}

void printCurrentTime() {
  acetime_t now = systemClock.getNow();

  // Create Pacific Time and print.
  auto time_zone = TimeZone::forZoneInfo(zonedb::kZoneRegistry[config.timezone],
                                         &zoneProcessor);
  auto current_time = ZonedDateTime::forEpochSeconds(now, time_zone);
  current_time.printTo(Serial);
  Serial.println();

  display_time_and_date(current_time.hour(), current_time.minute(), current_time.second());
}

void clock_loop(bool display) {
  static acetime_t prevNow = systemClock.getNow();
  systemClock.loop();
  acetime_t now = systemClock.getNow();

  if (display && now - prevNow >= 2) {
    printCurrentTime();
    prevNow = now;
  }
}
