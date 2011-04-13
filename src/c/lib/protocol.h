#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "lib/bytes.h"

// define overal packet structure!

// specs
#define SER_HEADER_LENGTH 6
#define SER_MAX_DATA_LENGTH 32
#define CHECKSUM_SIZE 2 // 2 bytes 16 bit == checksum should be enough...

// header
typedef struct _SSerialHeader {
     uint8 m_nSourceBusID;
     uint8 m_nSourceDeviceID;
     uint8 m_nDestinationBusID;  // is set by de master
     uint8 m_nDestinationDeviceID;   // device ID word ingesteld 
     uint8 m_nPacketID;
     // maximum size 32 bytes
     uint8 m_nPacketLength; // == maximum is 32
} SSerialHeader;

// total packet
typedef struct _SPacket {
     SSerialHeader header;
     uint8 data[SER_MAX_DATA_LENGTH + CHECKSUM_SIZE]; // max data length + 2 for the checksum
} SPacket;

extern uint16 nChecksum;

// OCTRL Protocol functions (placed in m_nPacketLength)
#define OCTRL_MSG_TYPE_START 255 // count backwards so we might can increase data length later ;)

#define OCTRL_PING OCTRL_MSG_TYPE_START - 0 // ping packet
#define OCTRL_PONG OCTRL_MSG_TYPE_START - 1 // pong packet
#define OCTRL_HELLO OCTRL_MSG_TYPE_START -2 // hello packet (send by device when first plugged in)
#define OCTRL_WELCOME OCTRL_MSG_TYPE_START - 3 // welcome packet (send by master when received hello packet)
#define OCTRL_ACK OCTRL_MSG_TYPE_START - 4 // ACK packet send in reply to notifie the the sender that the packet was received successfuly so it can be removed from the send buffer
#define OCTRL_REPORT OCTRL_MSG_TYPE_START - 5 // Send by the master to an node to make it report it's status
#define OCTRL_SILENCE OCTRL_MSG_TYPE_START - 6 // Send by the master to claim the line and make all nods shut the fuck up (can be directed at one client or broadcasted)
#define OCTRL_RESUME OCTRL_MSG_TYPE_START - 7 // Send by the master to make all nodes resume normal operation and notify that the can use the bus again (can be directed at one client or broadcasted)
#define OCTRL_BYE OCTRL_MSG_TYPE_START - 8 // bye packet may be send by the client when it want to unregister itself by the master

#define OCTRL_MSG_TYPE_END OCTRL_BYE // WARNING!! update when adding new message types

#endif // __PROTOCOL_H__
