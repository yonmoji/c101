#pragma once

struct {

#ifndef ENV_WIFI_SSID
#define ENV_WIFI_SSID ""
#pragma message "ENV_WIFI_SSID not defined"
#endif
  const char* SSID = ENV_WIFI_SSID;

#ifndef ENV_WIFI_PASSWORD
#define ENV_WIFI_PASSWORD ""
#pragma message "ENV_WIFI_PASSWORD not defined"
#endif
  const char* PASSWORD = ENV_WIFI_PASSWORD;

#ifndef ENV_BLE_DEVICE_NAME
#define ENV_BLE_DEVICE_NAME ""
#pragma message "ENV_BLE_DEVICE_NAME not defined"
#endif
  const char* BLE_DEVICE_NAME = ENV_BLE_DEVICE_NAME;

} EnvConst;
