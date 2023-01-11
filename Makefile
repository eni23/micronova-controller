.EXPORT_ALL_VARIABLES:
MONITOR_BAUD		= 115200
SERIAL_DEVICE		= /dev/ttyUSB*

.PHONY: all upload clean program uploadfs find-serial find-monitor-baud

all:
	platformio -f -c vim run

upload:
	pio run --target upload

clean:
	platformio -f -c vim run --target clean 

program:
	platformio -f -c vim run --target program 

uploadfs:
	platformio -f -c vim run --target uploadfs

update:
	pio run -t update

inotify-watch-upload:
	-tools/inotify-upload.sh src/

find-serial:
	ls -1 $(SERIAL_DEVICE)
	$(eval USBTTY=$(shell ls -1 $(SERIAL_DEVICE) 2>/dev/null | head -n1 ))

find-monitor-baud:
	$(eval MONITOR_BAUD=$(shell tools/config-value.sh SERIAL_BAUD ))

monitor: find-monitor-baud
	@pio device monitor --baud $(MONITOR_BAUD) --raw --quiet