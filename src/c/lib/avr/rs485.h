#ifndef __RS485_HW_H__
#define __RS485_HW_H__

#if defined(__AVR__)

#include "WProgram.h"
#include <NewSoftSerial.h>
#include "HardwareSerial.h"

#if defined(__AVR__)
#if defined(__AVR_ATmega2560__)
#define serBus Serial1
#else
NewSoftSerial serBus(3, 4);
#endif
#endif

#define octrlBusInitialize(baud) serBus.begin(baud)

#define octrlBusAvailable() serBus.available()

#define octrlBusWrite(byte) serBus.print(byte, BYTE)

#define octrlBusRead() serBus.read()

#endif // __AVR__

#endif // __RS485_HW_H__



