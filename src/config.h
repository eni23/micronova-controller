#ifndef __CONFIG_H
  #define __CONFIG_H
  
  #define SERIAL_BAUD     115200
  #ifndef TCP_SERVER_PORT
    #define TCP_SERVER_PORT   9040
  #endif

  #define STOVE_PIN_RX        27
  #define STOVE_PIN_TX        26
  #define STOVE_PIN_EN_RX     25
  #define PREFERENCES_NAMESPACE "mst-0.1"

#endif
