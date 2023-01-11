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
    web_server = new AsyncWebServer(settings.getUInt("web-port", WEB_SERVER_PORT));


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

    // read-eeprom, supports multiple addr, comma seperated
    // TODO: combine with eeprom-code
    web_server->on("/api/eeprom", HTTP_GET, [](AsyncWebServerRequest *request){
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
                stove.read_eeprom(val);
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

    // write-ram
    // POST only supports one param
    web_server->on("/api/ram", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("addr", true) && request->hasParam("value", true)) {
            String answer = "OK";
            String addr = request->getParam("addr", true)->value();
            String value = request->getParam("addr", true)->value();
            uint8_t addr_s = hexstr2uint(addr);
            uint8_t value_s = hexstr2uint(value);
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                request->send(500, "text/plain", "err_lock");
                return;
            }
            stove.write_ram(addr_s, value_s);
            if (request->hasParam("read_response", true)){
                stove.read_answer();
                answer = stove.last_read_value;
            }
            xSemaphoreGive(xStoveSemaphore);
            request->send(200, "text/plain", answer);
        } else {
            request->send(500, "text/plain", "err_param");
            return;
        }
    });
    // write-eeprom
    // TODO: merge with ram-write since mostly dual code
    web_server->on("/api/eeprom", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("addr", true) && request->hasParam("value", true)) {
            String answer = "OK";
            String addr = request->getParam("addr", true)->value();
            String value = request->getParam("addr", true)->value();
            uint8_t addr_s = hexstr2uint(addr);
            uint8_t value_s = hexstr2uint(value);
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                request->send(500, "text/plain", "err_lock");
                return;
            }
            stove.write_ram(addr_s, value_s);
            if (request->hasParam("read_response", true)){
                stove.read_answer();
                answer = stove.last_read_value;
            }
            xSemaphoreGive(xStoveSemaphore);
            request->send(200, "text/plain", answer);
        } else {
            request->send(500, "text/plain", "err_param");
            return;
        }
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
        if ( state != 0x01 && state != 0x04 ){
            xSemaphoreGive(xStoveSemaphore);
            request->send(500, "text/plain", "err_stovestate");
            return;
        }
        printf("turn stove off \n");
        stove.off();
        xSemaphoreGive(xStoveSemaphore);
        request->send(200, "text/plain", "OK");
    });

    // set power
    web_server->on("/api/power", HTTP_POST, [](AsyncWebServerRequest *request){        
        if (request->hasParam("go", true)) {
            String go = request->getParam("go", true)->value();
            if (go != "up" && go != "down"){
                request->send(500, "text/plain", "err_param");
                return;
            }
            // check for stove lock
            if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
                request->send(500, "text/plain", "err_lock");
                return;
            }
            // get current power
            stove.read_eeprom(STOVE_ADDR_POWER_EEPROM);
            uint8_t pwr = stove.last_read_value;
            if (go == "up"){
                if (pwr<4){
                    pwr++;
                }
            }
            if ( go =="down" ) {
                if (pwr > 1){
                    pwr--;
                }
            }
            stove.write_eeprom(STOVE_ADDR_POWER_EEPROM, pwr);
            xSemaphoreGive(xStoveSemaphore);
            request->send(200, "text/plain", "OK");
            return;
        } 
        request->send(500, "text/plain", "err_param");
    });


    // start the server
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    web_server->begin();

}