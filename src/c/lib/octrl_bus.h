#ifndef __OCTRL_BUS_H__
#define __OCTRL_BUS_H__

#if defined(__AVR__)
#include "lib/avr/rs485.h"
#elif defined(__GNUC__) && ! defined(__AVR__)
#include "lib/generic/rs232.h"
#endif

#endif // __OCTRL_BUS_H__
