#include "Arduino.h"
#include "micronova_stove.h"

#if defined(ESP8266) || defined(USE_SWSERIAL)
  #include <SoftwareSerial.h>
  SoftwareSerial StoveSerial;
#endif

#if defined(ESP32) || defined(USE_HWSERIAL)
  #include <HardwareSerial.h>
  HardwareSerial StoveSerial(1);
#endif


MicronovaStove::MicronovaStove( int rx, int tx, int rx_enable ) {
  pin_rx = rx;
  pin_tx = tx;
  pin_rx_enable = rx_enable;
}


void MicronovaStove::init(){
  StoveSerial.begin( 1200, SERIAL_MODE, pin_rx, pin_tx, false, 256 );
  if (dbg_out) printf("begin stove serial");
  pinMode( pin_rx_enable, OUTPUT );
  digitalWrite( pin_rx_enable, HIGH ); 
}


/* read functions */

void MicronovaStove::enable_rx(){
  if (dbg_out) printf("enable rx\n");
  digitalWrite( pin_rx_enable, LOW );
}


void MicronovaStove::disable_rx(){
  if (dbg_out) printf("disable rx\n");
  digitalWrite( pin_rx_enable, HIGH );
}


void MicronovaStove::flushInput(){
  if (dbg_out) printf("flush serial input\n");
    while (Serial.available()){
      Serial.read();
      if (dbg_out) printf("flush: read 1 extra byte\n");
    }
}



void MicronovaStove::read(uint8_t location, uint8_t addr){
      
  dbg_out=false;

  if (dbg_out){
    printf("read request loc=");
    printf("0x%02x ", location );
    printf(", addr=");
    printf("0x%02x ", addr );
    printf("\n");

  }

      
  StoveSerial.write(location);
  StoveSerial.flush();
  StoveSerial.write(addr);
  StoveSerial.flush();

  enable_rx();
  // the stove needs some time to answer, 120ms seems about the right amount, 110 is too short
  delay(120);

  if (dbg_out) printf("read stove answer\n");

  uint8_t rx_count = 0;
  stove_rx_data[0] = 0x00;
  stove_rx_data[1] = 0x00;
      
  while ( StoveSerial.available() ) {
    if (rx_count>1){
      // TODO: find a better way to do this
      rx_count = 0;
    }
    stove_rx_data[rx_count] = StoveSerial.read();
    if (dbg_out) printf("i=0x%02x data=0x%02x\n",rx_count,stove_rx_data[rx_count]);
    rx_count++;
  }

      
  disable_rx();
  last_read_value = stove_rx_data[1];
  last_read_checksum = stove_rx_data[0];
  last_read_param = last_read_checksum - last_read_value;

  if (last_read_param != addr ){
    if (dbg_out) printf("E: return mismatch\n");
  }
  // TODO: check for error with checksum
}


uint8_t MicronovaStove::read_ram( uint8_t addr ){
  read( STOVE_OFFSET_RAM_READ, addr );
  return last_read_value;
}

uint8_t MicronovaStove::read_eeprom( uint8_t addr ){
  read( STOVE_OFFSET_EEPROM_READ, addr );
  return last_read_value;
}



/* write functions */

void MicronovaStove::write( uint8_t location, uint8_t command, uint8_t data ){
  uint8_t chk = calculate_checksum( location, command, data );
  uint8_t data_to_write[4] = {
    location,
    command,
    data,
    chk
  };
  
  if (dbg_out) printf("write data=");
    
  for ( int i = 0; i < 4; i++ ){
    if (dbg_out) printf("0x%02x ", data_to_write[i] );
    StoveSerial.write( data_to_write[i] );
    delay(1);
  }
    
  if (dbg_out) printf("\n");

}


void MicronovaStove::read_answer(){
  enable_rx();
  delay(120);
  uint8_t rx_count = 0;
  stove_rx_data[0] = 0x00;
  stove_rx_data[1] = 0x00;
      
  while ( StoveSerial.available() ) {
    if (rx_count>1){
      // TODO: find a better way to do this
      rx_count = 0;
    }
    stove_rx_data[rx_count] = StoveSerial.read();
    rx_count++;
  }
  disable_rx();
  last_read_value = stove_rx_data[1];
  last_read_checksum = stove_rx_data[0];
  
}


void MicronovaStove::write_ram( uint8_t command, uint8_t data ){
  write( STOVE_OFFSET_RAM_WRITE, command, data );
}

void MicronovaStove::write_eeprom( uint8_t command, uint8_t data ){
  write( STOVE_OFFSET_EEPROM_WRITE, command, data);
}


byte MicronovaStove::calculate_checksum( uint8_t dest, uint8_t addr, uint8_t val ){
  uint8_t checksum = 0;
  checksum = dest+addr+val;
  if ( checksum >= 256 ){
    checksum = checksum - 256;
  }
  return (uint8_t)checksum;
}




/* Abstraction helper functions */
uint8_t MicronovaStove::get_state(){
  read_ram(STOVE_ADDR_STATE);
  return last_read_value;
}

void MicronovaStove::on(){
  write_ram(STOVE_ADDR_STATE, STOVE_STATE_TURN_ON);
}


void MicronovaStove::off(){
  simulate_infrared(STOVE_IR_POWER, 8);
}


float MicronovaStove::get_ambient_temp(){
  read_ram(STOVE_ADDR_AMBIENT_TEMP);
  return (float) last_read_value / 2;
}


uint8_t MicronovaStove::get_fumes_temp(){
  read_ram(STOVE_ADDR_FUMES_TEMP);
  return last_read_value;
}


uint8_t MicronovaStove::get_power(){
  read_ram(STOVE_ADDR_POWER_EEPROM);
  return last_read_value;
}


uint16_t MicronovaStove::get_fumes_fan_speed(){
  read_ram(STOVE_ADDR_FUMES_SPEED);
  return (uint16_t) last_read_value*10;
}


void MicronovaStove::set_power(uint8_t power_level){
  if (power_level > 4){
    power_level = 4;
  }
  // TODO: check if we really need to do this twice
  write_eeprom(STOVE_ADDR_POWER_EEPROM, power_level);
  write_ram(STOVE_ADDR_POWER_RAM, power_level);
}


void MicronovaStove::set_thermostat(uint8_t temperature){
  if (temperature > 32){
    temperature = 32;
  }
  write_eeprom(STOVE_ADDR_THERMOSTAT, temperature);
}

void MicronovaStove::simulate_infrared(uint8_t data, uint8_t repetitions){
  for (uint8_t i=0; i<repetitions; i++){
    write_ram(STOVE_ADDR_IRCOMMAND, data);
    delay(100);
  }
}
