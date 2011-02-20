void setup(void);
void loop(void)  ;                   // run over and over again
inline void timeoutProtection();
int handleProtocolPacket(void);
int sendPong(void);
int sendData();
int recFinished(void);
int sendFinished(void);
