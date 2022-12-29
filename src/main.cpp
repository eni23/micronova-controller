#include "Arduino.h"
#include "config.h"

#include "ESP32Console.h"
using namespace ESP32Console;
Console console;



#include "micronova_stove.hpp"


#define STOVE_PIN_RX        27
#define STOVE_PIN_TX        26
#define STOVE_PIN_EN_RX     25


MicronovaStove stove(
    STOVE_PIN_RX,
    STOVE_PIN_TX,
    STOVE_PIN_EN_RX
);




uint8_t convert_str_to_hex(String s){
    uint8_t ch = '\0';
    sscanf(s.c_str(), "%x", &ch);
    return ch;
}



uint8_t stove_answer[2];


void setup() {

    console.setPrompt("# ");
    console.registerSystemCommands();
    console.registerGPIOCommands();

    console.begin(SERIAL_BAUD);
    stove.init();



    // read-ram
    console.registerCommand(ConsoleCommandD("read-ram", [](int argc, char **argv) -> int {
        if (argc != 2){
            printf("invalid number of arguments\n");
            return EXIT_FAILURE;
        }
        auto arg = String(argv[1]);
        
        stove.read_ram(convert_str_to_hex(arg));
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
        stove.read_eeprom(convert_str_to_hex(arg));
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
        stove.write_ram(convert_str_to_hex(addr), convert_str_to_hex(data) );
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
        stove.write_eeprom(convert_str_to_hex(addr), convert_str_to_hex(data) );
        return EXIT_SUCCESS;
    }, "Write to EEPROM"));

    // console.registerCommand(ConsoleCommandD("kill-loop", [](int argc, char **argv) -> int {
    //     for (;;) {
    //         stove.write_ram(STOVE_ADDR_STATE, STOVE_STATE_TURN_ON );
    //         stove.read_ram(STOVE_ADDR_STATE );
    //         delay(100);
    //     }
    // }, "kill it"));

    console.registerCommand(ConsoleCommandD("dump-ram", [](int argc, char **argv) -> int {
        uint8_t resp;
        stove.dbg_out=false;
        for (int i=0; i<255; i++ ) {
            stove.read_ram(i);
            printf("0x%02x|0x%02x|%d|%c\n",i,stove.last_read_value,stove.last_read_value,stove.last_read_value);
            delay(1);
        }
        stove.dbg_out=true;
        return EXIT_SUCCESS;
    }, "read all ram addrs"));    
    
    console.registerCommand(ConsoleCommandD("dump-eeprom", [](int argc, char **argv) -> int {
        uint8_t resp;
        stove.dbg_out=false;
        for (int i=0; i<255; i++ ) {
            stove.read_eeprom(i);
            printf("0x%02x|0x%02x|%d|%c\n",i,stove.last_read_value,stove.last_read_value,stove.last_read_value);
            delay(1);
        }
        stove.dbg_out=true;
        return EXIT_SUCCESS;

        return EXIT_SUCCESS;
    }, "read all eeprom addrs"));


    console.registerCommand(ConsoleCommandD("read-loc", [](int argc, char **argv) -> int {
        
        if (argc != 3){
            printf("invalid number of arguments\n");
            return EXIT_FAILURE;
        }
        auto loc = String(argv[1]);
        auto addr = String(argv[2]);

        stove.read(convert_str_to_hex(loc), convert_str_to_hex(addr) );
        printf("v=0x%02x p=0x%02x\n",
            stove.last_read_value,
            stove.last_read_param
        );

        return EXIT_SUCCESS;
    }, "read all eeprom addrs"));

    console.registerCommand(ConsoleCommandD("enable-rx", [](int argc, char **argv) -> int {
        stove.enable_rx();
        return EXIT_SUCCESS;
    }, "."));
    
    console.registerCommand(ConsoleCommandD("disable-rx", [](int argc, char **argv) -> int {
        stove.disable_rx();
        return EXIT_SUCCESS;
    }, "."));

}

void loop() {

}
