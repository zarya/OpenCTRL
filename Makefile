# The arduino directories
ARDUINO_DIR=/usr/share/arduino
ARDUINO_LIB_PATH  = $(ARDUINO_DIR)/libraries
ARDUINO_CORE_PATH = $(ARDUINO_DIR)/hardware/arduino/cores/arduino

# The chip type:
# atmega328p > Uno
# atmega2560 > Mega
# atmega168 > Pro
MCU=atmega328p

# Project name
TARGET=OpenCRTLClient

# Used libraries. BUG: Now only one is allowed (more will make compile errors)
ARDUINO_LIBS=NewSoftSerial

# Arduino CPU Speed
F_CPU=20000000

# Port where we can find the arduino (Normaly ttyACM* or ttyUSB*)
ARDUINO_PORT=/dev/ttyACM0

# The actual make file
include Arduino.mk
