#ifndef __OPENCTRL_H__
#define __OPENCTRL_H__

#define MY_DEVICE_ID 01
#define MY_NETWORK_ID 01

#define isMaster() (SER_DEVICE_TYPE == MASTER)
#define isDevice() (SER_DEVICE_TYPE == SLAVE)

#define SERIAL_TIMEOUT_LIMIT 1000 // 1000 loop cycles is timeout
#define SERIAL_WAIT_TIME 20000

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

#endif // __OPENCTRL_H__
