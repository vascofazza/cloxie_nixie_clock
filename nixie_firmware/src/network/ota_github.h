#include <ESP_OTA_GitHub.h>
#include <CertStoreBearSSL.h>
#include "wifi.hpp"
#include "../configuration.hpp"

BearSSL::CertStore certStore;
bool cert_initialized = false;
bool first_boot = true;

bool setup_cert_store()
{
  if (cert_initialized)
    return true;
  SPIFFS.begin();
  int numCerts = certStore.initCertStore(SPIFFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  DEBUG_PRINT(F("Number of CA certs read: "));
  DEBUG_PRINTLN(numCerts);
  if (numCerts == 0)
  {
    DEBUG_PRINTLN(F("No certs found. Did you run certs-from-mozill.py and upload the SPIFFS directory before running?"));
    return false; // Can't connect to anything w/o certs!
  }

  if ((WiFi.status() != WL_CONNECTED))
  {
    DEBUG_PRINTLN(F("WiFi not connected. Skipping."));
    return false;
  }
  cert_initialized = true;
  return true;
}

void check_for_updates()
{
  if ((WiFi.status() != WL_CONNECTED))
  {
    DEBUG_PRINTLN(F("WiFi not connected. Skipping."));
    return;
  }
  if (!setup_cert_store())
  {
    DEBUG_PRINT(F("CertStore cannot be initialized. Skipping."));
    return;
  }
  ESPOTAGitHub API(&certStore, GHOTA_USER, GHOTA_REPO, GHOTA_CURRENT_TAG, GHOTA_BIN_FILE, GHOTA_ACCEPT_PRERELEASE);
  DEBUG_PRINTLN(F("Checking for update..."));
  if (API.checkUpgrade())
  {
    DEBUG_PRINT(F("Upgrade found at: "));
    DEBUG_PRINTLN(API.getUpgradeURL());
    if (!first_boot)
      ESP.reset();
    if (API.doUpgrade())
    {
      DEBUG_PRINTLN(F("Upgrade complete.")); //This should never be seen as the device should restart on successful upgrade.
    }
    else
    {
      DEBUG_PRINT(F("Unable to upgrade: "));
      DEBUG_PRINTLN(API.getLastError());
    }
  }
  else
  {
    DEBUG_PRINT(F("Not proceeding to upgrade: "));
    DEBUG_PRINTLN(API.getLastError());
  }
  first_boot = false;
}