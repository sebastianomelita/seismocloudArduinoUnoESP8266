#include "httpclient.h"
//#include <utility/w5100.h>
//#include <utility/socket.h>
#include <avr/pgmspace.h>
//#define SYSLOG_PKT_SIZE 128
#define BUFLEN1 64
#define BUFLEN2 83

 void httpQuakeRequest() {
  char *server = "www.sapienzaapps.it";
  
  char buf[BUFLEN1];
  char out[9];
  snprintf(buf,BUFLEN1, "deviceid=%s", ESP8266wifi::getWifi().getMAC());
  ftoa(out,9,getUNIXTime());
  snprintf(buf,BUFLEN1, "%s&tsstart=%s", buf, out);
  ftoa(out,9,getLatitude());
  snprintf(buf,BUFLEN1, "%s&lat=%s", buf, out);
  ftoa(out,9,getLongitude());
  snprintf(buf,BUFLEN1, "%s&lon=%s&sensor=MPU6050", buf, out);
  
  //Serial.println(postVars);
  httpRequest(server, "80", "/seismocloud/terremoto.php", buf, buf, "\r\n\r\n",true);
  Serial.print(F("\nQuake response: "));
  buf[1]=0;
  Serial.println(buf);

  Serial.println(F("\nEnd httpQuakeRequest")); 
}

void httpAliveRequest() {
  char *server = "www.sapienzaapps.it";
  //Serial.println(F("\nBegin HttpAliveRequest"));
  char buf[BUFLEN2], sigma[10];
  char out[9];
  snprintf(buf,BUFLEN2, "deviceid=%s", ESP8266wifi::getWifi().getMAC());
  snprintf(buf,BUFLEN2, "%s&model=uno&version=%s", buf, getVersionAsString().c_str());
  ftoa(out,9,getLatitude());
  snprintf(buf,BUFLEN2, "%s&lat=%s", buf, out);
  ftoa(out,9,getLongitude());
  snprintf(buf,BUFLEN2, "%s&lon=%s&sensor=MPU6050", buf, out);
  //Serial.println(postVars);
  //postVars += "&memfree="  + String(getLongitudeAsString()); 
  //postVars += "&avg=" + getDoubleAsString(getCurrentAVG());
  //postVars += "&stddev=" + getDoubleAsString(getCurrentSTDDEV());
  
  //char msg2[128];	
  //snprintf(msg2,128, "deviceid=%s&model=uno&version=%s&lat=%s&lon=%s&sensor=MPU6050", ESP8266wifi::getWifi().getMAC(), VERSION, getLatitude(), getLongitude());
  //postVars += "&jsonoutput=1"; //134 Bytes! attualmente il buffer MSG_BUFFER_MAX su ESP8266wifi.h Ã¨ di 128 byte
  httpRequest(server, "80", "/seismocloud/alive.php", buf, buf, "server:",true); 
  Serial.print(F("\nAliveResponse: "));
  Serial.println(buf);
  readParameter(buf,"sigma",sigma,10);
  if(atof(sigma)>0)
     setSigma(atof(sigma)); 
  else
     setSigma(11.33);
 
  Serial.println(F("\nEnd HttpAliveRequest"));
}

void logRequest(char* msg) {	
  char *server = "www.sapienzaapps.it";  // log server
  char msg2[116];	
  snprintf(msg2,116, "<134>[%lu] [I] [%s] %s", getUNIXTime(), ESP8266wifi::getWifi().getMAC(), msg);
  // Send a syslog request
  //Serial.print(F("\nSyslog request: "));
  ESP8266wifi::getWifi().beginUDPPacket((const char*)server, "514"); // 514 is the syslog port
  ESP8266wifi::getWifi().write((const unsigned char*)msg2,strlen(msg2));
  ESP8266wifi::getWifi().endUDPPacket(false);
  ESP8266wifi::getWifi().disconnectFromServer();//non aspetto risposta
  Serial.println(F("\nEnd logRequest")); 
}

void httpRequest(char* host, char* port, char* path, char *postVars, char * buf, char *offset, bool keepAlive){
  //Serial.println(F("\nBeginHttpRequest")); NON STAMPARE NULLA ADESSO, INSPIEGABILMENTE SI PIANTA!
  char len[5];	
  ESP8266wifi &client=ESP8266wifi::getWifi();
  // if there's a successful connection:
  int cresult = client.beginTCPConnection(host, port);

  if (cresult) {
    if(postVars == "") {
    	client.printD(F("\nGET "));
    } else {
        client.printD(F("\nPOST "));
    }
    client.printD(path);
    client.printlnD(F(" HTTP/1.1"));
    client.printD(F("Host: "));
    client.printlnD(host);
    client.printlnD(F("User-Agent: arduino-esp8266"));
    client.printD(F("Connection: "));
    if(keepAlive) {
    	client.printlnD(F("keep-alive"));
    } else {
        client.printlnD(F("closed"));
    }
    //client.println(F("User-Agent: arduino-ethernet"));
    if(postVars != "") {
      client.printlnD(F("Content-Type: application/x-www-form-urlencoded"));
      client.printD(F("Content-Length: "));
	  sprintf(len,"%d",strlen(postVars));
      client.printlnD(len);
      client.printlnD(F(""));
      client.printD(postVars);
    }else{
    	client.printlnD(F("Content-Length: 0"));
		client.printlnD(F(""));
	} 
    
    if(client.available(3000,offset)) {  //legge la risposta a partire dall stringa in offset 
		client.readLine(buf,80);
    } else {
        Serial.println(F("Socket read error"));
    }
    
  } else {
    // if you couldn't make a connection:
    Serial.print(F("connection failed to: "));
    Serial.print(host);
    Serial.print(F(":"));
    Serial.print(port);
    Serial.print(F(" "));
    Serial.println(cresult);
  }
  if(keepAlive) {
  	Serial.print(F("\nDisconnect HTTP From: "));
  	Serial.print(host);
  	Serial.print(F(":"));
  	Serial.print(port);
  	Serial.println(F(""));
  	client.disconnectFromServer();
  }
  Serial.print(F("\nFree memory: "));
  sprintf(len,"%u",freeMemory());
  Serial.print(len);  
  Serial.println(F(" byte")); 
}



