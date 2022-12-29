## my stove confirmed locations 

| Type | Loc. | DESCRIPTION |
|------|------|-------------|
| RAM | 0x00 | internal timer of the program (ranging from 0-255 continuously) | 
| RAM | 0x01 | temperature multiplied by 2 | 
| RAM | 0x21 | power state (0-stove off, 1-stove start) | 
| RAM | 0x19 | power setting (0-4, only read works) | 
| EEPROM | 0x7d | set temperature as 1:1 value | 
| EEPROM | 0x7f | power, 1-4 | 
| EEPROM | 0x25 | night mode, 0=off, 1=on |
| RAM | 0x3E | Fumes temperature | 
| RAM | 0x02 | temp something, increasing |
| RAM | 0x37 | fumes fan |
| RAM | 0x0D | pellet loading in use?  | 
| RAM | 0x7D | temp, increasing, intake?, gets cold quicker than 0x02 | 

power set: write both to EEPROM:0x7f and RAM:0x19


0x21:
0=off
1=control
2=phase1
3=phase2
4=running
6=turning off 
7=cooling


## Locations 
(according to 3ka)

| Type | Loc. | DESCRIPTION |
|------|------|-------------|
| RAM | 0x00 | internal timer of the program (ranging from 0-255 continuously) | 
| RAM | 0x01 | temperature multiplied by 2 | 
| RAM | 0x0D | pellet loading in use (formula !!) | 
| RAM | 0x21 | power state (0-stove off, 1-stove start) | 
| RAM | 0x37 | speed 'fan fumes using (formula !!) | 
| RAM | 0x5A | smoke temperature | 
| RAM | 0x63 | seconds current | 
| RAM | 0x64 | DOW (day of week) | 
| RAM | 0x65 | current time | 
| RAM | 0x66 | minutes current | 
| RAM | 0x67 | date current | 
| RAM | 0x68 | current month | 
| RAM | 0x69 | current year | 
| RAM | 0x73 | intake air temperature | 
| RAM | 0x7D | copy of the set temperature from EEPROM | 
| EEPROM | 0x08 | FAN SMOKE IN POWER 5 | 
| EEPROM | 0x7D | set temperature | 
| RAM |0x8D-0x9C | Display message in Ascii|


## Status locations
(according to italian forum)


| Type | Loc. | DESCRIPTION | ADD | RATIO |
|------|------|-------------|-----|-------|
| RAM | 01 | Room temperature | 0 | 2 | 
| EPR | 125 | Set temperature | 0 | 1| 
| RAM | 115 | Suction air temperature | 0 | 2 | 
| RAM | 90 | Smoke temperature | 0 | 1 | 
| RAM | 13 | Auger revolutions | 0 | 25 | 
| RAM | 55 | Smoke fan revolutions | 250 | 10 | 
| EPR | 127 | Set power | 0 | 1 | 
| RAM | 101 | Time current | 0 | 1 | 
| RAM | 102 | Current minutes | 0 | 1 | 
| RAM | 100 | Day of the week | 0 | 1 | 
| RAM | 103 | Date | 0 | 1 | 
| RAM | 104 | Month | 0 | 1 | 
| RAM | 105 | Year | 0 | 1 | 
| RAM | 33 | Operating status | 0 | 1 | 



## EPROM LOCATIONS

(according to italian forum)

| Loc. | DESCRIPTION | ADD | RATIO |
|------|-------------|-----|-------|
| 00 | Cleaning interval | 0 | 1 |
| 01 | Ignition auger | 0 | 25 |
| 02 | Starter auger | 0 | 25 |
| 03 | P1 | auger | 0 | 25 |
| 04 | P5 | auger | 0 | 25 |
| 05 | Ignition fumes | 250 | 10 |
| 06 | Starter fumes | 250 | 10 |
| 07 | P1 fumes | 250 | 10 |
| 08 | P5 fumes | 250 | 10 |
| 09 | P1 air | 60 | 5 |
| 10 | P5 air | 60 | 5 |
| 11 | Shut-off air | 60 | 5 |
| 12 | Air threshold | 30 | 1 |
| 13 | Comp. | 30 | 1 |
| 14 | Delta T off | 0 | 1 |
| 15 | Delta T on | 0 | 1 |
| 17 | Start time | 0 | 1 |
| 18 | Ignition time | 0 | 1 |
| 19 | Start time | 0 | 1 |
| 20 | Ignition precharge | 0 | 1 |
| 21 | Flue gas module | 30 | 1 |
| 22 | Tank module | 30 | 1 |
| 23 | Threshold 1 | 30 | 1 |
| 24 | Threshold 2 | 30 | 1 |
| 25 | Threshold on/off | 30 | 1 |
| 26 | Power change | 0 | 1 |
| 27 | P1/P2 delay | 0 | 1 |
| 28 | Cleaning duration | 0 | 1 |
| 29 | Cleaning fumes | 250 | 10 |
| 30 | Cleaning auger | 0 | 25 |
| 31 | Cleaning excluded | 0 | 1 |
| 32 | Starting fumes | 250 | 10 |  |
| 33 | Extinguishing fumes | 250 | 10 |
| 34 | Alarm fumes | 250 | 10 |
| 35 | Alarm air | 60 | 5 |
| 36 | Waiting flame | 0 | 1 |
| 37 | Waiting flame suction | 0 | 1 |  |
| 54 | Auger var | -45 | 5 |
| 55 | Suction var | -45 | 5 |
| 56 | Flow var | -45 | 5 |
| 76 | Chrono enabled | 0 | 1 |
| 77 | Chrono1  on  [minutes] | 0 | 10 |
| 78 | Chrono1 off [minutes] | 0 | 10 |
| 79 | Chrono1 Monday | 0 | 1 |
| 80 | Chrono1 Tuesday | 0 | 1 |
| 81 | Chrono1 Wednesday | 0 | 1 |
| 82 | Chrono1 Thursday | 0 | 1 |
| 83 | Chrono1 Friday | 0 | 1 |
| 84 | Chrono1 Saturday | 0 | 1 |
| 85 | Chrono1 Sunday | 0 | 1 |
| 87 | Chrono2 on [minutes] | 0 | 10 |
| 88 | Chrono2 off [minutes] | 0 | 10 |
| 89 | Chrono2 Monday | 0 | 1 |
| 90 | Chrono2 Tuesday | 0 | 1 |
| 91 | Chrono2 Wednesday | 0 | 1 |
| 92 | Chrono2 Thursday | 0 | 1 |
| 93 | Chrono2 Friday | 0 | 1 |
| 94 | Chrono2 Saturday | 0 | 1 |
| 95 | Chrono2 Sunday | 0 | 1 |
| 97 | Chrono3 on  [minutes] | 0 | 10 |
| 98 | Chrono3 off [minutes] | 0 | 10 |
| 99 | Chrono3 Monday | 0 | 1 |
| 100 | Chrono3 Tuesday | 0 | 1 |
| 101 | Chrono3 Wednesday | 0 | 1 |
| 102 | Chrono3 Thursday | 0 | 1 |
| 103 | Chrono3 Friday | 0 | 1 |
| 104 | Chrono3 Saturday | 0 | 1 |
| 105 | Chrono3 Sunday | 0 | 1 |
| 107 | Chrono4 on [minutes] | 0 | 10 |
| 108 | Chrono4 off [minutes] | 0 | 10 |
| 109 | Chrono4 Monday | 0 | 1 |
| 110 | Chrono4 Tuesday | 0 | 1 |
| 111 | Chrono4 Wednesday | 0 | 1 |
| 112 | Chrono4 Thursday | 0 | 1 |
| 113 | Chrono4 Friday | 0 | 1 |
| 114 | Chrono4 Saturday | 0 | 1 |
| 115 | Chrono4 Sunday | 0 | 1