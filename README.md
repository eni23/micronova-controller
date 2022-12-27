# Micronova Controller

## Stove


### Protocol

Half-Duplex Serial, 1200 baud, 2 stop bits

#### Reading:
```
[LOCATION] [ADDRESS]
```

Where `LOCATION` is one of:

* `0x00` RAM
* `0x20` EEPROM

The stove answers with 2 bytes, with the following

```
[Param+Checksum] [Data]

data = stove_rx_data[1];
checksum = stove_rx_data[0];
location = checksum - data;
```
#### Writing

```
[LOCATION] [ADDRESS] [DATA] [CHECKSUM]
```
Where `LOCATION` is one of:

* `0x80` RAM
* `0xA0` EEPROM

The checksum can be calculated as follows:

```
checksum = LOCATION + ADDRESS + DATA

if ( checksum >= 256 ){
  checksum = checksum - 256;
}
```

### Interface Circuit

#### BOM

* 2x Resistor 150R
* 1x Resistor 330R
* 3x Optocoupler PC817 or EL817
* 1x Step Down Module (20V input, 5v output)

#### Circuit

![circuit](doc/schematic-ifboard.png)

#### Connector on stove

![connector](doc/connector.png)



## Links that helped me

- https://k3a.me/ir-controller-for-pellet-stove-with-micronova-controller-stufe-e-pellet-aria-ir-telecomando/
- https://ridiculouslab.altervista.org/en/micronova-protocol-converter/
- https://stufapellet.forumcommunity.net/?t=57458738
- https://github.com/philibertc/micronova_controller