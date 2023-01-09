#include "Arduino.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "common.h"
#include "config.h"

extern AsyncWebServer *web_server;
void web_init_server();
