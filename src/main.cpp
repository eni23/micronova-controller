#include "Arduino.h"
#include "config.h"

#include "common.h"
#include "micronova_stove.h"
#include "console.h"
#include "wifi.h"


MicronovaStove stove = MicronovaStove(
    STOVE_PIN_RX,
    STOVE_PIN_TX,
    STOVE_PIN_EN_RX
);
Preferences settings;



void setup() {
    settings.begin(PREFERENCES_NAMESPACE, false); 
    stove.init();
    init_console();
}

void loop() {

}
