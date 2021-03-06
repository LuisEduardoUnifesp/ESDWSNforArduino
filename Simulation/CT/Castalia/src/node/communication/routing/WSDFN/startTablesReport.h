
#ifndef _START_TABLES_REPORT_H_
#define _START_TABLES_REPORT_H_

#include "Structs.h"

/**
 * Initializes tables so that network works as data collector
 *
 */
void WSDFN::startTables()
{
	trace() << "Starting Tables for " << SELF_NETWORK_ADDRESS;
	//for all nodes, Controller inclusive
	sink=0;

	//timer 0: 2 minutes for preparate report temperature in all sensors
	timerTable[0].timerID=0;
	timerTable[0].actionID=0;
	timerTable[0].t0=8.9; //inicial time  almost 9 seconds
	timerTable[0].time=2; //repeat in 2 minutes
	timerTable[0].repeat=255; //repeat indefinitely
	timerTable[0].multiply=60;
	//timer 1: +30 seconds and 2 minutes for deactivate  all sensors
	timerTable[1].timerID=1;
	timerTable[1].actionID=1;
	timerTable[1].t0=9+30; //inicial time 39 seconds (9 +30 seconds)
	timerTable[1].time=2; //repeat in 2 minutes
	timerTable[1].repeat=255; //repeat indefinitely
	timerTable[1].multiply=60;
	//action 0=Activate Radio
	actionTable[0].actionID=0;
	actionTable[0].actionType=A_RECEIVER_ON;
	//action 1=Deactivate Radio
	actionTable[1].actionID=1;
	actionTable[1].actionType=A_RECEIVER_OFF;
	
	if(!this->isController){
		//for all nodes except Controller

		//all sensor have a temperture sensor.
		sensorTable[0].sensorID=0;
		sensorTable[0].sensorType= TEMPERATURE;
		sensorTable[0].sensorType= CELSIUS;
		sensorTable[0].dataType= INT_8;
		sensorTable[0].interfaceType= ANALOGIC;
		sensorTable[0].interfaceNumber1 = 10;


		//timer 2: 5 minutes for read temperature in all sensors
		timerTable[2].timerID=2;
		timerTable[2].actionID=2;
		timerTable[2].t0=9; //inicial time 9 seconds
		timerTable[2].time=2; //repeat in 2 minutes
		timerTable[2].repeat=255; //repeat indefinitely
		timerTable[2].multiply=60;
		//action 2=Read Sensor
		actionTable[2].actionID=2;
		actionTable[2].actionType=A_READ_SENSOR;
		actionTable[2].value1.valueInt16_1=0;

		//action 3=send temperature read to nexthop
		actionTable[3].actionID=3;
		actionTable[3].actionType=A_SEND_TO_SINK;
		//for each node the nexthop is different
		string SNA=SELF_NETWORK_ADDRESS;
		if(SNA.compare("4") == 0)
			actionTable[3].value1.valueInt16_1=3;
		if(SNA.compare("3") == 0)
			actionTable[3].value1.valueInt16_1=2;
		if(SNA.compare("2") == 0)
			actionTable[3].value1.valueInt16_1=1;
		if(SNA.compare("1") == 0)
			actionTable[3].value1.valueInt16_1=0;

		// Flow for read Sensor
		flowTable[0].flowID=0;
		flowTable[0].actionID=3;
//		flowTable[0].priority=1;
		flowTable[0].timeout=0;
		flowTable[0].counters=0;
		flowTable[0].active=true;
		//source is the node itself, so it is a read sensor data
		flowTable[0].fa.sourceAddress=atoi(SELF_NETWORK_ADDRESS); 
		flowTable[0].fa.sourceMask=0xFFFF; //source address mask 1bit
		flowTable[0].fa.destinationAddress=-1;
		flowTable[0].fa.destinationMask=0xFFFF;		
		flowTable[0].fc.operatorC=NONE;

		//Flow send to Sink
		flowTable[1].flowID=1;
		flowTable[1].actionID=3;
//		flowTable[1].priority=1;
		flowTable[1].timeout=0;
		flowTable[1].counters=0;
		flowTable[1].active=true;
		flowTable[1].fa.sourceAddress=0; 
		flowTable[1].fa.sourceMask=0; 
		flowTable[1].fa.destinationAddress=sink;
		flowTable[1].fa.destinationMask=0xFFFF;
		flowTable[1].fc.operatorC=NONE;

		for(int j=2; j< SIZEOF_FLOW_TABLE;j++) flowTable[j].active = false;
		
		//schedule defined Timers 
		scheduleTimer(0);
		scheduleTimer(1);
		scheduleTimer(2);
		
	} else {
		//Only for Controller

		//Flow send to Sink
		flowTable[0].flowID=0;
		flowTable[0].actionID=2;
//		flowTable[0].priority=1;
		flowTable[0].timeout=0;
		flowTable[0].counters=0;
		flowTable[0].active=true;
		flowTable[0].fa.sourceAddress=0; 
		flowTable[0].fa.sourceMask=0; 
		flowTable[0].fa.destinationAddress=sink;
		flowTable[0].fa.destinationMask=0xFFFF;
		flowTable[0].fc.operatorC=NONE;
		for(int j=1; j< SIZEOF_FLOW_TABLE;j++) flowTable[j].active = false;

		//action 4=Display Sensor
		actionTable[2].actionID=2;
		actionTable[2].actionType=A_DISPLAY;

		//schedule defined Timers 
		scheduleTimer(0);
		scheduleTimer(1);

	}


}


#endif				//_START_TABLES_REPORT_H_
