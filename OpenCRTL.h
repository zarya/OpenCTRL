#define MY_DEVICE_ID 01
#define MY_NETWORK_ID 01

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

// TODO optimize all booleans into struct with bitfield :1 ^.^
#define bool uint8
#define true 1
#define false 0

#define SERIAL_TIMEOUT_LIMIT 1000 // 1000 loop cycles is timeout
#define SERIAL_WAIT_TIME 32000000 // 32000000 cycles == more then 2 seconds

// packet specs
#define SER_HEADER_LENGTH 6
#define SER_MAX_DATA_LENGTH 32
#define CHECKSUM_SIZE 2 // 2 bytes 16 bit == checksum should be enough...

#define OCTRL_MSG_TYPE_START 255 // count backwards so we might can increase data length later ;)

#define OCTRL_PING OCTRL_MSG_TYPE_START - 0 // ping packet
#define OCTRL_PONG OCTRL_MSG_TYPE_START - 1 // pong packet
#define OCTRL_HELLO OCTRL_MSG_TYPE_START -2 // hello packet (send by device when first plugged in)
#define OCTRL_WELCOME OCTRL_MSG_TYPE_START - 3 // welcome packet (send by master when received hello packet)
#define OCTRL_ACK OCTRL_MSG_TYPE_START - 4 // ACK packet send in reply to notifie the the sender that the packet was received successfuly so it can be removed from the send buffer
#define OCTRL_REPORT OCTRL_MSG_TYPE_START - 5 // Send by the master to an node to make it report it's status
#define OCTRL_SILENC OCTRL_MSG_TYPE_START - 6 // Send by the master to claim the line and make all nods shut the fuck up (can be directed at one client or broadcasted)
#define OCTRL_RESUME OCTRL_MSG_TYPE_START - 7 // Send by the master to make all nodes resume normal operation and notify that the can use the bus again (can be directed at one client or broadcasted)

// define the serial header for the packets
typedef union _SSerialHeader {
     struct {
	  uint8 m_nSourceNetwork;
	  uint8 m_nSourceDeviceID;
	  uint8 m_nDestinationNetwork;  // is set by de master
	  uint8 m_nDestinationDevice;   // device ID word ingesteld 
	  uint8 m_nPacketID;
	  // maximum size 32 bytes
	  uint8 m_nPacketLength; // == maximum is 32
     };
     uint8 arr[SER_HEADER_LENGTH];
} SSerialHeader;

// define the total packet

typedef struct _SPacket {
     SSerialHeader header;
     uint8 data[SER_MAX_DATA_LENGTH + CHECKSUM_SIZE]; // max data length + 2 for the checksum
} SPacket;

void setup(void);
void loop(void)  ;                   // run over and over again
inline void timeoutProtection();
int handleProtocolPacket(void);
int sendPong(void);
int sendData(void ); // normaly respond to master so we don't care, master asks again
int recFinished(void);
int sendFinished(void);
void initSerial();
void initOpenCTRL();
void readSerial();
int recvWelcome();
int sendWelcome();
int sendHello();
