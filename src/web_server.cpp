#include "Arduino.h"
#include "web_server.h"

#include "web_content.h"

AsyncWebServer *web_server;

// TODO: merge with function in console.cpp
uint8_t hexstr2uint(String s){
    uint8_t ch = '\0';
    sscanf(s.c_str(), "%x", &ch);
    return ch;
}


void web_init_server(){

    // TODO: make port configurable
    web_server = new AsyncWebServer(WEB_SERVER_PORT);


    printf("init web server \n");
    web_server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", web_index_html);
    });

    // read-ram, supports multiple addr, comma seperated
    web_server->on("/api/ram", HTTP_GET, [](AsyncWebServerRequest *request){
        String addrs;
        String answer="{";
        if (request->hasParam("addr")) {
            addrs = request->getParam("addr")->value();
            // check if length is OK
            if (sizeof(addrs)>80){
                request->send(500, "text/plain", "err_size");
                return;
            }
            // check for stove lock
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                request->send(500, "text/plain", "err_lock");
                return;
            }
            // split entries, and prepare response json
            char buff[80];
            addrs.toCharArray(buff, sizeof(buff));
            char *ptr;
            ptr = strtok(buff, ","); 
            while(ptr != NULL){
                uint8_t val = hexstr2uint(ptr);
                stove.read_ram(val);
                answer += "\"" + String(ptr) + "\":\""+stove.last_read_value+"\"";
                ptr = strtok(NULL, ",");
                if (ptr) {
                    answer+=",";
                }
            }
            xSemaphoreGive(xStoveSemaphore);
        }
        answer +="}";
        request->send(200, "text/json", answer);
    });


    // turn stove on
    web_server->on("/api/on", HTTP_POST, [](AsyncWebServerRequest *request){
        // check for stove lock
        if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
            request->send(500, "text/plain", "err_lock");
            return;
        }
        // check if state is allowed to turn off
        uint8_t state = stove.get_state();
        if ( state != 0 ){
            xSemaphoreGive(xStoveSemaphore);
            request->send(500, "text/plain", "err_stovestate");
            return;
        }
        stove.on();
        xSemaphoreGive(xStoveSemaphore);
        String answer="{\"status\":\"ok\"}";
        request->send(200, "text/plain", "OK");
    });

    // turn stove off
    web_server->on("/api/off", HTTP_POST, [](AsyncWebServerRequest *request){
        // check for stove lock
        if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
            request->send(500, "text/plain", "err_lock");
            return;
        }
        // check if state is allowed to turn off, (only in state 1 and 4)
        uint8_t state = stove.get_state();
        if ( state !=1 || state != 4 ){
            xSemaphoreGive(xStoveSemaphore);
            request->send(500, "text/plain", "err_stovestate");
            return;
        }
        stove.off();
        xSemaphoreGive(xStoveSemaphore);
        String answer="{\"status\":\"ok\"}";
        request->send(200, "text/plain", "OK");
    });

    // set power
    web_server->on("/api/power", HTTP_POST, [](AsyncWebServerRequest *request){        
        if (request->hasParam("go", true)) {
            String go = request->getParam("go", true)->value();
            // check for stove lock
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                request->send(500, "text/plain", "err_lock");
                return;
            }
            uint8_t cmd;
            if (go == "up"){
                cmd = 0x54;
            }
            if ( go =="down" ) {
                cmd = 0x50;
            }
            for (uint8_t i=0; i<3; i++){
                stove.write_ram(STOVE_ADDR_IRCOMMAND, cmd);
            }
            xSemaphoreGive(xStoveSemaphore);
            request->send(200, "text/json", "OK");
            return;
        } 
        request->send(500, "text/json", "err_param");
    });


    // start the server
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    web_server->begin();

}