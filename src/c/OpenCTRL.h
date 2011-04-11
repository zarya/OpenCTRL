#ifndef __OPENCTRL_H__
#define __OPENCTRL_H__

#define MY_DEVICE_ID 01
#define MY_NETWORK_ID 01

#define MASTER 1
#define SLAVE 2

#if SER_DEVICE_TYPE != MASTER && SER_DEVICE_TYPE != SLAVE
#error Device type must be MASTER or SLAVE
#endif

#if DEVICE_ID <= 0 || DEVICE_ID > 255
#error Device ID must be between 1 and 255
#endif

#define isMaster() (SER_DEVICE_TYPE == MASTER)
#define isDevice() (SER_DEVICE_TYPE == SLAVE)

#define SERIAL_TIMEOUT_LIMIT 1000 // 1000 loop cycles is timeout
#define SERIAL_WAIT_TIME 20000

#if __cplusplus
extern "C"
{
#endif
     typedef struct _SDeviceContext {
	  //TODO make it a status register with one bit for each boolean..
	  bool m_bBusBusy;
	  bool m_bOutputReady;
	  bool m_bWaitForResponse;

	  uint8 m_nBusID;
	  uint8 m_nDeviceID;
	  uint8 m_nMasterID;
	  uint8 m_nLastPacketID;
     } SDeviceContext;

     // initialize the RS-485 bus
     extern void octrlInitInterface(void);

     // initialize the OpenCTRL bus
     extern void octrlInitProtocol(uint8 _devId, bool _master);

     // read data from the bus if any available
     extern void octrlReadData(void);

     // send data to the bus if any in the buffer and the bus is free
     extern int octrlSendData(void);

     // resets the input buffer for a new packet
     extern int octrlRecFinished(void);

     // resets the output buffer for a new packet
     extern int octrlSendFinished(void);

     // inline function for timeout protector
     extern void octrlTimeoutProtection(void);

     // inline function to get the device id
     extern uint8 octrlGetDeviceID(void);

     // inline function to set the device id (returns false if the device is already initialized)
     extern bool octrlSetDeviceID(uint8 _devId);

     // inline function to get the bus id
     extern uint8 octrlGetBusID(void);
     
     // inline function to set the bus id (returns false if the device is already initialized)
     extern bool octrlSetBusID(uint8 _busId);

     extern uint8 octrlGetMasterID(void);

     extern bool octrlSetMasterID(uint8 _busId);
#if __cplusplus
}
#endif

#endif // __OPENCTRL_H__
