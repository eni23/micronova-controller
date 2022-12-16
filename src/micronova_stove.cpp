#include <Arduino.h>
#include <SoftwareSerial.h>


#define SERIAL_MODE SWSERIAL_8N2


#define STOVE_OFFSET_RAM 0x00
#define STOVE_OFFSET_EEPROM 0x20
#define STOVE_OFFSET_RAM_WRITE 0x80
#define STOVE_OFFSET_EEPROM_WRITE 0xA0

// addresses RAM
#define STOVE_ADDR_STATE 0x21
#define STOVE_ADDR_FUMES_TEMP 0x3E
#define STOVE_ADDR_FLAMES_POWER 0x34




class MicronovaStove {

  private:

    int pin_rx;
    int pin_tx;
    int pin_rx_enable;



  public:


    SoftwareSerial StoveSerial;
    char stove_rx_data[2];


    MicronovaStove(int rx, int tx, int rx_enable) {
      pin_rx = rx;
      pin_tx = tx;
      pin_rx_enable = rx_enable;
    }


    void init(){
      StoveSerial.begin(1200, SERIAL_MODE, pin_rx, pin_tx, false, 256);
      pinMode(pin_rx_enable, OUTPUT);
      digitalWrite(pin_rx_enable, HIGH); 
    }


    void enable_rx(){
      digitalWrite(pin_rx_enable, LOW);
    }


    void disable_rx(){
      digitalWrite(pin_rx_enable, HIGH);
    }


    void read_stove_answer(){
      enable_rx();
      uint8_t rx_count = 0;
      stove_rx_data[0] = 0x00;
      stove_rx_data[1] = 0x00;
      while (StoveSerial.available()) {
        stoveRxData[rxCount] = StoveSerial.read();
        rx_count++;
      }
      disable_rx();
    }

    byte calculate_checksum(uint8_t dest, uint8_t addr, uint8_t val){
      uint8_t checksum = 0;
      checksum = dest+addr+val;
      if (checksum>=256){
        checksum=checksum-256;
      } 
      return (uint8_t)checksum;
    }


};