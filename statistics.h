#ifndef __statistics_h
#define __statistics_h

#include "Arduino.h"

#define EARTH_GRAVITY   9.80665
//#define SCALE_MULTIPLIER    0.004

class statistics {
    public:
        statistics(double scaleMultiplier=1.0,double sigmaIter=1.0,int sumx=0,int sumy=0,int sumz=0);
		double xyztomod(int,int,int);
		double getModule();
		double getModule(int,int,int);
		//int getCalibratedModule();
		//int getCalibratedModule(int,int,int);
		//int getCalibratedModuleMCU();
		//int getCalibratedModuleMCU(int,int,int);
		//int getCalibratedModuleMCUEMA(float);
		//int getCalibratedModuleMCUEMA(float,int,int,int);
		//void calibration();
		//void MCUCalibration();
		void setXYZ(int,int,int);
		unsigned int getStatProbeSpeed();
		double getQuakeThreshold();
		double getCurrentAVG();
		double getCurrentSTDDEV();
		double getModuleEMA(double a);
		void setSigmaIter(double);
		double getSigmaIter();
		void resetLastPeriod();
		void addValueToAvgVar(double);
		void addValueToAvgVar(int, int, int );
		void calcMCU(int cx, int cy, int cz);
		float* getMCU( float *m);
		//void resetMCU();
		//void setFactor(double f);
     private:
        uint8_t devAddr;
		//int16_t IX,IY,IZ,AcX,AcY,AcZ,Tmp, x, y, z;
		double x, y, z;
		//double iX, iY, iZ, X, Y, Z;
		double ex,ey,ez;
		float MCUx,MCUy,MCUz;
		double partialAvg;
	    double partialStdDev;
	    unsigned int elements, n;
	    double quakeThreshold;
	    double sigmaIter;
	    double scalef; // 0.20 / 32768.0; // Assuming +/- 16G.
	    int sumx,sumy,sumz;
	    //double EARTH_GRAVITY_MS2 = 
	    //double factor;
};
#endif


