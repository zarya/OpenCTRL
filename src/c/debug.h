#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "WProgram.h"
#include "lib/protocol.h"
#include "lib/debug.h"

// global defines
#define DBG_BUFFERSIZE 64

// check for compile option, if set use debug if not don't...
#ifdef SERIAL_DEBUG

#ifdef __cplusplus
extern "C"
{
#endif
     // initialize debug interface
     inline void dbgInitialize()
     {
	  hwDbgInitialize();
     }
    
     // print line to debug
     extern void dbgPrintln(char *_str, ...);
    
     // print string to debug
     extern void dbgPrint(char *_str, ...);
    
     // default is received packet: dbgPacket(&sInput) if you want to print output: dbgPacket(&sOutput, false)
     extern void dbgPacket(SPacket *_packet, uint16 _checksum, bool checksumValid);
    
#ifdef __cplusplus
}
#endif

#else // ! defined SERIAL_DEBUG

// no debug defined, exclude all debug functions
//#define dbgBaudrate(x)
#define dbgInitialize()
#define dbgPrintln(...)
#define dbgPrint(...)
#define dbgPacket(...)

#endif // SERIAL_DEBUG

#endif // __DEBUG_H__
