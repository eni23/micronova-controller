#include "Arduino.h"
#include <AsyncTCP.h>
#include <WiFi.h>
#include "config.h"

#include "common.h"
#include "micronova_stove.h"
#include "console.h"
#include "wifi.h"
#include "tcp_server.h"

MicronovaStove stove = MicronovaStove(
    STOVE_PIN_RX,
    STOVE_PIN_TX,
    STOVE_PIN_EN_RX
);
Preferences settings;
SemaphoreHandle_t xStoveSemaphore;




void setup() {
    
    // if this is not set, the server will crash it all
    WiFi.mode(WIFI_STA);
    
    // we begin stove and serial console before wifi
    stove.init();

    // we create a lock for the stove serial and give it to be used
    xStoveSemaphore = xSemaphoreCreateBinary(); 
    xSemaphoreGive(xStoveSemaphore);

    // load "EEPROM" settings
    settings.begin(PREFERENCES_NAMESPACE, false); 

    // start serial console
    init_console();
        
    // init tcp server
    tcp_init_server();

    // wifi is at the end, so it is no-blocking even when it's not
    wifi_connect();

}


void loop() {
    // it's all async :)
}
