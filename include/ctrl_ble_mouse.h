#pragma once
#include <ArduinoJson.h>
#include <BleMouse.h>
#include <WebServer.h>

#include "app.h"

NimBLECharacteristic* findNimBleCharacteristic(BleMouse *ble_mouse);
std::vector<NimBLEAddress> findBlePeerAddresses(NimBLECharacteristic *characteristic);

class BleMouseCtrl {
private:
  NimBLECharacteristic *_ble_characteristic;

public:
  App *app;

  BleMouseCtrl(App *app) : app(app) {}

  NimBLECharacteristic* bleCharacteristic() {
    if (_ble_characteristic == nullptr) {
      _ble_characteristic = findNimBleCharacteristic(app->ble_mouse);
    }
    return _ble_characteristic;
  }

  // payload を 5 byte づつ送る。
  size_t mouseReports(uint8_t *payload, size_t length) {
    if (!app->ble_mouse->isConnected()) {
      app->p->printf("mouse not connected\n");
      return 0;
    }
    app->p->printf("get chara\n");
    auto characteristic = bleCharacteristic();
    auto i = 0;
    while (i + 4 < length) {
      app->p->printf("notify chara\n");
      characteristic->notify(&payload[i], 5);
      i += 5;
    }
    return i;
  }

  void handleMouseReport() {
    app->logRequest();
    String jsontext = app->server->arg("plain");
    const size_t cap= JSON_OBJECT_SIZE(10);
    StaticJsonDocument<cap> doc;
    deserializeJson(doc, jsontext);

    JsonObject jsonObj = doc.as<JsonObject>();
    uint8_t m[5];
    m[0] = jsonObj["button"] | 0;
    m[1] = jsonObj["x"] | 0;
    m[2] = jsonObj["y"] | 0;
    m[3] = jsonObj["wheel"] | 0;
    m[4] = jsonObj["hwheel"] | 0;
    app->p->printf("xxx");
    app->p->printf("%s", m);
    mouseReports(m, 5);
    app->server->send(200, APP_CONTENT_TYPE_TEXT_PLAIN_UTF8, "OK");
  }

  void handleMouseReports() {
    app->logRequest();
    String jsontext = app->server->arg("plain");
    const size_t cap= JSON_OBJECT_SIZE(80);
    StaticJsonDocument<cap> doc;
    deserializeJson(doc, jsontext);

    JsonArray jsonArr = doc.as<JsonArray>();
    auto sz = jsonArr.size();
    uint8_t m[sz*5];
    for (auto i=0; i<sz; i++) {
      JsonObject jsonObj = jsonArr[i].as<JsonObject>();
      m[i*5+0] = jsonObj["button"] | 0;
      m[i*5+1] = jsonObj["x"] | 0;
      m[i*5+2] = jsonObj["y"] | 0;
      m[i*5+3] = jsonObj["wheel"] | 0;
      m[i*5+4] = jsonObj["hwheel"] | 0;
    }
    mouseReports(m, sz*5);
    app->server->send(200, APP_CONTENT_TYPE_TEXT_PLAIN_UTF8, "OK");
  }

  void handleGetAddress() {
    app->logRequest();
    auto characteristic = bleCharacteristic();
    auto addrs = findBlePeerAddresses(characteristic);

    StaticJsonDocument<20> doc;
    JsonArray array = doc.to<JsonArray>();
    for (auto addr: addrs) {
      array.add(addr.toString().c_str());
    }
    char buf[255];
    serializeJson(array, buf);
    app->server->send(200, APP_CONTENT_TYPE_APPLICATION_JSON, buf);
  }
};

// BleMouse から NimBLECharacteristic を参照したい。
// NimBLECharacteristic は BleConnectionStatus から辿れる。
//
// BleMouse は次のようなクラス定義をしている。
//   class BleMouse {
//   private:
//     uint8_t _buttons;
//     BleConnectionStatus* connectionStatus;
//     ..
//   public:
//     ..
//   protected:
//     virtual void onStarted(NimBLEServer *pServer) { };
//   };
NimBLECharacteristic* findNimBleCharacteristic(BleMouse *ble_mouse) {
  NimBLECharacteristic* characteristic;
  if (ble_mouse == nullptr) { return characteristic; }
  auto pt = (char*)ble_mouse; // char is 1byte
  pt = pt + 4; // vtable + 4byte => _button
  pt = pt + 1 + 3; // _button 1byte + padding 3byte => connectionStatus
  auto conn_status = (BleConnectionStatus*)pt;
  characteristic = conn_status->inputMouse;
  return characteristic;
}

std::vector<NimBLEAddress> findBlePeerAddresses(NimBLECharacteristic *characteristic) {
  std::vector<NimBLEAddress> v;
  if (characteristic == nullptr) {
    return v;
  }
  NimBLEServer *svr = characteristic->getService()->getServer();

  // size_t cnt = svr->getConnectedCount();
  for (uint16_t id: svr->getPeerDevices()) {
    NimBLEConnInfo peer_info = svr->getPeerIDInfo(id);
    NimBLEAddress addr = peer_info.getIdAddress();
    v.push_back(addr);
  }
  return v;
}

