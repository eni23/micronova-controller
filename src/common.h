#include "Arduino.h"
#include "config.h"

#include <Preferences.h>
#include "micronova_stove.h"
#include "ESP32Console.h"
using namespace ESP32Console;

extern MicronovaStove stove;
extern Preferences settings;