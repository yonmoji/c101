#include <M5Atom.h>

#include "ctrl_ok.h"
#include "ctrl_logger.h"
#include "ctrl_ble_mouse.h"
#include "env_const.h"
#include "print_buffered.h"
#include "setup_ble_mouse.h"
#include "setup_wifi.h"

WebServer g_server(80);
BleMouse g_ble_mouse(EnvConst.BLE_DEVICE_NAME);

auto g_buff_cap = 400;
BufferedPrint g_bprint(std::make_unique<uint8_t[]>(g_buff_cap), g_buff_cap);
MultiPrint g_mprint(
  std::vector<Print*>{
    &Serial,
    &g_bprint,
  }
);

App g_app{
  .p=&g_mprint,
  .server=&g_server,
  .ble_mouse=&g_ble_mouse,
};
OkCtrl g_ok_ctrl(&g_app);
LoggerCtrl g_logger_ctrl(&g_app, &g_bprint);
BleMouseCtrl g_ble_mouse_ctrl(&g_app);

void webServerSetup();

void setup() {
  M5.begin(true, false, true);
  Serial.begin(115200);
  delay(50);
  auto app = &g_app;

  app->fillpix(0x555500); // yellow
  wifiSetup(&g_mprint, EnvConst.SSID, EnvConst.PASSWORD);
  webServerSetup();
  bleMouseSetup(&g_mprint, &g_ble_mouse);
  g_ble_mouse_ctrl.

  app->p->printf("setup done\n");
  app->fillpix(0x001100); // green
}

void loop() {
  g_server.handleClient();
  delay(1);
}

void webServerSetup() {
  g_server.on("/ok", HTTP_GET, []() { g_ok_ctrl.handleOk(); });
  g_server.on("/logs", HTTP_GET, []() { g_logger_ctrl.handleGetLog(); });
  g_server.on("/mouse/report", HTTP_POST, []() { g_ble_mouse_ctrl.handleMouseReport(); });
  g_server.on("/mouse/reports", HTTP_POST, []() { g_ble_mouse_ctrl.handleMouseReports(); });
  g_server.on("/mouse/address", HTTP_GET, []() { g_ble_mouse_ctrl.handleGetAddress(); });

  // // send CrossOriginHeader
  // server.on("/", HTTP_OPTIONS, [=](){ server.send(204); });
  // server.on(UriBraces("/{}"), HTTP_OPTIONS, [=](){ server.send(204); });
  // server.on(UriBraces("/{}/{}"), HTTP_OPTIONS, [=](){ server.send(204); });
  // server.on(UriBraces("/{}/{}/{}"), HTTP_OPTIONS, [=](){ server.send(204); });
  // server.enableCORS();

  g_server.begin();
}

