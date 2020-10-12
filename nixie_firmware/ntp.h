#include <AceTime.h>
using namespace ace_time;
using namespace ace_time::clock;

// ZoneProcessor instance should be created statically at initialization time.
static BasicZoneProcessor pacificProcessor;

static NtpClock ntpClock;
static SystemClockLoop systemClock((Clock*) &ntpClock /*reference*/, nullptr /*backup*/);

void setup_ntp() {

  ntpClock.setup();
  systemClock.setup();

  // Creating timezones is cheap, so we can create them on the fly as needed.
  auto pacificTz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_Los_Angeles,
                                         &pacificProcessor);

  // Set the SystemClock using these components.
  auto pacificTime = ZonedDateTime::forComponents(
                       2019, 6, 17, 19, 50, 0, pacificTz);
  systemClock.setNow(pacificTime.toEpochSeconds());
}

void printCurrentTime() {
  acetime_t now = systemClock.getNow();

  // Create Pacific Time and print.
  auto pacificTz = TimeZone::forZoneInfo(&zonedb::kZoneEurope_Madrid,
                                         &pacificProcessor);
  auto pacificTime = ZonedDateTime::forEpochSeconds(now, pacificTz);
  pacificTime.printTo(Serial);
  Serial.println();
}

// Do NOT use delay(), it breaks systemClock.loop()
void ntp_loop() {
  static acetime_t prevNow = systemClock.getNow();
  systemClock.loop();
  acetime_t now = systemClock.getNow();
  if (now - prevNow >= 2) {
    printCurrentTime();
    prevNow = now;
  }
}
