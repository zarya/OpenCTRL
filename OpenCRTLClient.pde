#include <NewSoftSerial.h>
#include "OpenCRTL.h"

#define isChecksumValid() (*((uint16*)ptrChecksumStart) == nChecksum)

NewSoftSerial serBus(2, 3);

// generic bus status
bool bBusBusy = false; // set when input occures or we are ouputting
uint8 nNetworkID = 0;
uint8 nDeviceID = 0;
uint8 nMasterID = 0;

// serial input variables (buffers and indexes)
SPacket sInput;
uint8 *ptrInputBuffer = NULL;
uint8 *ptrInputFinished = NULL;
uint8 *ptrChecksumStart = NULL;
uint16 nChecksum = 0;
uint8 nLastChar = 0;
bool bInvalidPacket = false;

// serial output buffers
SPacket sOutput;
bool bOutputReady = false;
bool bWaitForResponse = false;
uint16 nLastPacketID = 0; // remember to check if the master recved our 'interrupt / state change'

int nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;

void setup(void)
{
     Serial.begin(57600);
     Serial.println("Loading OpenCTRL Client...");
     Serial.println("Starting soft serial interface");
     initSerial();
     Serial.println("Starting OpenCTRL interface (over soft serial)");
     initOpenCTRL();

     // only for startup use delay and start to make sure the bus is empty before trying to send
     delay(100);
     readSerial();
     delay(100);
     readSerial();
     sendData();
}

void loop(void)                     // run over and over again
{
     // main loop
}

void initSerial()
{
     serBus.begin(9600);
     ptrInputBuffer = (uint8 *)&sInput;
}

void initOpenCTRL()
{
// TODO only when device is not set master!!! If is set master start initial ping to all nods... 
// BUG How does the device know when it's newly added to the BUS or it sufferd from power loss, not all MCU's have Brown Out Register
     if (false) // TODO check master pin
	  return; // scanNetwork()
     else
	  sendHello();
}

void readSerial()
{
     if (serBus.available())
     {
	  nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;
	  bBusBusy = true;
	  
	  // bits... go parse!
	  *ptrInputBuffer++ = nLastChar = (char)serBus.read();
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
	       if (!bInvalidPacket)
	       {
		    // we got the whole packet! Check if we have anything to do with it...
		    if (
			 (sInput.header.m_nDestinationNetwork == nNetworkID && sInput.header.m_nDestinationDevice == nDeviceID) // directed at us
			 || (sInput.header.m_nDestinationNetwork == nNetworkID && sInput.header.m_nDestinationDevice == 0 && sInput.header.m_nSourceDeviceID == nMasterID) // broadcast msg from master
			 ) // end of if
		    {
			 // packet is directed at us \o/
			 if (isChecksumValid())
			 {
			      // if good parse rest of the packet
			      if (sInput.header.m_nPacketLength > SER_MAX_DATA_LENGTH)
			      {
				   handleProtocolPacket(); // protocol packet parse the fucker
				   recFinished(); // data handled clear buffers
			      }
			 }
		    }
		    else
		    {
			 // we got nothing to do with it reset buffer for new packet
			 recFinished();
		    }
	       }
	       else
	       {
		    // invalid packet reset buffers
		    recFinished();
	       }
	  }
	  else if (ptrInputBuffer == &sInput.data[SER_MAX_DATA_LENGTH])
	  {
	       // packet longer then RFC mark invalid and reset pointer to prefent buffer overflow ;)
	       ptrInputBuffer = &sInput.data[0]; // overwrite previous data invalid anywayzz
	       bInvalidPacket = true;
	  }

	  // just another char... we don't care ^.^
     }
     else if (bBusBusy) // bus is busy but we didn't recieve any data 
     {
	  timeoutProtection();
     }
}

inline void timeoutProtection()
{
     if (--nTimeoutCounter == 0)
     {
	  recFinished();
     }
}

// make this some struct array for automated function calling
int handleProtocolPacket(void)
{
     switch (sInput.header.m_nPacketLength)
     {
     case OCTRL_PING:
	  sendPong();
	  break;

//     case OCTRL_PONG: // Should be placed in the firmware not in the driver...
//	  recvPong();
//	  break;

     case OCTRL_HELLO:
	  sendWelcome();
	  break;

     case OCTRL_WELCOME:
	  recvWelcome();
	  break;
     }
}

//int sendData(char *header, char *sendData, char nDataLength)
int sendData(bool _waitForResponse)
{
     if (!bBusBusy && bOutputReady)
     {
	  // if bus full don't send yet and just return and keep buffers
	  // else we can send the data
	  
	  register char idx = 0;
	  union _checksum {
	       uint16 checksum;
	       char arr[CHECKSUM_SIZE];
	  } cs;
	  
	  // write header to bus
	  for (; idx < SER_HEADER_LENGTH; idx++)
	  {
	       cs.checksum += sOutput.header.arr[idx];
	       serBus.print(sOutput.header.arr[idx], BYTE);
	  }
	  
	  // write data to bus
	  for (idx = 0; idx < sOutput.header.m_nPacketLength; idx++)
	  {
	       cs.checksum += sOutput.data[idx];
	       serBus.print(sOutput.data[idx], BYTE);
	  }
	  
	  // write checksum to bus
	  for (idx = 0; idx < CHECKSUM_SIZE; idx++)
	  {
	       serBus.print(cs.arr[idx], BYTE);
	  }
	  
	  if (_waitForResponse)
	       bWaitForResponse = _waitForResponse;
	  else
	       sendFinished();
     }
}

int recFinished(void)
{
     bBusBusy = false;
     nChecksum = 0;
     nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;
     bInvalidPacket = false;
     ptrInputBuffer = ptrInputFinished = (uint8 *)&sInput;
     ptrChecksumStart = &sInput.data[SER_MAX_DATA_LENGTH - 1];
}

int sendFinished(void)
{
     bOutputReady = false;
     bWaitForResponse = false;
}

int recvWelcome(void)
{
     if (bWaitForResponse && sInput.header.m_nPacketID == nLastPacketID)
     {
	  nNetworkID = sInput.header.m_nDestinationNetwork;

	  // we got our network \o/
	  sendFinished();
     }
}

int sendWelcome(void)
{
     if (! bOutputReady)
     {
	  // set welcome type
	  sOutput.header.m_nPacketLength = OCTRL_WELCOME;
	  
	  // set output header
	  sOutput.header.m_nDestinationNetwork = nNetworkID;
	  sOutput.header.m_nDestinationDevice = sInput.header.m_nSourceDeviceID;
	  sOutput.header.m_nSourceDeviceID = nDeviceID;
	  sOutput.header.m_nSourceNetwork = nNetworkID;
	  
	  // set packet ID to recv packet id... for obious reasons
	  sOutput.header.m_nPacketID = sInput.header.m_nPacketID;

	  // only send, if it failes the device will ask again ;)
	  bOutputReady = true;
     }
     else
     {
	  Serial.println("sendWelcome(): Output buffer already filled!");
     }
}

int sendHello(void)
{
     if (! bOutputReady)
     {
	  // send HELLO packet to bus se we can identify ourselfs
	  sOutput.header.m_nPacketLength = OCTRL_HELLO;
	  
	  // set address
	  sOutput.header.m_nDestinationDevice = 0;
	  sOutput.header.m_nDestinationNetwork = 0;
	  sOutput.header.m_nSourceNetwork = 0;
	  sOutput.header.m_nSourceDeviceID = nDeviceID;
	  
	  // set packet ID
	  sOutput.header.m_nPacketID = nLastPacketID = 4; //TODO this is a truely random number chosen by the roll of a dice
	  
	  bOutputReady = true;
	  bWaitForResponse = true;
	  // set the buffer filled flag so sendData will be called later on... why w8? First we need to monitor to check if the bus is free! =)
     }
     else
     {
	  Serial.println("sendHello(): Output buffer already filled!");
     }
}

int sendPong(void)
{
     if (! bOutputReady)
     {
	  // send PONG packet to bus
	  sOutput.header.m_nPacketLength = OCTRL_PONG;
	  
	  // set address
	  sOutput.header.m_nDestinationDevice = sInput.header.m_nSourceDeviceID;
	  sOutput.header.m_nDestinationNetwork = sInput.header.m_nSourceNetwork;
	  sOutput.header.m_nSourceNetwork = nNetworkID;
	  sOutput.header.m_nSourceDeviceID = nDeviceID;
	  
	  // set packet ID
	  sOutput.header.m_nPacketID = sInput.header.m_nPacketID; 
	  
	  bOutputReady = true;
     }
}
