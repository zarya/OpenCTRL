//#include "WProgram.h"

#include "debug.h"
#include "OpenCTRL.h"

void setup(void)
{
     dbgInitialize();
     dbgPrintln("Loading OpenCTRL...");

     octrlInitInterface();
     octrlInitProtocol(DEVICE_ID, isMaster());
     dbgPrintln("Running as (%s) with Device ID (%d) and Network ID (%d)", isMaster() ? "MASTER" : "SLAVE", octrlGetDeviceID(), octrlGetBusID());

     // only for startup use delay and start to make sure the bus is empty before trying to send
     delay(500);
     octrlReadData();
     delay(500);
}

void loop(void)                     // run over and over again
{
     // main loop
     octrlReadData();
     delay(SERIAL_WAIT_TIME);
     octrlSendData();
}

