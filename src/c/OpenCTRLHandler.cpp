#include "debug.h"
#include "lib/protocol.h"

#include "OpenCTRLHandler.h"

static octrlProtoHandler handleJumpTable[] = {
     octrlRecvPing,
     octrlRecvPong,
     octrlRecvHello,
     octrlRecvWelcome,
     octrlRecvAck,
     octrlRecvReport,
     octrlRecvSilence,
     octrlRecvResume,
     octrlRecvBye,
};

static octrlProtoHandler senderJumpTable[] = {
     octrlSendPing,
     octrlSendPong,
     octrlSendHello,
     octrlSendWelcome,
     octrlSendAck,
     octrlSendReport,
     octrlSendSilence,
     octrlSendResume,
     octrlSendBye,
};

void octrlHandleProtocolPacket(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     register uint8 nFunction =  OCTRL_MSG_TYPE_START - _sInput->header.m_nPacketLength;

     if (nFunction <= OCTRL_MSG_TYPE_END)
	  handleJumpTable[nFunction](_sInput, _sOutput, _sDeviceContext);
     else
	  dbgPrintln("Warning! Trying to handle invalid protocol message (%d)\n", _sInput->header.m_nPacketLength);
}

void octrlSendProtocolPacket(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext, uint8 _packetType)
{
     register uint8 nFunction = OCTRL_MSG_TYPE_START - _packetType;

     if (nFunction <= OCTRL_MSG_TYPE_END)
	  senderJumpTable[nFunction](_sInput, _sOutput, _sDeviceContext);
     else
	  dbgPrintln("Warning! Trying to send invalid protocol message (%d)\n", _packetType);
}

void octrlRecvPing(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     octrlSendProtocolPacket(_sInput, _sOutput, _sDeviceContext, OCTRL_PONG);
}

void octrlSendPong(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     if (! _sDeviceContext->m_bOutputReady)
     {
	  dbgPrintln("Preparing PONG packet");

	  // send PONG packet to bus
	  _sOutput->header.m_nPacketLength = OCTRL_PONG;
	  
	  // set address and packet id
	  octrlReplyToSender(_sInput, _sOutput);

	  _sDeviceContext->m_bOutputReady = true;
     }
     else
     {
	  dbgPrintln("sendPong(): Output buffer already filled!");
     }
}

void octrlRecvHello(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     if (isMaster())
     {
	  octrlSendProtocolPacket(_sInput, _sOutput, _sDeviceContext, OCTRL_WELCOME);
     }
}

void octrlSendHello(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     if (! _sDeviceContext->m_bOutputReady)
     {
	  dbgPrintln("Preparing HELLO packet");

	  // send HELLO packet to bus se we can identify ourselfs
	  _sOutput->header.m_nPacketLength = OCTRL_HELLO;
	  
	  // set address
	  octrlFullBroadcast(_sOutput);
	  
	  // set packet ID
	  octrlSetPacketID(_sOutput, _sDeviceContext, 4); //TODO this is a truely random number chosen by the roll of a dice
	  
	  _sDeviceContext->m_bOutputReady = true;
	  _sDeviceContext->m_bWaitForResponse = true;
	  // set the buffer filled flag so sendData will be called later on... why w8? First we need to monitor to check if the bus is free! =)
     }
     else
     {
	  dbgPrintln("sendHello(): Output buffer already filled!");
     }
}

void octrlSendWelcome(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     if (! _sDeviceContext->m_bOutputReady)
     {
	  dbgPrintln("Perparing WELCOME packet");
	  // set welcome type
	  _sOutput->header.m_nPacketLength = OCTRL_WELCOME;
	  
	  // set output header
	  octrlReplyToSender(_sInput, _sOutput);

	  // only send, if it failes the device will ask again ;)
	  _sDeviceContext->m_bOutputReady = true;
     }
     else
     {
	  dbgPrintln("sendWelcome(): Output buffer already filled!");
     }
}

void octrlRecvWelcome(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     if (_sDeviceContext->m_bWaitForResponse && _sInput->header.m_nPacketID == _sDeviceContext->m_nLastPacketID)
     {
	  dbgPrintln("Valid WELCOME packet");

	  _sDeviceContext->m_nBusID = _sInput->header.m_nSourceBusID;
	  _sDeviceContext->m_nMasterID = _sInput->header.m_nSourceDeviceID;

	  dbgPrintln("Now joined bus (%d) with master (%d)", _sDeviceContext->m_nBusID, _sDeviceContext->m_nMasterID);

	  // we got our network \o/
	  octrlSendFinished();
     }
     else
     {
	  dbgPrintln("Invalid WELCOME packet... ignoring!");
     }
}

void octrlRecvPong(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("Received an pong message very nice but can't handle it yet...");
}

void octrlRecvAck(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     if (_sInput->header.m_nPacketID == _sDeviceContext->m_nLastPacketID)
     {
	  // clear send buffer
	  dbgPrintln("Received an ACK packet with matching packet ID clearing output buffer!");
     }
     else
	  dbgPrintln("WTF is this?! This is not my packet, friend!");
}

void octrlRecvReport(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("Received a REPORT packet... wtf should I do now?!");
}

void octrlRecvSilence(SPacket *_sInput, SPacket *_sOut, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("Received SILEND packet! Need to STFU I guess");
}

void octrlRecvResume(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("wh00t RESUME we may talk again!");
}

void octrlRecvBye(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("Device is parting the bus to bad...");
}

void octrlSendPing(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     if (! _sDeviceContext->m_bOutputReady)
     {
	  dbgPrintln("Preparing PING packet");
	  
	  _sOutput->header.m_nPacketLength = OCTRL_PING;
	  
	  // todo add address code here...

	  _sDeviceContext->m_bOutputReady = true;
     }
     else
     {
	  dbgPrintln("sendPing(): output buffer already filled!");
     }
     dbgPrintln("Need to send a ping packet... but i'm lazy");
}

void octrlSendAck(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     if (! _sDeviceContext->m_bOutputReady)
     {
	  dbgPrintln("Preparing ACK packet");

	  // send PONG packet to bus
	  _sOutput->header.m_nPacketLength = OCTRL_ACK;
	  
	  // set address and packet id
	  octrlReplyToSender(_sInput, _sOutput);

	  _sDeviceContext->m_bOutputReady = true;
     }
     else
     {
	  dbgPrintln("sendAck(): Output buffer already filled!");
     }
}

void octrlSendReport(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("Need to send an report packet but cba!");
}

void octrlSendSilence(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("Need to send an silence packet but i don't know how yet");
     if (! _sDeviceContext->m_bOutputReady)
     {
	  if (isMaster())
	  {
	       dbgPrintln("Preparing SILENCE packet");
	       
	       // send SILENCE packet to bus
	       _sOutput->header.m_nPacketLength = OCTRL_SILENCE;

	       // set broadcast address
	       octrlFullBroadcast(_sOutput);

	       _sDeviceContext->m_bOutputReady = true;
	  }
	  else
	       dbgPrintln("sendSilence(): Try to send SILENCE but device is not a bus master!");
     }
     else
     {
	  dbgPrintln("sendSilence(): Output buffer already filled!");
     }
}

void octrlSendResume(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("Need to send an RESUME packet but i like the silence..");
}

void octrlSendBye(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext)
{
     dbgPrintln("Need to send an BYE packet... don't want to leave so FU!");
}
