#ifndef __DEBUG_USART_H__

#if defined(__AVR__)
#include "lib/avr/usart_serial.h"
#elif defined(__GNUC__) && ! defined(__AVR__)
#include "lib/generic/stdout.h"
#endif

#endif __DEBUG_USART_H__

extern 
