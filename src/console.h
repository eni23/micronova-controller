#include "Arduino.h"
#include "common.h"

extern Console console;
uint8_t convert_str_to_hex(String s);
bool aqquire_stove_lock();
void release_stove_lock();
void init_console();
