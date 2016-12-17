// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "seismometer.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
//#include <SoftwareSerial.h>
#define BUFLEN 100
MPU6050 accelero;
//float a[3];
//statistics stat(0.20 / 32768.0); //scale factor
statistics stat((double)2.0/ 32768.0,5.0); //scale factor

double getCurrentAVG(){
	return stat.getCurrentAVG();
}

double getCurrentSTDDEV(){
	return stat.getCurrentSTDDEV();
}

void resetStat(){
stat.resetLastPeriod();
calibrate(0.01);  	
}

void seismometerInit() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
   #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
   #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
   #endif

  // initialize device
  //Serial.println("Initializing I2C devices...");
  accelero.initialize();
  // verify connection
  //Serial.println("Testing accel connections...");
  Serial.println(accelero.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  
  accelero.setFullScaleAccelRange(0); //+-2G
    //Serial.print("\n");
    //accelero.setXAccelOffset(-2380);
    //accelero.setYAccelOffset(500);
    //accelero.setZAccelOffset(1810);
    accelero.resetSensors();
	accelero.resetFIFO();
    stat.resetLastPeriod();
    accelero.setXAccelOffset(0);
    accelero.setYAccelOffset(0);
    accelero.setZAccelOffset(0);
    
    /*for(unsigned int i=0; i<100; i++){
    	stat.calcMCU(accelero.getAccelerationX(),accelero.getAccelerationY(),accelero.getAccelerationZ());
    	//delay(500);
	}
	accelero.setXAccelOffset(accelero.getXAccelOffset()-stat.getMCU(a)[0]);
    accelero.setYAccelOffset(accelero.getYAccelOffset()-stat.getMCU(a)[1]);
    accelero.setZAccelOffset(accelero.getZAccelOffset()-stat.getMCU(a)[2]);
    */
    calibrate(0.01);
	/*
    Serial.print(accelero.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelero.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelero.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.println();
    */
    //delay(10000);
}

void seismometerTick() {
	RECORD db = {0, 0, false};
	
    stat.setSigmaIter(getSigma());
	//db.ts = getUNIXTime();
	
	stat.setXYZ(accelero.getAccelerationX(),accelero.getAccelerationY(),accelero.getAccelerationZ());
	db.accel = stat.getModule();
	db.overThreshold = stat.getModuleEMA(0.6) > stat.getQuakeThreshold();
    stat.addValueToAvgVar(db.accel);
    //logRequest("New Event: v:%lf - thr:%f - iter:%f - avg:%f - stddev:%f", db.accel, stat.quakeThreshold, stat.getSigmaIter(), stat.getCurrentAVG(), stat.getCurrentSTDDEV());
	//Serial.println(accelero.getAccelerationY(),DEC);
	//Serial.println(accelero.getAccelerationZ(),DEC);
	if(db.overThreshold) {
		LED::red(true);
		// QUAKE
		//create log message
		char msg[BUFLEN];
		char out[9];
		ftoa(out,9,db.accel);
		snprintf(msg,BUFLEN, "New Event: v:%s", out);
		ftoa(out,9,stat.getQuakeThreshold());
		//Serial.println(F("QUAKE2"));
		snprintf(msg,BUFLEN, "%s - thr:%s", msg, out);
		//Serial.println(F("QUAKE3"));
		ftoa(out,9,stat.getSigmaIter());
		//Serial.println(F("QUAKE4"));
		snprintf(msg,BUFLEN, "%s - iter:%s", msg, out);
	    //Serial.println(F("QUAKE5"));
		ftoa(out,9,stat.getCurrentAVG());
		//Serial.println(F("QUAKE6"));
		snprintf(msg,BUFLEN, "%s - avg:%s", msg, out);
		//Serial.println(F("QUAKE7"));
		ftoa(out,9,stat.getCurrentSTDDEV());
		//Serial.println(F("QUAKE8"));
		snprintf(msg,BUFLEN, "%s - stddev:%s", msg, out);
		Serial.println(msg);
		httpQuakeRequest();
        Serial.println(F("logRequest"));
	    logRequest(msg);
        Serial.println(F("resetSensors"));
		accelero.resetSensors();
		accelero.resetFIFO();
		Serial.println(F("resetStats"));
		stat.resetLastPeriod();  
		Serial.println(F("Delay 5 sec"));
		delay(5000);
		//la calibrazione permette la riconfigurazione se si cambia la posizione di ancoraggio del dispositivo (rotazione)
		calibrate(0.01); //chiamate successive alla prima convergono piÃ¹ rapidamente se non si azzera l'offset
		Serial.println(F("QUAKE END"));
		LED::red(false);
	}
}
	
void calibrate(float iter){
	int16_t ax, ay, az;
    int16_t off_ax, off_ay, off_az;
    delay(200);
    Serial.println(F("Start calibration"));
    stat.xyztomod(ax,ay,az);
   do{
    	//get accelerations
    	ax=accelero.getAccelerationX();
    	ay=accelero.getAccelerationY();
    	az=accelero.getAccelerationZ();
    	//get offsets
    	off_ax = accelero.getXAccelOffset();
    	off_ay = accelero.getYAccelOffset();
    	off_az = accelero.getZAccelOffset();
    	//check offsets 
    	/*Serial.print(ax);
    	Serial.print(":");
    	Serial.print(ay);
    	Serial.print(":");
    	Serial.print(az);
    	Serial.print(":");
    	Serial.println(stat.xyztomod(ax,ay,az));
    	*/
    	if (az > 0) off_az--; else if (az < 0) off_az++;
    	if (ay > 0) off_ay--; else if (ay < 0) off_ay++;
    	if (ax > 0) off_ax--; else if (ax < 0) off_ax++;
    	//set partial offsets
    	accelero.setXAccelOffset(off_ax);
    	accelero.setYAccelOffset(off_ay);
    	accelero.setZAccelOffset(off_az);
    	//delay(2);
	}while(stat.xyztomod(ax,ay,az)>iter);
	Serial.println(F("Calibration done"));
}




