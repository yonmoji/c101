#pragma once
#include <BleMouse.h>
#include <FastLED.h>
#include <M5Atom.h>
#include <WebServer.h>

#include "print_buffered.h"

auto APP_CONTENT_TYPE_TEXT_PLAIN_UTF8 = PSTR("text/plain; charset=utf-8");
auto APP_CONTENT_TYPE_APPLICATION_JSON = PSTR("application/json");

struct App {
  Print *p;
  WebServer *server;
  BleMouse *ble_mouse;
  CRGB pixel = 0x000000; // black

  void fillpix(const CRGB color) {
    pixel = color;
    M5.dis.fillpix(pixel);
  }

  void logRequest() {
    p->printf(PSTR("accessed on %s\n"), server->uri().c_str());
  }
};
