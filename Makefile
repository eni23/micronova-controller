.EXPORT_ALL_VARIABLES:
MONITOR_BAUD		= 115200
SERIAL_DEVICE		= /dev/ttyUSB*

.PHONY: all inotify-watch-upload monitor-loop

all:
	platformio -f -c vim run

upload: monitor-close
	pio run --target upload --verbose

upload-reopen-monitor: all upload monitor-notify-done

clean:
	platformio -f -c vim run --target clean --verbose

program:
	platformio -f -c vim run --target program --verbose

uploadfs:
	platformio -f -c vim run --target uploadfs --verbose

update: --verbose
	pio run -t update

inotify-watch-upload:
	-tools/inotify-upload.sh lib/TFT_ST7735/_usr/ src/

find-serial:
	ls -1 $(SERIAL_DEVICE)
	$(eval USBTTY=$(shell ls -1 $(SERIAL_DEVICE) 2>/dev/null | head -n1 ))

find-monitor-baud:
	$(eval MONITOR_BAUD=$(shell tools/config-value.sh SERIAL_BAUD ))

monitor: find-monitor-baud find-serial
	tools/monitor.sh open $(USBTTY) $(MONITOR_BAUD)

monitor-close:
	tools/monitor.sh close

monitor-loop: find-monitor-baud find-serial
	-tools/monitor.sh loop $(USBTTY) $(MONITOR_BAUD)

monitor-notify-done:
	tools/monitor.sh notify-done
