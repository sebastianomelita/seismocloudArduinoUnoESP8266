#ifndef __httpclient_h
#define __httpclient_h

#include "common.h"

void httpRequest(char* host, char* port, char* path, char* postVars, char * buf, char * offset, bool keepAlive=false);
void httpAliveRequest();
void httpQuakeRequest();
void logRequest(char* msg); 
#endif


