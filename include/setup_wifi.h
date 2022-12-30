#pragma once
#include <WiFi.h>

// Connect WiFi + Sync NTP
void wifiSetup(Print *p, const char* ssid, const char* password) {
  p->println(PSTR("WiFi+NPF setup start"));

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    p->print('.');
  }
  p->printf(PSTR("WiFi connected. IP address: %s\n"), WiFi.localIP().toString());

  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");
  struct tm timeinfo;
  p->println(PSTR("getLocalTime"));
  while (!getLocalTime(&timeinfo)) {
    p->print('.');
  }
  time_t now;
  time(&now);
  p->println(now);
  p->println(PSTR("WiFi+NTP setup done"));
}
