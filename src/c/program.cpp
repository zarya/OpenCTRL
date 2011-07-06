#include "WProgram.h"

#include "debug.h"
#include "OpenCTRL.h"

uint8 getShiftRegisterData();
void inputHandler(SPacket *sPacket);

void setup(void)
{
     dbgInitialize();
     dbgPrintln("Loading OpenCTRL...");

     //uint8 deviceId = getShiftRegisterData();

     octrlInitInterface();
     octrlInitProtocol(DEVICE_ID, isMaster(), inputHandler);
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

void inputHandler(SPacket *sPacket)
{
     // got new no protocol packet
}

float getVoltage(int pin)
{
     return (((float)analogRead(pin)) * 0.004882814f);
}

// -- SHIFTERS <header>
#define SN74HC165N_SHLD 12
#define SN74HC165N_CLK 11
#define SN74HC165N_CLK_INH 10
#define SN74HC165N_DAT 9

// -- SHIFTERS <source>
uint8 getShiftRegisterData()
{
     // timing the SN74HC165N proper value of DATA will be there in max 38nS with 16Mhz a clock cycle takes 16nS
     // this means we should be able to load the data without the use of delay

     // store parallel input in loader

     digitalWrite(SN74HC165N_SHLD, HIGH);

     delay(100);

     // shift data to output
     uint8 reg = 0;

     // get the data CLK LOW to HIGH
     register uint8 ctr;
     for (ctr = 0; ctr < 8; ++ctr)
     {
	  digitalWrite(SN74HC165N_CLK, HIGH);

	  delay(100);

	  reg |= digitalRead(SN74HC165N_DAT) << ctr; // fill from last down

	  delay(100);

	  digitalWrite(SN74HC165N_CLK, LOW);
     }

     // release the chip
     digitalWrite(SN74HC165N_SHLD, LOW);

     return reg;
}
