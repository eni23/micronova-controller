#include "Arduino.h"
#include <WiFi.h>

#include "config.h"

#include "common.h"
#include "console.h"
#include "micronova_stove.h"
#include "wifi.h"
#include "tcp_server.h"



Console console;

uint8_t convert_str_to_hex(String s){
    uint8_t ch = '\0';
    sscanf(s.c_str(), "%x", &ch);
    return ch;
}

bool aqquire_stove_lock(){
    if( !xSemaphoreTake( xStoveSemaphore, ( TickType_t ) STOVE_SEMAPHORE_WAIT_TICKS ) == pdTRUE ){
        printf("err: could not aqquire lock on stove\n");
        return false;
    }
    return true;
}

void release_stove_lock(){
    xSemaphoreGive(xStoveSemaphore);
}



// this is a task that runs around 600ms after creation of the terminal to 
// set linemode correct when no terminal client is attached
void console_fix_linenoise(void* pvParameters){
    delay(600);
    linenoiseSetDumbMode(0);
    vTaskDelete(NULL);
}




void init_console(){

    console.begin(SERIAL_BAUD);
    console.setPrompt(CONSOLE_PROMPT);
    console.registerSystemCommands();
    console.registerGPIOCommands();

    // create fix task
    xTaskCreate(console_fix_linenoise, "lfx", 5000, NULL, 1, NULL);


    /****
    Stove functions
    ****/

    // read-ram
    console.registerCommand(ConsoleCommandD("read-ram", [](int argc, char **argv) -> int {
        if (argc != 2){
            printf("invalid number of arguments\n");
            return EXIT_FAILURE;
        }
        auto arg = String(argv[1]);
        if (!aqquire_stove_lock()) return EXIT_FAILURE;
        stove.read_ram(convert_str_to_hex(arg));
        release_stove_lock();
        printf(" returned chk=0x%02x data=0x%02x param=0x%02x dec=%d ascii=%c\n", 
            stove.stove_rx_data[0], 
            stove.stove_rx_data[1], 
            stove.last_read_param,
            stove.last_read_value,
            stove.last_read_value
        );

        return EXIT_SUCCESS;
    }, "Read RAM"));

    // read-eeprom
    console.registerCommand(ConsoleCommandD("read-eeprom", [](int argc, char **argv) -> int {
        if (argc != 2){
            printf("invalid number of arguments\n");
            return EXIT_FAILURE;
        }
        auto arg = String(argv[1]);
        if (!aqquire_stove_lock()) return EXIT_FAILURE;
        stove.read_eeprom(convert_str_to_hex(arg));
        release_stove_lock();
        printf(" returned chk=0x%02x data=0x%02x param=0x%02x dec=%d ascii=%c\n", 
            stove.stove_rx_data[0], 
            stove.stove_rx_data[1], 
            stove.last_read_param,
            stove.last_read_value,
            stove.last_read_value
        );        
        return EXIT_SUCCESS;
    }, "Read EEPROM"));


    // write-ram
    console.registerCommand(ConsoleCommandD("write-ram", [](int argc, char **argv) -> int {
        if (argc != 3){
            printf("invalid number of arguments\n");
            return EXIT_FAILURE;
        }
        auto addr = String(argv[1]);
        auto data = String(argv[2]);
        if (!aqquire_stove_lock()) return EXIT_FAILURE;
        stove.write_ram(convert_str_to_hex(addr), convert_str_to_hex(data) );
        release_stove_lock();
        return EXIT_SUCCESS;
    }, "Write to RAM"));
    
    // write-eeprom
    console.registerCommand(ConsoleCommandD("write-eeprom", [](int argc, char **argv) -> int {
        if (argc != 3){
            printf("invalid number of arguments\n");
            return EXIT_FAILURE;
        }
        auto addr = String(argv[1]);
        auto data = String(argv[2]);
        if (!aqquire_stove_lock()) return EXIT_FAILURE;
        stove.write_eeprom(convert_str_to_hex(addr), convert_str_to_hex(data) );
        release_stove_lock();
        return EXIT_SUCCESS;
    }, "Write to EEPROM"));

    // dump all of the ram
    console.registerCommand(ConsoleCommandD("dump-ram", [](int argc, char **argv) -> int {
        if (!aqquire_stove_lock()) return EXIT_FAILURE;
        uint8_t resp;
        stove.dbg_out=false;
        for (int i=0; i<255; i++ ) {
            stove.read_ram(i);
            printf("0x%02x|0x%02x|%d|%c\n",i,stove.last_read_value,stove.last_read_value,stove.last_read_value);
            delay(1);
        }
        release_stove_lock();
        stove.dbg_out=true;
        return EXIT_SUCCESS;
    }, "read all ram addrs"));    
    
    // dump all of the EEPROM
    console.registerCommand(ConsoleCommandD("dump-eeprom", [](int argc, char **argv) -> int {
        uint8_t resp;
        stove.dbg_out=false;
        if (!aqquire_stove_lock()) return EXIT_FAILURE;
        for (int i=0; i<255; i++ ) {
            stove.read_eeprom(i);
            printf("0x%02x|0x%02x|%d|%c\n",i,stove.last_read_value,stove.last_read_value,stove.last_read_value);
            delay(1);
        }
        release_stove_lock();
        stove.dbg_out=true;
        return EXIT_SUCCESS;

        return EXIT_SUCCESS;
    }, "read all eeprom addrs"));


    console.registerCommand(ConsoleCommandD("stove-on", [](int argc, char **argv) -> int {
        if (!aqquire_stove_lock()) return EXIT_FAILURE;
        stove.on();
        release_stove_lock();
        return EXIT_SUCCESS;
    }, "Turn the stove on"));
    
    console.registerCommand(ConsoleCommandD("stove-off", [](int argc, char **argv) -> int {
        if (!aqquire_stove_lock()) return EXIT_FAILURE;
        stove.off();
        release_stove_lock();
        return EXIT_SUCCESS;
    }, "turn the stove off"));



    /* configuration */

    // config wifi-username&password
    console.registerCommand(ConsoleCommandD("wificonfig", [](int argc, char **argv) -> int {
        if (argc != 3){
            if (argc == 1){
                printf("current wifi config:\n ssid: %s\n password: %s\n",
                    settings.getString("wifi-ssid", WIFI_SSID),
                    settings.getString("wifi-pass", WIFI_PASS).c_str()
                );
                return EXIT_SUCCESS;
            }
            printf("usage: wificonfig [ssid] [password]\n");
            return EXIT_FAILURE;
        }
        auto ssid = String(argv[1]);
        auto pass = String(argv[2]);

        settings.putString("wifi-ssid", ssid);
        settings.putString("wifi-pass", pass); 

        return EXIT_SUCCESS;
    }, "Set-up Wifi"));


    // configure IP static/dynamic
    console.registerCommand(ConsoleCommandD("ifconfig", [](int argc, char **argv) -> int {
        if (argc == 1){
            printf("wifi status: ");
            if (WiFi.isConnected()){
                printf("connected\n");
                printf(" RSSI: %d dBm",WiFi.RSSI());
            }
            else {
                printf("disconnected\n");
            }
            uint8_t macAddr[6];
            WiFi.macAddress(macAddr);
            printf(" MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
            if ( !settings.getBool("if-static", IFCONFIG_STATIC) ){
                if (WiFi.isConnected()){
                    printf(" ipaddress: %s\n gateway: %s\n netmask: %s\n dns-server: %s\n",
                        WiFi.localIP().toString().c_str(),
                        WiFi.gatewayIP().toString().c_str(),
                        WiFi.subnetMask().toString().c_str(),
                        WiFi.dnsIP(0).toString().c_str()
                    );
                }
                printf("current config: DHCP\n");
            } else {
                printf("current config: static\n ipaddress: %s\n gateway: %s\n netmask: %s\n dns-server: %s\n",
                    settings.getString("if-ip", IFCONFIG_ADDR),
                    settings.getString("if-gw", IFCONFIG_GATEWAY),
                    settings.getString("if-nm", IFCONFIG_NETMASK),
                    settings.getString("if-dns", IFCONFIG_DNS)
                );
            }
            #if TCP_SERVER_ENABLE == True
                printf("configured TCP port: %i\n", settings.getUInt("tcp-port", TCP_SERVER_PORT));
            #endif
            #if WEB_SERVER_ENABLE == True
                printf("configured web port: %i\n", settings.getUInt("web-port", WEB_SERVER_PORT));
            #endif

            return EXIT_SUCCESS;
        }
        if (argc == 2){
            auto cmd = String(argv[1]);
            if (cmd == "dhcp"){
                settings.putBool("if-static", false);
                return EXIT_SUCCESS;
            }
        }
        if (argc != 6){
            printf("usage: ifconfig static [ipaddr] [gateway] [netmask] [dnsserver]\n");
            return EXIT_FAILURE;
        }
        settings.putBool("if-static", true);
        settings.putString("if-ip", String(argv[2]) );
        settings.putString("if-gw", String(argv[3]) );
        settings.putString("if-nm", String(argv[4]) );
        settings.putString("if-dns", String(argv[5]) );
        
        return EXIT_SUCCESS;

    }, "Set-up dhcp or static IP"));


    console.registerCommand(ConsoleCommandD("default-settings", [](int argc, char **argv) -> int {
        settings.clear();
        return EXIT_SUCCESS;
    }, "delete all settings"));


    #if TCP_SERVER_ENABLE == True

    console.registerCommand(ConsoleCommandD("set-tcp-port", [](int argc, char **argv) -> int {
        if (argc != 2){
            printf("usage: set-tcp-port [PORTNUM]\n");
            return EXIT_FAILURE;
        }
        uint16_t port = '\0';
        sscanf(argv[1], "%u", &port);
        if (port<1){
            printf("err: port needs to be bigger than 0\n");
            return EXIT_FAILURE;
        }
        settings.putUInt("tcp-port", port);
        printf("port configured to: %i. please run 'restart' to apply.\n", port);
        return EXIT_SUCCESS;
    }, "Configure TCP server port"));
    
    #endif




    // wifi commands
    console.registerCommand(ConsoleCommandD("wifi-connect", [](int argc, char **argv) -> int {
        if (WiFi.isConnected()){
            printf("wifi is already connected\n");
            return EXIT_FAILURE;
        }
        wifi_connect();
        return EXIT_SUCCESS;
    }, "connect to wifi"));

    console.registerCommand(ConsoleCommandD("wifi-disconnect", [](int argc, char **argv) -> int {
        if (!WiFi.isConnected()){
            printf("wifi is already disconnected\n");
            return EXIT_FAILURE;
        }
        wifi_disconnect();
        return EXIT_SUCCESS;
    }, "disconnect wifi"));

    console.registerCommand(ConsoleCommandD("wifi-reconnect", [](int argc, char **argv) -> int {
        wifi_reconnect();
        return EXIT_SUCCESS;
    }, "reconnect wifi"));


    // web server stuff
    #if WEB_SERVER_ENABLE == True

    console.registerCommand(ConsoleCommandD("set-web-port", [](int argc, char **argv) -> int {
        if (argc != 2){
            printf("usage: set-web-port [PORTNUM]\n");
            return EXIT_FAILURE;
        }
        uint16_t port = '\0';
        sscanf(argv[1], "%u", &port);
        if (port<1){
            printf("err: port needs to be bigger than 0\n");
            return EXIT_FAILURE;
        }
        settings.putUInt("web-port", port);
        printf("web server port configured to: %i. please run 'restart' to apply.\n", port);
        return EXIT_SUCCESS;
    }, "Configure web server port"));

    #endif


}