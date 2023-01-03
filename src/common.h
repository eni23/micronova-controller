#include "Arduino.h"
#include "config.h"

#include <Preferences.h>
#include "micronova_stove.h"
#include "ESP32Console.h"
using namespace ESP32Console;

extern MicronovaStove stove;
extern Preferences settings;

#define STOVE_SEMAPHORE_WAIT_TICKS  30
extern SemaphoreHandle_t xStoveSemaphore;
