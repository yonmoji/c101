#pragma once
#include "app.h"

class OkCtrl {
public:
  App *app;

  OkCtrl(App *app) : app(app) {}

  void handleOk() {
    app->logRequest();
    app->server->send(200, APP_CONTENT_TYPE_TEXT_PLAIN_UTF8, "OK");
  }
};

