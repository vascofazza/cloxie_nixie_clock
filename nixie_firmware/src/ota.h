#include <ESP_OTA_GitHub.h>
#include <CertStoreBearSSL.h>
#include "configuration.hpp"
#include "wifi.hpp"
#include "webserver.h"

BearSSL::CertStore certStore;
bool cert_initialized = false;

bool setup_cert_store()
{
  if (cert_initialized)
    return true;
  SPIFFS.begin();
  int numCerts = certStore.initCertStore(SPIFFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.print(F("Number of CA certs read: "));
  Serial.println(numCerts);
  if (numCerts == 0)
  {
    Serial.println(F("No certs found. Did you run certs-from-mozill.py and upload the SPIFFS directory before running?"));
    return false; // Can't connect to anything w/o certs!
  }

  if ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(F("WiFi not connected. Skipping."));
    return false;
  }
  cert_initialized = true;
  return true;
}

void check_for_updates()
{
  if ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(F("WiFi not connected. Skipping."));
    return;
  }
  if (!setup_cert_store())
  {
    Serial.print(F("CertStore cannot be initialized. Skipping."));
    return;
  }
  ESPOTAGitHub API(&certStore, GHOTA_USER, GHOTA_REPO, GHOTA_CURRENT_TAG, GHOTA_BIN_FILE, GHOTA_ACCEPT_PRERELEASE);
  Serial.println(F("Checking for update..."));
  if (API.checkUpgrade())
  {
    wifi_free_resources();
    stop_webserver();
    Serial.print(F("Upgrade found at: "));
    Serial.println(API.getUpgradeURL());
    if (API.doUpgrade())
    {
      Serial.println(F("Upgrade complete.")); //This should never be seen as the device should restart on successful upgrade.
    }
    else
    {
      Serial.print(F("Unable to upgrade: "));
      Serial.println(API.getLastError());
      setup_wifi(nullptr);
      start_webserver();
    }
  }
  else
  {
    Serial.print(F("Not proceeding to upgrade: "));
    Serial.println(API.getLastError());
  }
}