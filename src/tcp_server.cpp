#include "Arduino.h"
#include "tcp_server.h"
#include <WiFi.h>


bool tcp_debug_on = true;
//AsyncServer* server;


void tcp_handle_data(void* arg, AsyncClient* client, void *data, size_t len){

    if (tcp_debug_on) printf("data received from client %s \n", client->remoteIP().toString().c_str());
    //Serial.write((uint8_t*)data, len);

    // reply to client
    if (client->space() > 32 && client->canSend()){
        char reply[32];
        sprintf(reply, "this is from esp\n");
        client->add(reply, strlen(reply));
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


// TODO: find out why this crashes but works in setup()
void tcp_init_server(){
    if (tcp_debug_on) printf("starting server on port %s\n", TCP_SERVER_PORT);

    WiFi.mode(WIFI_STA);
    AsyncServer* server = new AsyncServer(TCP_SERVER_PORT); 
    server->onClient(&tcp_handle_new_cient, server);
    server->begin();
}
