#!/usr/bin/make -f
BOARD_TAG     = uno

MONITOR_PORT  = /dev/ttyACM0
include /usr/share/arduino/Arduino.mk

# Mac OS X settings
#MONITOR_PORT  = /dev/tty.usbmodem1411
#include /Users/idubrov/Projects/Arduino-Makefile/Arduino.mk

