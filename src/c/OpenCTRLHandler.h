#ifndef __OPEN_CTRL_HANDLER_H__
#define __OPEN_CTRL_HANDLER_H__

#include "lib/protocol.h"

#include "OpenCTRL.h"

#ifdef __cplusplus
extern "C"
{
#endif
     // function pointer defintion for input handlers
     typedef void (*octrlProtoHandler)(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     // handle incomming Open CTRL protocol packets
     extern void octrlHandleProtocolPacket(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     // prepare outgoing Open CTRL protocol packets
     extern void octrlSendProtocolPacket(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext, uint8 _packetType);

     // set output buffer for default reply
     extern void octrlSendReply(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     inline void octrlReplyToSender(SPacket *_sOutput, SPacket *_sInput)
     {
	  _sOutput->header.m_nDestinationDeviceID = _sInput->header.m_nSourceDeviceID;
	  _sOutput->header.m_nDestinationBusID = _sInput->header.m_nSourceBusID;
	  _sOutput->header.m_nPacketID = _sInput->header.m_nPacketID;
     }

     inline void octrlFullBroadcast(SPacket *_sOutput)
     {
	  _sOutput->header.m_nDestinationDeviceID = 0;
	  _sOutput->header.m_nDestinationBusID = 0;
     }

     inline void octrlBusBroadcast(SPacket *_sOutput)
     {
	  _sOutput->header.m_nDestinationDeviceID = 0;
	  _sOutput->header.m_nDestinationBusID = octrlGetBusID();
     }

     inline void octrlSetPacketID(SPacket *_sOutput, SDeviceContext *_sDeviceContext, uint8 _packetID)
     {
	  _sOutput->header.m_nPacketID = _packetID;
	  _sDeviceContext->m_nLastPacketID = _packetID;
     }

     extern void octrlRecvPing(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlRecvPong(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlRecvHello(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlRecvWelcome(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlRecvAck(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlRecvReport(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlRecvSilence(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlRecvResume(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlRecvBye(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendPing(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendPong(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendHello(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendWelcome(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendAck(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendReport(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendSilence(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendResume(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);

     extern void octrlSendBye(SPacket *_sInput, SPacket *_sOutput, SDeviceContext *_sDeviceContext);
#ifdef __cplusplus
}
#endif

#endif // __OPEN_CTRL_HANDLER_H__
