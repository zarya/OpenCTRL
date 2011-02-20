ARDUINO_DIR=/usr/share/arduino
MCU=atmega328p
TARGET=OpenCRTLClient
ARDUINO_LIBS=NewSoftSerial
F_CPU=20000000
ARDUINO_PORT=/dev/ttyACM0

include Arduino.mk
