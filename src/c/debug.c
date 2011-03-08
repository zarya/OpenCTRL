#ifdef SERIAL_DEBUG

//#ifdef __AVR__
#include "WProgram.h"
//#endif

#include "debug.h"

#include "lib/debug.h"

#include <string.h>
#include <stdarg.h>

static char writeBuffer[DBG_BUFFERSIZE]; // debug write buffer

void dbgInitialize()
{
     debugInitialize();
}

void dbgPrintln(char *str, ...)
{
     va_list va;
     va_start(va, str);
     vsnprintf(writeBuffer, DBG_BUFFERSIZE, str, va);

     debugPrintLine(writeBuffer);
}

void dbgPrint(char *str, ...)
{
     va_list va;
     va_start(va, str);
     vsnprintf(writeBuffer, DBG_BUFFERSIZE, str, va);
     
     debugPrint(writeBuffer);
}

// default is received packet: dbgPacket(&sInput) if you want to print output: dbgPacket(&sOutput, false)
void dbgPacket(SPacket *packet, uint16 _checksum)
{
     dbgPrintln("%s (%d.%d) -> (%d.%d)", 
		_checksum == 0 ? "INPUT" : "OUTPUT",
		packet->header.m_nSourceNetwork,
		packet->header.m_nSourceDeviceID,
		packet->header.m_nDestinationNetwork,
		packet->header.m_nDestinationDevice );
     dbgPrintln("Packet ID: %d", packet->header.m_nPacketID);
     dbgPrintln("Checksum: (%d) %s", (_checksum > 0 ? _checksum : nChecksum), (_checksum ? "" : (isChecksumValid() ? "valid" : "INVALID")));
     
     if (packet->header.m_nPacketLength > SER_MAX_DATA_LENGTH)
	  dbgPrintln("Protocol code: %d", packet->header.m_nPacketLength);
     else
	  dbgPrintln("Data length: %d", packet->header.m_nPacketLength);

     register char data = 0;
     for (; data < (packet->header.m_nPacketLength > SER_MAX_DATA_LENGTH ? 0 : packet->header.m_nPacketLength); data++)
	  dbgPrint("%d ", packet->data[data]);

     dbgPrintln("---------------------------------- \n");
}

#define dbgBaudrate Serial.begin
//     dbgBaudrate(57600);

#else
#error "No AVR Toolchain found"
#endif
