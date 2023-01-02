#include "Arduino.h"
#include <AsyncTCP.h>

#include "common.h"
#include "config.h"


extern bool tcp_debug_on;
//extern AsyncServer* server;
//extern AsyncServer server;

extern void tcp_handle_new_cient(void* arg, AsyncClient* client);
extern void tcp_handle_timeout(void* arg, AsyncClient* client, uint32_t time);
extern void tcp_handle_disconnect(void* arg, AsyncClient* client);
extern void tcp_handle_data(void* arg, AsyncClient* client, void *data, size_t len);
extern void tcp_handle_error(void* arg, AsyncClient* client, int8_t error);

extern void tcp_init_server();
