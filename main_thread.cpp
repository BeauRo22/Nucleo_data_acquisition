#include "mbed.h"
#include "canPayloadCreator.hh"
#include "sharedWithCanPayloadCreator.hh"
 
#define NUM_THREADS                 14
#define THREAD_TIME_INTERVAL_SEC    2
Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led1(LED1);

Thread thread_CANManager;
Ticker dataTimer;

bool bTimerFlag = false;

int * pIdsToIdx;


void timerCallback(void) {
    bTimerFlag = true;
}

void addDataToBuffer(int sensorIdx, int * pData){
    led1 = !led1;    

    CPC->updateSensorData( sensorIdx, pData );    
}

/* main threads*/


#define SENSOR_1_ID 1
int main() {
    CPC = new canPayloadCreator();
    
    // initialize sensors and add a storage element to the data buffer
    //test, creating one sensor
    CPC->addSensor(SENSOR_1_ID);   
    
    CPC->Init();
    //  set up main processing loops
    thread_CANManager.start(thread_canPayloadCreator_main);

    dataTimer.attach(&timerCallback, SENSOR_ACQ_SECS);
    while(1)
    {
        if(bTimerFlag == true){
            static int count = 1;
            while(lock != AVAIL){}
            lock = LOCKED;
            addDataToBuffer( CPC->getSensorIdxFromSensorId(SENSOR_1_ID) , &count );
            lock = AVAIL;
            count++;
            bTimerFlag = false;    
        }
        sleep();
    }
}