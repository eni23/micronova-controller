#include "Arduino.h"
#include <AsyncTCP.h>

#include "common.h"
#include "config.h"



#define TCP_MAX_MSG_LEN       32

// tcp server commands
#define TCP_CMD_PING          0x01  
#define TCP_CMD_ON            0x02
#define TCP_CMD_OFF           0x03
#define TCP_CMD_READ_RAM      0x10
#define TCP_CMD_READ_EEPROM   0x11
#define TCP_CMD_WRITE_RAM     0x20 
#define TCP_CMD_WRITE_EEPROM  0x21

#define TCP_ERR_GENERAL       0xFF
#define TCP_ERR_LOCK          0xFE
#define TCP_ERR_CHECKSUM      0xFE


extern bool tcp_debug_on;

extern void tcp_handle_new_cient(void* arg, AsyncClient* client);
extern void tcp_handle_timeout(void* arg, AsyncClient* client, uint32_t time);
extern void tcp_handle_disconnect(void* arg, AsyncClient* client);
extern void tcp_handle_data(void* arg, AsyncClient* client, void *data, size_t len);
extern void tcp_handle_error(void* arg, AsyncClient* client, int8_t error);

extern void tcp_init_server();
