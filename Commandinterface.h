#ifndef __commandinterface_h
#define __commandinterface_h

#include "common.h"
#define PACKET_SIZE 70

typedef enum {
  	PKTTYPE_DISCOVERY = 1,
  	PKTTYPE_DISCOVERY_REPLY = 2,
  	PKYTYPE_PING = 3,
  	PKYTYPE_PONG = 4,
  	PKTTYPE_START = 5,
  	PKTTYPE_STOP = 6,
  	PKTTYPE_SENDGPS = 7,
  	PKTTYPE_OK = 8,
  	PKTTYPE_SETSYSLOG = 9,
  	PKTTYPE_REBOOT = 10,
  	PKTTYPE_GETINFO = 11,
  	PKTTYPE_GETINFO_REPLY = 12,
  	PKTTYPE_RESET = 13
	//PKTTYPE_TRACE = 14
	} PacketType;

void commandInterfaceInit();
void commandInterfaceTick();
#endif



