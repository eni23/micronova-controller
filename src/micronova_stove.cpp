#include <Arduino.h>


#define SERIAL_MODE SERIAL_8N2


#define STOVE_OFFSET_RAM_READ     0x00
#define STOVE_OFFSET_EEPROM_READ  0x20
#define STOVE_OFFSET_RAM_WRITE    0x80
#define STOVE_OFFSET_EEPROM_WRITE 0xA0

// addresses RAM
#define STOVE_ADDR_STATE          0x21
#define STOVE_ADDR_FUMES_TEMP     0x3E
#define STOVE_ADDR_FLAMES_POWER   0x34




class MicronovaStove {

  private:

    int pin_rx;
    int pin_tx;
    int pin_rx_enable;



  public:


    char stove_rx_data[2];


    MicronovaStove( int rx, int tx, int rx_enable ) {
      pin_rx = rx;
      pin_tx = tx;
      pin_rx_enable = rx_enable;
    }


    void init(){
      Serial2.begin( 1200, SERIAL_MODE, pin_rx, pin_tx );
      pinMode( pin_rx_enable, OUTPUT );
      digitalWrite( pin_rx_enable, HIGH ); 
    }


    void enable_rx(){
      digitalWrite( pin_rx_enable, LOW );
    }


    void disable_rx(){
      digitalWrite( pin_rx_enable, HIGH );
    }


    void read_stove_answer(){
      enable_rx();
      uint8_t rx_count = 0;
      stove_rx_data[0] = 0x00;
      stove_rx_data[1] = 0x00;
      while ( Serial2.available() ) {
        stove_rx_data[rx_count] = Serial2.read();
        rx_count++;
      }
      disable_rx();
    }

    void read_request( uint8_t location, uint8_t addr ){
      Serial2.write(location);
      delay(1);
      Serial2.write(addr);
    }

    uint8_t read_ram( uint8_t addr ){
      read_request( STOVE_OFFSET_RAM_READ, addr );
      read_stove_answer();
      uint8_t val = stove_rx_data[1];
      uint8_t checksum = stove_rx_data[0];
      uint8_t param = checksum - val;
      return val
    }

    void write_data( uint8_t location, uint8_t command, uint8_t data ){
      uint8_t chk = calculate_checksum( location, command, data );
      uint8_t data_to_write[4] = {
        location,
        command,
        data,
        chk
      };
      for ( int i = 0; i < 4; i++ ){
        Serial2.write( data[i] );
        delay(1);
      }
    }

    void write_ram( uint8_t command, uint8_t data ){
      write_data( STOVE_OFFSET_RAM_WRITE, command, data );
    }

    void write_eeprom( uint8_t command, uint8_t data ){
      write_data( STOVE_OFFSET_EEPROM_WRITE, command, data);
    }

    byte calculate_checksum( uint8_t dest, uint8_t addr, uint8_t val ){
      uint8_t checksum = 0;
      checksum = dest+addr+val;
      if ( checksum >= 256 ){
        checksum = checksum - 256;
      }
      return (uint8_t)checksum;
    }


};