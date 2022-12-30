#pragma once
#include <BleMouse.h>

void bleMouseSetup(Print *p, BleMouse *ble_mouse) {
  p->println(PSTR("blemouse setup"));

  ble_mouse->begin();
  p->printf(PSTR("Waiting for Bluetooth connection."));
  while (!ble_mouse->isConnected()) {
    p->printf(".");
    delay(500);
  }

  p->println(PSTR("blemouse setup done"));
}
