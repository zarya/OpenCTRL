#include "WProgram.h"
#include <NewSoftSerial.h>

#include "lib/bytes.h"
#include "lib/protocol.h"
#include "lib/octrl_bus.h"

#include "OpenCTRL.h"
#include "OpenCTRLHandler.h"
#include "debug.h"

#define isChecksumValid() (*((uint16*)ptrChecksumStart) == nChecksum)
#define getChecksum() *((uint16*)ptrChecksumStart)

// device context / global settings
SDeviceContext sDeviceContext = {
     sDeviceContext.m_bBusBusy = false,
     sDeviceContext.m_nBusID = isMaster() ? DEVICE_ID : 0,
     sDeviceContext.m_nDeviceID = DEVICE_ID,
     sDeviceContext.m_nMasterID = isMaster() ? DEVICE_ID : 0,
     sDeviceContext.m_nLastPacketID = 0
};

// serial input variables (buffers and indexes)
SPacket sInput;
uint8 *ptrInputBuffer = NULL;
uint8 *ptrInputFinished = NULL;
uint8 *ptrInputEOB = NULL;
uint8 *ptrChecksumStart = NULL;
uint16 nChecksum = 0;
uint8 nLastChar = 0;
bool bInvalidPacket = false;

// serial output buffers
SPacket sOutput;

int nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;

packetHandler octrlInputPacketHandler;

void octrlInitInterface(void)
{
     octrlBusInitialize(9600);
     ptrInputBuffer = (uint8 *)&sInput;
     ptrChecksumStart = &sInput.data[SER_MAX_DATA_LENGTH - 1];
}

void octrlInitProtocol(uint8 _devId, bool _master, packetHandler funPtr)
{
     octrlSetDeviceID(_devId);
     
     if (_master)
	  octrlSetBusID(_devId);
     else
	  octrlSetBusID(0);

     ptrInputEOB = &sInput.data[SER_MAX_DATA_LENGTH];

// TODO only when device is not set master!!! If is set master start initial ping to all nods... 
// BUG How does the device know when it's newly added to the BUS or it sufferd from power loss, not all MCU's have Brown Out Register
#ifdef MAX485_PIN
     pinMode(MAX485_PIN, OUTPUT);
#endif

     if (_master) // TODO check master pin
	  return; // scanNetwork()
     else
	  octrlSendProtocolPacket(&sInput, &sOutput, &sDeviceContext, OCTRL_HELLO); // octrlSendProtoPacket()

     octrlInputPacketHandler = funPtr;
}

void octrlReadData(void)
{
     while (octrlBusAvailable())
     {
	  nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;
	  sDeviceContext.m_bBusBusy = true;
	  
	  // bits... go parse!
	  *ptrInputBuffer++ = nLastChar = (char)octrlBusRead();

	  dbgPrintln("Read char: %u - %c", nLastChar, nLastChar);

	  if (ptrInputBuffer < ptrChecksumStart) // only add non checksum to checksum ;)
	       nChecksum += nLastChar;
	  
	  if (ptrInputBuffer == &sInput.data[0]) // input buffer is now starting to write data so we have the header
	  {
	       // we have a header! Awesome! Calculate length and only parse it when whole packet is in!
	       // got whole packet when memory address == data start + packetlength + checksum (2 bytes)
	       // calculate end address of packet
	       ptrChecksumStart = &sInput.data[0] + (sInput.header.m_nPacketLength <= SER_MAX_DATA_LENGTH ? sInput.header.m_nPacketLength : 0);
	       ptrInputFinished = ptrChecksumStart + CHECKSUM_SIZE;
	  }
	  else if (ptrInputBuffer == ptrInputFinished) // we got the whole packet let's parse
	  {
	       dbgPrintln("Checksum should be: %d", getChecksum());
	       dbgPacket(&sInput, nChecksum, isChecksumValid());

	       if (!bInvalidPacket)
	       {
		    // we got the whole packet! Check if we have anything to do with it...
		    if (
			 (sInput.header.m_nDestinationBusID == sDeviceContext.m_nBusID && sInput.header.m_nDestinationDeviceID == sDeviceContext.m_nDeviceID) // directed at us
			 || (sInput.header.m_nDestinationBusID == sDeviceContext.m_nBusID && sInput.header.m_nDestinationDeviceID == 0 && sInput.header.m_nSourceDeviceID == sDeviceContext.m_nMasterID) // broadcast msg from master
			 ) // end of if
		    {
			 // packet is directed at us \o/
			 if (isChecksumValid())
			 {
			      // if good parse rest of the packet
			      if (sInput.header.m_nPacketLength > SER_MAX_DATA_LENGTH)
			      {
				   octrlHandleProtocolPacket(&sInput, &sOutput, &sDeviceContext);
				   octrlRecFinished(); // data handled clear buffers
			      }
			      else
			      {
				   // protocol packet send to callback
				   octrlInputPacketHandler(&sOutput);
			      }
			 }
			 else
			      dbgPrintln("Checksum didn't match!");
		    }
		    else
		    {
			 // we got nothing to do with it reset buffer for new packet
			 octrlRecFinished();
			 dbgPrintln("Packet not for us, ignoring...");
		    }
	       }
	       else
	       {
		    // invalid packet reset buffers
		    octrlRecFinished();
		    dbgPrintln("Invalid packet! To bad!");
	       }
	  }
	  else if (ptrInputBuffer == ptrInputEOB)
	  {
	       // packet longer then RFC mark invalid and reset pointer to prefent buffer overflow ;)
	       ptrInputBuffer = &sInput.data[0]; // overwrite previous data invalid anywayzz
	       bInvalidPacket = true;
	       dbgPrintln("WARNING!! Input buffer overflow!");
	  }

	  // just another char... we don't care ^.^
     }
     
     if (sDeviceContext.m_bBusBusy) // bus is busy but we didn't recieve any data 
     {
	  octrlTimeoutProtection();
     }
}

//int sendData(char *header, char *sendData, char nDataLength)
int octrlSendData(void)
{
     if (!sDeviceContext.m_bBusBusy && sDeviceContext.m_bOutputReady)
     {
	  dbgPrintln("Trying to send data!");

	  sOutput.header.m_nSourceDeviceID = octrlGetDeviceID();
	  sOutput.header.m_nSourceBusID = octrlGetBusID();

	  // if bus full don't send yet and just return and keep buffers
	  // else we can send the data
	  uint8 *ptrOutputBuffer = (uint8 *)&sOutput; // output buffer start
	  uint8 *ptrOutputFinished = (ptrOutputBuffer + sizeof(SSerialHeader) + (sOutput.header.m_nPacketLength > SER_MAX_DATA_LENGTH ? 0 : sOutput.header.m_nPacketLength));
	  uint16 *ptrChecksum = (uint16 *)ptrOutputFinished;
	  uint16 *ptrChecksumFinished = (uint16 *)(ptrOutputFinished + 2); // got 16 bit checksum...
	  *ptrChecksum = 0; // initialize @ 0

#ifdef MAX485_PIN
	  digitalWrite(MAX485_PIN, HIGH);
#endif

	  // barf whole buffer to bus...
	  while (ptrOutputBuffer < ptrOutputFinished)
	  {
	       *ptrChecksum += *ptrOutputBuffer;
	       octrlBusWrite(*ptrOutputBuffer++);
	  }
	  
	  // write checksum to bus
#if BIG_ENDIAN
	  ptrOutputBuffer = (uint8 *)ptrChecksumFinished;
	  while (ptrOutputBuffer > (uint8 *)ptrChecksum)
	  {
	       octrlBusWrite(*ptrOutputBuffer--);
	  }
#else
	  while (ptrOutputBuffer < (uint8 *)ptrChecksumFinished)
	  {
	       octrlBusWrite(*ptrOutputBuffer++);
	  }
#endif

	  dbgPacket(&sOutput, *ptrChecksum, isChecksumValid());

#ifdef MAX485_PIN
	  delay(100);
	  digitalWrite(MAX485_PIN, LOW);
#endif

	  if (! sDeviceContext.m_bWaitForResponse)
	       octrlSendFinished();
     }
}

int octrlRecFinished(void)
{
     dbgPrintln("Clearing recording buffer");
     sDeviceContext.m_bBusBusy = false;
     nChecksum = 0;
     nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;
     bInvalidPacket = false;
     ptrInputBuffer = ptrInputFinished = (uint8 *)&sInput;
     ptrChecksumStart = &sInput.data[SER_MAX_DATA_LENGTH - 1];
}

int octrlSendFinished(void)
{
     sDeviceContext.m_bOutputReady = false;
     sDeviceContext.m_bWaitForResponse = false;
}

void octrlTimeoutProtection(void)
{
     if (--nTimeoutCounter == 0)
     {
	  dbgPrintln("WARNING! Packet timeout!");
	  octrlRecFinished();
     }
}

uint8 octrlGetDeviceID()
{
     return sDeviceContext.m_nDeviceID; 
}

bool octrlSetDeviceID(uint8 _devId)
{
     sDeviceContext.m_nDeviceID = _devId;
}

uint8 octrlGetBusID()
{
     return sDeviceContext.m_nBusID;
}

bool octrlSetBusID(uint8 _busId)
{
     sDeviceContext.m_nBusID = _busId;
}

uint8 octrlGetMasterID()
{
     return sDeviceContext.m_nMasterID;
}

bool octrlSetMasterID(uint8 _masterId)
{
     sDeviceContext.m_nMasterID = _masterId;
}
