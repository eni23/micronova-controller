#include "Arduino.h"
#include "common.h"
#include "config.h"
#include "console.h"



// a tick occurs every 250ms
#define WIFI_TRY_CONNECT_TICKS 240

extern uint32_t wifi_connect_ticks;

extern void wifi_connect();
extern void wifi_disconnect();
extern void wifi_reconnect();

