#include <NewSoftSerial.h>

#include "OpenCRTL.h"

#define MY_DEVICE_ID 01
#define MY_NETWORK_ID 01

NewSoftSerial mySerial(2, 3);

#define bool char
#define true 1
#define false 0

#define SERIAL_TIMEOUT_LIMIT 1000 // 1000 loop cycles is timeout
#define HEADER_LENGTH 7
#define SER_MAX_DATA_LENGTH 32
#define SER_START_PROTOCOL_METHODS 34
#define SER_HELLO 253
#define SER_PING 254            // count backwards so we might can increase data length later ;)
#define SER_PONG 255
#define CRC_LENGTH 2 // 2 bytes 16 bit crc16...

// define the serial header for the packets
typedef union _SSerialHeader {
  struct sSerialHeader {
    char m_nSourceNetwork;
    char m_nSourceDeviceID;
    char m_nDestinationNetwork;  // is set by de master
    char m_nDestinationDevice;   // device ID word ingesteld 
    short m_nPackedID;
    // maximum size 32 bytes
    char m_nPacketLength; // == maximum is 32
  } str;
  char arr[HEADER_LENGTH];
} SSerialHeader;

bool bBusBusy = false;
bool bOurData = false;
bool bWriteData = false;
int nDataLength = 0;

char aData[SER_MAX_DATA_LENGTH];

SSerialHeader serialHeader;

int nHeaderIdx = 0;
int nDataIdx = 0;
char nShadow = 0;
short nChecksum = 0;

int nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;

void setup(void)
{
  mySerial.begin(9600);
  // send OHAI packet to bus
}

void loop(void)                     // run over and over again
{
  if (mySerial.available() && !bWriteData) // only read when there is no data in the buffer we want to right first
    // ugly as shit build two buffers lazy ass....
    {
      nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;
      
      bBusBusy = true;
      
      // bits... go parse!
      if (nHeaderIdx < HEADER_LENGTH)
	nShadow = serialHeader.arr[nHeaderIdx++] = (char)mySerial.read();
      else
	{
	  nShadow = aData[nDataIdx++] = (char)mySerial.read(); 
	  nChecksum += nShadow;
	}
      // TODO FROM HEADER BUFFER TO DATA BUFFER IS HEADER IS FULL CALCULATE OFFSET FROM START ADDRESS to START ADDRESS + HEADER_LENGTH
      
      if (nHeaderIdx == HEADER_LENGTH) // if we have 5 bytes check if it is for us
	{
	  nHeaderIdx++;
	  
	  if (serialHeader.str.m_nPacketLength > SER_MAX_DATA_LENGTH && serialHeader.str.m_nPacketLength == 0)
	    {
	      if (serialHeader.str.m_nDestinationNetwork == MY_DEVICE_ID && serialHeader.str.m_nDestinationNetwork == MY_NETWORK_ID)
		{
		  // packets is ours!
		  bOurData = true;
		  
		  // it is a protocol packets
		  handleProtocolPacket();
		}
	      
	      // transmission ended bus free
	      recFinished();
	    }
	}
      else if (nHeaderIdx == HEADER_LENGTH + 1) // we got the whole buffer and it's parsed all data now is data
	{
	  // check data is not a protocol packet store data in data buffer
	  if (nDataIdx == serialHeader.str.m_nPacketLength)
	    {
	      if (bOurData)
		{
		  // do some magic
		}
	      
	      recFinished(); // got all data bus is free again \o/
	    }
	}
      else
	{
	  // this can only happen if there wasn't anything on the bus for lets say.... 1ms
	  // bBusBusy = false;
	  timeoutProtection();
	}
    }
  else if (bBusBusy)
    {
      timeoutProtection();
    }

  if (bWriteData)
    {
      // sendData((char*)0, (char*)0, 0);
      sendData();
    }

  // bus open do as you please! \o/
}

inline void timeoutProtection()
{
  if (--nTimeoutCounter == 0)
    {
      recFinished();
    }
}

int handleProtocolPacket(void)
{
  switch (serialHeader.str.m_nPacketLength)
    {
    case SER_PING:
      sendPong();
      break;
    }
}

int sendPong(void)
{
  // source id & net == new destination
  char nTemp;
  
  serialHeader.str.m_nDestinationDevice = serialHeader.str.m_nSourceDeviceID;
  serialHeader.str.m_nDestinationNetwork = serialHeader.str.m_nSourceNetwork;
  // keep packed ID 
  // set our net id and dev ID
  serialHeader.str.m_nSourceNetwork = MY_NETWORK_ID;
  serialHeader.str.m_nSourceDeviceID = MY_DEVICE_ID;
  serialHeader.str.m_nPacketLength = SER_PONG; // set as pong

  nDataLength = 0;

  // barf data to bus if free
  //sendData(serialHeader.arr, (char*)0, 0);
  sendData();
}

//int sendData(char *header, char *sendData, char nDataLength)
int sendData()
{
  if (!bBusBusy)
    {
      // if bus full don't send yet and just return and keep buffers
      // else we can send the data
      
      register char idx = 0;
      union _crc16 {
	short checksum;
	char arr[2];
      } crc16;
      
      // increase header[dataLength] with 2...
      //((SSerialHeader*)header)->str.m_nPacketLength += 2;
      serialHeader.str.m_nPacketLength += 2;
      
      // write header to bus
      for (; idx < HEADER_LENGTH; idx++)
	{
	  mySerial.print(serialHeader.arr[idx], BYTE);
	}
      
      // write data to bus
      for (idx = 0; idx < nDataLength; idx++)
	{
	  crc16.checksum += aData[idx];
	  mySerial.print(aData[idx], BYTE);
	}
      
      if (nDataLength > 0)
	{
	  // write checksum to bus
	  for (idx = 0; idx < CRC_LENGTH; idx++)
	    {
	      mySerial.print(crc16.arr[idx], BYTE);
	    }
	}
    }
}

int recFinished(void)
{
  bBusBusy = false;
  nDataIdx = 0;
  nHeaderIdx = 0;
  nChecksum = 0;
  nTimeoutCounter = SERIAL_TIMEOUT_LIMIT;
}

int sendFinished(void)
{
  nDataLength = 0;
}

