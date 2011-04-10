#ifndef __USART_SERIAL_H__
#define __USART_SERIAL_H__

#if defined(__AVR__)

#include "WProgram.h"
#include "HardwareSerial.h"

#define hwDbgInitialize() Serial.begin(DEBUG_BAUD_RATE)

#define hwDbgPrintLine(str) Serial.println(str)

#define hwDbgPrint(str) Serial.print(str)

#endif // __AVR__

#endif // __USART_SERIAL_H__
