#include "Commandinterface.h"

//byte udpPacketBuffer[PACKET_SIZE];

void commandInterfaceInit() {
	Serial.println(F("beginUDPServer"));
    ESP8266wifi::getWifi().beginUDPServer("62001");
}

void commandInterfaceTick() {
  byte udpPacketBuffer[PACKET_SIZE];
  //int packetSize = ESP8266wifi::getWifi().parseUDPPacket(); //ci vorrebbe ma, se si legge tutto il buffer con una read, 
  //l'available Ã¨ sufficiente per richiamare un nuovo paccchetto
  if(ESP8266wifi::getWifi().available(10, NULL, LOCALSERVER)) {
  	Serial.println(F("available"));
    // read the packet into packetBufffer
    size_t r = ESP8266wifi::getWifi().read((char*)udpPacketBuffer, PACKET_SIZE);
    Serial.print(F("Packet size: "));
    Serial.println(r);
    if(memcmp("INGV\0", udpPacketBuffer, 5) != 0) {
      return;
    }
    
    bool reboot = false;
    unsigned long unixTimeM = getUNIXTime();
    unsigned long uptime = getUNIXTime() - getBootTime();
    byte macaddress[6] = { 0 };
    HEXStrToByte(macaddress, ESP8266wifi::getWifi().getMAC());
    uint32_t probeSpeed = getProbeSpeedStatistic();
    uint32_t freeramkb = freeMemory();
    float latency = 0;
    if(udpPacketBuffer[5] == PKTTYPE_GETINFO) {
      latency = tcpLatency();
    }

    float longitude = 0;
    float latitude = 0;
 
    switch(udpPacketBuffer[5]) {
      case PKTTYPE_DISCOVERY:
      	
      	Serial.println(F("DISCOVERY"));
      	
        // Reply to discovery
        udpPacketBuffer[5] = PKTTYPE_DISCOVERY_REPLY;

        memcpy(udpPacketBuffer + 6,  macaddress, 6);
        memcpy(udpPacketBuffer + 12, getVersionAsString().c_str(), 4);
        memcpy(udpPacketBuffer + 16, "uno", 4);
        break;
      case PKYTYPE_PING:
      	Serial.println(F("PING"));
        // Reply to ping
        udpPacketBuffer[5] = PKYTYPE_PONG;
        break;
      case PKTTYPE_SENDGPS:
      	Serial.println(F("SENDGPS"));
        // Get coords
        udpPacketBuffer[5] = PKTTYPE_OK;

        memcpy(&latitude, udpPacketBuffer + 12, 4);
        memcpy(&longitude, udpPacketBuffer + 16, 4);
        reverse4bytes((byte*)&latitude);
        reverse4bytes((byte*)&longitude);
        break;
      case PKTTYPE_REBOOT:
      	Serial.println(F("REBOOT"));
        // Reboot
        // Reply with OK
        udpPacketBuffer[5] = PKTTYPE_OK;
        reboot = true;
        break;
      case PKTTYPE_GETINFO:
      	Serial.println(F("GETINFO"));
        udpPacketBuffer[5] = PKTTYPE_GETINFO_REPLY;

        memcpy(udpPacketBuffer + 6,  macaddress, 6);
        //memcpy(udpPacketBuffer + 12, ip, 4);
        memcpy(udpPacketBuffer + 28, &uptime, 4);
        memcpy(udpPacketBuffer + 32, &unixTimeM, 4);
        memcpy(udpPacketBuffer + 36, VERSION, 4);
        memcpy(udpPacketBuffer + 40, &freeramkb, 4);
        memcpy(udpPacketBuffer + 44, &latency, 4);
        memcpy(udpPacketBuffer + 53, "uno", 3);
        memcpy(udpPacketBuffer + 57, "MPU6050", 7);
        memcpy(udpPacketBuffer + 65, &probeSpeed, 4);

        break;
        /*case PKTTYPE_SETSYSLOG: {
				uint32_t ip = 0;
				memcpy(&ip, pktBuffer + 6, 4);
				pkt->syslogServer = IPaddr(ip);
			Log::setSyslogServer(pkt.syslogServer);

			pkt.type = PKTTYPE_OK;
			sendPacket(pkt);
		}*/
#ifdef RESET_ENABLED
      case PKTTYPE_RESET:
        initEEPROM();
        reboot = true;
        break;
#endif
      default:
        // Unknown packet or invalid command
        Serial.println(F("INVALID COMMAND"));
        return;
    }

    if(longitude != 0 && latitude != 0) {
      setLongitude(longitude);
      setLatitude(latitude);
    }

    ESP8266wifi::getWifi().beginUDPPacket();
    ESP8266wifi::getWifi().write((unsigned char*) udpPacketBuffer,sizeof(udpPacketBuffer));
    ESP8266wifi::getWifi().endUDPPacket(false,LOCALSERVER);

    /*if(reboot) {
      soft_restart();
    }*/
  }
}



