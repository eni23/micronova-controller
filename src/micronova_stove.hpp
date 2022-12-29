#include <Arduino.h>


#if defined(ESP8266) || defined(USE_SWSERIAL)
  #include <SoftwareSerial.h>
  SoftwareSerial StoveSerial;
  #define SERIAL_MODE SWSERIAL_8N2 
#endif

#if defined(ESP32) || defined(USE_HWSERIAL)
  #include <HardwareSerial.h>
  HardwareSerial StoveSerial(1);
  #define SERIAL_MODE SERIAL_8N2 
#endif


#define STOVE_OFFSET_RAM_READ     0x00
#define STOVE_OFFSET_EEPROM_READ  0x20
#define STOVE_OFFSET_RAM_WRITE    0x80
#define STOVE_OFFSET_EEPROM_WRITE 0xA0

// addresses RAM
#define STOVE_ADDR_STATE          0x21
#define STOVE_ADDR_FUMES_TEMP     0x3E
#define STOVE_ADDR_FLAMES_POWER   0x34

// STATE
#define STOVE_STATE_TURN_ON       0x01
#define STOVE_STATE_TURN_OFF      0x06
#define STOVE_STATE_FORCE_OFF     0x00


//0 - OFF, 1 - Starting, 2 - Pellet loading, 3 - Ignition, 4 - Work, 5 - Brazier cleaning, 6 - Final cleaning, 7 - Standby, 8 - Pellet missing alarm, 9 - Ignition failure alarm, 10 - Alarms (to be investigated)




class MicronovaStove {

  private:

    int pin_rx;
    int pin_tx;
    int pin_rx_enable;



  public:


    bool dbg_out = true;
    char stove_rx_data[2];
    uint8_t last_read_value;
    uint8_t last_read_param;
    uint8_t last_read_checksum;


    MicronovaStove( int rx, int tx, int rx_enable ) {
      pin_rx = rx;
      pin_tx = tx;
      pin_rx_enable = rx_enable;
    }


    void init(){
      StoveSerial.begin( 1200, SERIAL_MODE, pin_rx, pin_tx );
      printf("begin stove serial");
      pinMode( pin_rx_enable, OUTPUT );
      digitalWrite( pin_rx_enable, HIGH ); 
    }



    /* read functions */
    void enable_rx(){
      if (dbg_out) printf("enable rx\n");
      digitalWrite( pin_rx_enable, LOW );
    }


    void disable_rx(){
      if (dbg_out) printf("disable rx\n");
      digitalWrite( pin_rx_enable, HIGH );
    }

    void flushInput(){
      while (Serial.available()){
        Serial.read();
        printf("flush: read 1 extra byte\n");
      }
    }


    void read(uint8_t location, uint8_t addr){

      if (dbg_out){
        printf("read request loc=");
        printf("0x%02x ", location );
        printf(", addr=");
        printf("0x%02x ", addr );
        printf("\n");

      }

      StoveSerial.write(location);
      delay(1);
      StoveSerial.write(addr);
      // flush in arduino only waits until the output buffer is written
      StoveSerial.flush();


      if (dbg_out) printf("read stove answer\n");

      //return;
      delay(20);
      enable_rx();
      
      uint8_t rx_count = 0;
      stove_rx_data[0] = 0x00;
      stove_rx_data[1] = 0x00;
      // we have to flush it first to make sure no crap data is on the bus
      StoveSerial.flush();
      while ( StoveSerial.available() ) {
        if (rx_count>1){
          while ( StoveSerial.available() ) {
            printf("read 1 extra byte\n");
          }
          break;
        }
        stove_rx_data[rx_count] = StoveSerial.read();
        rx_count++;
      }
      last_read_value = stove_rx_data[1];
      last_read_checksum = stove_rx_data[0];
      last_read_param = last_read_checksum - last_read_value;
      // TODO: check for error with checksum
      disable_rx();
    }

    uint8_t read_ram( uint8_t addr ){
      read( STOVE_OFFSET_RAM_READ, addr );
      return last_read_value;
    }

    uint8_t read_eeprom( uint8_t addr ){
      read( STOVE_OFFSET_EEPROM_READ, addr );
      return last_read_value;
    }


    /* write functions */

    void write( uint8_t location, uint8_t command, uint8_t data ){
      uint8_t chk = calculate_checksum( location, command, data );
      uint8_t data_to_write[4] = {
        location,
        command,
        data,
        chk
      };
      printf("write data=");
      for ( int i = 0; i < 4; i++ ){
        printf("0x%02x ", data_to_write[i] );
        StoveSerial.write( data_to_write[i] );
        delay(1);
      }

      printf("\n");
      // TODO: Read/verify answer
      flushInput();

    }

    void write_ram( uint8_t command, uint8_t data ){
      write( STOVE_OFFSET_RAM_WRITE, command, data );
    }

    void write_eeprom( uint8_t command, uint8_t data ){
      write( STOVE_OFFSET_EEPROM_WRITE, command, data);
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