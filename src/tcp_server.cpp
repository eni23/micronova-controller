#include "Arduino.h"
#include "tcp_server.h"

#include "crc8.h"

bool tcp_debug_on = true;

void tcp_handle_data(void* arg, AsyncClient* client, void *data, size_t len){

    if (tcp_debug_on) printf("data received from client %s \n", client->remoteIP().toString().c_str());

    uint8_t rcv_data[TCP_MAX_MSG_LEN];
    memcpy(rcv_data, (uint8_t*)data, len);

    uint8_t cmd = rcv_data[0];
    uint8_t reply_len = 0;
    char reply[8];

    // checksum calculation
    uint8_t checksum = crc8((uint8_t*)data, len-1);
    if (checksum != rcv_data[len-1]){
        cmd = TCP_ERR_CHECKSUM;
    }
    
    // remove checksum byte from length
    len = len-1;

    switch (cmd){

        case TCP_CMD_PING:
            reply[0] = 0x01;
            reply_len = 1;
            break;


        case TCP_CMD_ON:
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                reply[0] = TCP_ERR_LOCK;
                reply_len = 1; 
                break;
            }
            stove.on();
            reply[0] = TCP_CMD_ON;
            reply_len = 1;
            xSemaphoreGive(xStoveSemaphore);
            break;


        case TCP_CMD_OFF:
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                reply[0] = TCP_ERR_LOCK;
                reply_len = 1; 
                break;
            }
            stove.off();
            reply[0] = TCP_CMD_OFF;
            reply_len = 1;
            xSemaphoreGive(xStoveSemaphore);
            break;


        case TCP_CMD_READ_RAM:
        case TCP_CMD_READ_EEPROM:
            if (len!=2){
                reply[0] = TCP_ERR_GENERAL;
                reply_len = 1;
                break;
            }
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                reply[0] = TCP_ERR_LOCK;
                reply_len = 1; 

                break;
            }
            if (cmd == TCP_CMD_READ_RAM){
                stove.read_ram(rcv_data[1]);
            } 
            else {
                stove.read_eeprom(rcv_data[1]);
            }
            reply[0] = stove.last_read_checksum;
            reply[1] = stove.last_read_value;
            reply_len = 2;
            xSemaphoreGive(xStoveSemaphore);
            break;


        case TCP_CMD_WRITE_RAM:
        case TCP_CMD_WRITE_EEPROM:
            if (len<3){
                reply[0] = TCP_ERR_GENERAL;
                reply_len = 1;
                break;
            }
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                reply[0] = TCP_ERR_LOCK;
                reply_len = 1; 
                break;
            }
            if (cmd == TCP_CMD_WRITE_RAM){
                stove.write_ram(rcv_data[1], rcv_data[2]);
            } 
            else {
                stove.write_eeprom(rcv_data[1], rcv_data[2]);
            }
            if ( len==4 & rcv_data[3]==0x01 ){
                stove.read_answer();
                reply[0] = stove.last_read_value;
                reply[1] = stove.last_read_value;
                reply_len = 2;
            } else {
                reply[0] = cmd;
                reply_len = 1;
            }
            xSemaphoreGive(xStoveSemaphore);
            break;


        case TCP_CMD_IR_CMD:
            if (len!=3){
                reply[0] = TCP_ERR_GENERAL;
                reply_len = 1;
                break;
            }
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                reply[0] = TCP_ERR_LOCK;
                reply_len = 1; 
                break;
            }
            reply[0] = TCP_CMD_IR_CMD;
            reply_len = 1;
            stove.simulate_infrared(rcv_data[1], rcv_data[2]);
            xSemaphoreGive(xStoveSemaphore);
            break;

        
        case TCP_ERR_CHECKSUM:
            reply[0] = TCP_ERR_CHECKSUM;
            reply_len = 1;
            break;


        default:
            reply[0] = TCP_ERR_NOCMD;
            reply_len = 1;
            break;
    
    }

    // reply to client
    if (client->space() > 32 && client->canSend()){
        // checksum addition
        uint8_t resp_chk = crc8((uint8_t*)reply, reply_len);
        reply[reply_len] = resp_chk;
        reply_len++;
        client->add(reply, reply_len);
        client->send();
    }
}



void tcp_handle_timeout(void* arg, AsyncClient* client, uint32_t time){
    if (tcp_debug_on) printf("client ACK timeout ip: %s \n", client->remoteIP().toString().c_str());
}


void tcp_handle_disconnect(void* arg, AsyncClient* client){
    if (tcp_debug_on) printf("client %s disconnected \n", client->remoteIP().toString().c_str());
}


void tcp_handle_error(void* arg, AsyncClient* client, int8_t error){
    if (tcp_debug_on){
        printf("connection error %s from client %s \n", 
            client->errorToString(error),
            client->remoteIP().toString().c_str()
        );
    }
}


void tcp_handle_new_cient(void* arg, AsyncClient* client){
    if (tcp_debug_on) printf("new client has been connected to server, ip: %s\n", client->remoteIP().toString().c_str());

    client->onData(&tcp_handle_data, NULL);
    client->onError(&tcp_handle_error, NULL);
    client->onDisconnect(&tcp_handle_disconnect, NULL);
    client->onTimeout(&tcp_handle_timeout, NULL);
}


void tcp_init_server(){
    AsyncServer* server = new AsyncServer(settings.getUInt("tcp-port", TCP_SERVER_PORT)); 
    server->onClient(&tcp_handle_new_cient, server);
    server->begin();
    printf("TCP server running on 0.0.0.0:%i\n", settings.getUInt("tcp-port", TCP_SERVER_PORT));
}
