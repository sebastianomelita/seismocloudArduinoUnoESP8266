
#ifndef __SEISMOMETER_H
#define __SEISMOMETER_H

#include "common.h"
void seismometerInit();
void seismometerTick();
statistics& getStat();
double getCurrentAVG();
double getCurrentSTDDEV();
void resetStat();
void calibrate(float iter);

typedef struct {
	unsigned long ts;
	double accel;
	bool overThreshold;
} RECORD;

	//unsigned long statLastCounterTime = 0;
	//unsigned int statProbeSpeed = 0;
	//bool inEvent;
	//unsigned long lastEventWas;

#endif


