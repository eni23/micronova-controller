#!/usr/bin/env python3
"""
Use this script to sync time from local computer on stove

This file is an example how to use the python client
"""

import sys
import time
import argparse
import datetime


from micronova_client import MicronovaClient


STOVE_LOCATION_DOW = 0xf8 
STOVE_LOCATION_HOUR = 0xF9 
STOVE_LOCATION_MINUTE = 0xFA
STOVE_LOCATION_DAY = 0xFB 
STOVE_LOCATION_MONTH = 0xFC
STOVE_LOCATION_YEAR = 0xFD



def main():
  parser = argparse.ArgumentParser(
    prog = 'Micronova stove time syncer'
  )
  parser.add_argument('-H', '--host', required=True)
  parser.add_argument('-p', '--port', default=9040, type=int)
  parser.add_argument('-e', '--execute', action="store_true")  
  args = parser.parse_args()
  stove = MicronovaClient(args.host, args.port)

  try:
    stove.connect()
    ping_res = stove.ping()
  except:
    stove.close()
    print("ERR: cannot connect to stove")
    sys.exit(1)
  
  if not ping_res:
    print("ERR: stove does not responds as it should")
    sys.exit(1)

  
  now = datetime.datetime.now()

  # put everything in a array with [ desc, eeprom_location, value ]
  arr = []
  arr.append( [ "dow", STOVE_LOCATION_DOW,
    stove.convert_number_for_clock( 
      datetime.datetime.today().strftime('%w')
    )]
  )
  arr.append( [ "minute", STOVE_LOCATION_MINUTE, stove.convert_number_for_clock( now.minute ) ] )
  arr.append( [ "hour", STOVE_LOCATION_HOUR, stove.convert_number_for_clock( now.hour ) ] )
  arr.append( [ "day", STOVE_LOCATION_DAY, stove.convert_number_for_clock( now.day )] )
  arr.append( [ "month", STOVE_LOCATION_MONTH,  stove.convert_number_for_clock( now.month )] )
  arr.append( [ "year", STOVE_LOCATION_YEAR,  stove.convert_number_for_clock( now.year-2000 )] )

  # loop trough the array
  for desc,location,data in arr:
    print(f"set {desc} to 0x{data:02x} (addr=0x{location:02x})")
    # if -e is given to cmdline, do the change
    if (args.execute):
      stove.write_eeprom(location, data)
      time.sleep(0.15)

  stove.close()
  

if __name__ == "__main__":
  main()
