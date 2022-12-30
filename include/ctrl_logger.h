#pragma once
#include "app.h"

class LoggerCtrl {
public:
  App *app;
  BufferedPrint *bp;

  LoggerCtrl(App *app, BufferedPrint *bp) : app(app), bp(bp) {}

  void handleGetLog() {
    app->logRequest();
    auto cap = bp->cap;
    uint8_t buf[cap+1] = {};
    auto sz = bp->dump(buf, cap);
    app->server->send(200, APP_CONTENT_TYPE_TEXT_PLAIN_UTF8, (char *)buf);
  }
};
