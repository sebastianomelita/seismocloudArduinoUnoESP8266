#ifndef __COMMON_H
#define __COMMON_H

#define LED_RED     8
#define LED_YELLOW  9
#define LED_GREEN   7
#define VERSION     "1.10"

//#define RESET_ENABLED
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "MemoryFree.h"
#include "utils.h"
#include "MPU6050.h"
#include "SoftReset.h"
#include "LED.h"
#include "ntp.h"
#include "ESP8266WIFI.h"
#include "statistics.h"
#include "seismometer.h"
#include "Commandinterface.h"
#include "httpclient.h"


void initEEPROM();
void checkEEPROM();
float getLatitude();
String getLatitudeAsString();
String getLongitudeAsString();
String getDoubleAsString(double d);
float getLongitude();
unsigned long getBootTime();
void setBootTime(unsigned long);
void setLatitude(float l);
void setLongitude(float l);
void loadConfig();
void setProbeSpeedStatistic(uint32_t);
uint32_t getProbeSpeedStatistic();
String getVersionAsString();
byte* HEXStrToByte(byte* , char*);
byte getNumVal(char);
void setSigma(float s);
float getSigma();
bool readParameter(char* cfg, char* tag, char* into, int maxn); 
void ftoa(char* buf, int m, float fnum);
#endif


