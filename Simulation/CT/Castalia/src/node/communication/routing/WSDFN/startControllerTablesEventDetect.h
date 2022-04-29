
#ifndef _START_CONTROLLER_TABLES_EVENT_DETECT_H_
#define _START_CONTROLLER_TABLES_EVENT_DETECT_H_

#include "Structs.h"

struct timerEntry timerTableC[SIZEOF_TIMER_TABLE][MAXNODES];
struct sensorEntry sensorTableC[SIZEOF_SENSOR_TABLE][MAXNODES];
struct actionEntry actionTableC[SIZEOF_ACTION_TABLE][MAXNODES];
struct flowEntry flowTableC[SIZEOF_FLOW_TABLE][MAXNODES]; 
int lastNode;
int programPackages[MAXNODES];
int programmedPackages[MAXNODES];

/**
 * Initializes tables so that network works as data collector
 *
 */
void WSDFN::startTables()
{
	lastNode=4;
	if(this->isController){
		trace() << "Starting Controller Tables for " << SELF_NETWORK_ADDRESS;
		//timer 0: 2 minutes for preparate report temperature in all sensors
		timerTable[0].timerID=0;
		timerTable[0].actionID=0;
		timerTable[0].t0=8.9; //inicial time  almost 9 seconds
		timerTable[0].time=2.0; //repeat in 2 minutes
		timerTable[0].repeat=255; //repeat indefinitely
		timerTable[0].multiply=60;
		timerTable[0].active=true;
		
		//timer 1: +30 seconds and 2 minutes for deactivate  all sensors
		timerTable[1].timerID=1;
		timerTable[1].actionID=1;
		timerTable[1].t0=9.0+30.0; //inicial time 39 seconds (9 +30 seconds)
		timerTable[1].time=2.0; //repeat in 2 minutes
		timerTable[1].repeat=255; //repeat indefinitely
		timerTable[1].multiply=60;
		timerTable[1].active=true;

		//timer 2: 5 minutes for read temperature in all sensors
		timerTable[2].timerID=2;
		timerTable[2].actionID=2;
		timerTable[2].t0=9.0; //inicial time 9 seconds
		timerTable[2].time=2.0; //repeat in 2 minutes
		timerTable[2].repeat=255; //repeat indefinitely
		timerTable[2].multiply=60;
		timerTable[2].active=true;


		for(int j=3; j< SIZEOF_TIMER_TABLE;j++) timerTable[j].active=false;

		//action 0=Activate Radio
		actionTable[0].actionID=0;
		actionTable[0].actionType=A_RECEIVER_ON;
		actionTable[0].active=true;
		//action 1=Deactivate Radio
		actionTable[1].actionID=1;
		actionTable[1].actionType=A_RECEIVER_OFF;
		actionTable[1].active=true;
		//action 2=Read Sensor
		actionTable[2].actionID=2;
		actionTable[2].actionType=A_READ_SENSOR;
		actionTable[2].value1.valueInt16_1=0;
		actionTable[2].active=true;


		//action 3=send temperature read to nexthop
		actionTable[3].actionID=3;
		actionTable[3].actionType=A_SEND_TO_SINK;
		actionTable[3].active=true;
		actionTable[3].value1.valueInt16_1=3;




		// Flow for read Sensor
		flowTable[0].flowID=0;
		flowTable[0].actionID=3;
		flowTable[0].timeout=0;
		flowTable[0].counters=0;
		flowTable[0].active=true;
		//source is the node itself, so it is a read sensor data
		flowTable[0].fa.sourceAddress=atoi(SELF_NETWORK_ADDRESS); 
		flowTable[0].fa.sourceMask=0xFFFF; //source address mask 1bit
		flowTable[0].fa.destinationAddress=-1;
		flowTable[0].fa.destinationMask=0xFFFF;		
		//case temperature>90 celsius
		flowTable[0].fc.dataType=INT_8;
		flowTable[0].fc.operatorC=GT;
		flowTable[0].fc.value.valueInt8_1=90;


		//Flow send to Sink
		flowTable[1].flowID=0;
		flowTable[1].actionID=3;
		flowTable[1].timeout=0;
		flowTable[1].counters=0;
		flowTable[1].active=true;
		flowTable[1].fa.sourceAddress=0; 
		flowTable[1].fa.sourceMask=0; 
		flowTable[1].fa.destinationAddress=sink;
		flowTable[1].fa.destinationMask=0xFFFF;
		flowTable[1].fc.operatorC=NONE;
		for(int j=2; j< SIZEOF_FLOW_TABLE;j++) flowTable[j].active = false;


		for(int j=4; j< SIZEOF_ACTION_TABLE;j++) actionTable[j].active = false;

		//schedule defined Timers 
		scheduleTimer(0);
		scheduleTimer(1);



		for(int i=0;i<lastNode; i++){
			//for all nodes

			programPackages[i]=0;
			programmedPackages[i]=0;
			reportPackages[i]=0;
			reportedPackages[i]=0;

			//timer 0: 2 minutes for preparate report temperature in all sensors
			timerTableC[0][i].timerID=0;
			timerTableC[0][i].actionID=0;
			timerTableC[0][i].t0=8.9; //inicial time  almost 9 seconds
			timerTableC[0][i].time=2.0; //repeat in 2 minutes
			timerTableC[0][i].repeat=255; //repeat indefinitely
			timerTableC[0][i].multiply=60;
			timerTableC[0][i].active=true;
			//timer 1: +30 seconds and 2 minutes for deactivate  all sensors
			timerTableC[1][i].timerID=1;
			timerTableC[1][i].actionID=1;
			timerTableC[1][i].t0=9.0+30.0; //inicial time 39 seconds (9 +30 seconds)
			timerTableC[1][i].time=2.0; //repeat in 2 minutes
			timerTableC[1][i].repeat=255; //repeat indefinitely
			timerTableC[1][i].multiply=60;
			timerTableC[1][i].active=true;
			//action 0=Activate Radio
			actionTableC[0][i].actionID=0;
			actionTableC[0][i].actionType=A_RECEIVER_ON;
			actionTableC[0][i].active=true;
			//action 1=Deactivate Radio
			actionTableC[1][i].actionID=1;
			actionTableC[1][i].actionType=A_RECEIVER_OFF;
			actionTableC[1][i].active=true;
		

	

			//timer 2: 5 minutes for read temperature in all sensors
			timerTableC[2][i].timerID=2;
			timerTableC[2][i].actionID=2;
			timerTableC[2][i].t0=9.0; //inicial time 9 seconds
			timerTableC[2][i].time=2.0; //repeat in 2 minutes
			timerTableC[2][i].repeat=255; //repeat indefinitely
			timerTableC[2][i].multiply=60;
			timerTableC[2][i].active=true;

			// sink not read the sensor
			if(i==0) 			timerTableC[2][i].active=false;


			for(int j=3; j< SIZEOF_TIMER_TABLE;j++) timerTableC[j][i].active=false;
			//action 2=Read Sensor
			actionTableC[2][i].actionID=2;
			actionTableC[2][i].actionType=A_READ_SENSOR;
			actionTableC[2][i].value1.valueInt16_1=0;
			actionTableC[2][i].active=true;
	
			//action 3=send temperature read to nexthop
			actionTableC[3][i].actionID=3;
			actionTableC[3][i].actionType=A_SEND_TO_SINK;
			actionTableC[3][i].active=true;
			//for each node the nexthop is different

			stringstream out; out << i; string SNA = out.str();
			//string SNA=itos(i);
			if(i==4)
				actionTableC[3][i].value1.valueInt16_1=3;
			if(i==3)
				actionTableC[3][i].value1.valueInt16_1=0;
			if(i==2)
				actionTableC[3][i].value1.valueInt16_1=1;
			if(i==1)
				actionTableC[3][i].value1.valueInt16_1=0;			

			//action 4=Display Sensor
			actionTableC[4][i].actionID=4;
			actionTableC[4][i].actionType=A_DISPLAY;
			actionTableC[4][i].active=true;	


			for(int j=5; j< SIZEOF_ACTION_TABLE;j++) actionTableC[j][i].active = false;
	
			// Flow for read Sensor
			flowTableC[0][i].flowID=0;
			flowTableC[0][i].actionID=3;
//			flowTableC[0].priority=1;
			flowTableC[0][i].timeout=0;
			flowTableC[0][i].counters=0;
			flowTableC[0][i].active=true;
			//source is the node itself, so it is a read sensor data
			flowTableC[0][i].fa.sourceAddress=i; 
			flowTableC[0][i].fa.sourceMask=0xFFFF; //source address mask 1bit
			flowTableC[0][i].fa.destinationAddress=-1;
			flowTableC[0][i].fa.destinationMask=0xFFFF;	
	
			//case temperature>90 celsius
			flowTableC[0][i].fc.dataType=INT_8;
			flowTableC[0][i].fc.operatorC=GT;
			flowTableC[0][i].fc.value.valueInt8_1=90;
	
			//Flow send to Sink or display
			flowTableC[1][i].flowID=1;
			//resend to sink or display 
			if(i==sink) 	flowTableC[1][i].actionID=4;
			else   			    flowTableC[1][i].actionID=3;
			trace()<< "Teste preenchendo tables de i: "<< i << ", action: "<<flowTableC[1][i].actionID;
			flowTableC[1][i].timeout=0;
			flowTableC[1][i].counters=0;
			flowTableC[1][i].active=true;
			flowTableC[1][i].fa.sourceAddress=0; 
			flowTableC[1][i].fa.sourceMask=0; 
			flowTableC[1][i].fa.destinationAddress=sink;
			flowTableC[1][i].fa.destinationMask=0xFFFF;
			flowTableC[1][i].fc.operatorC=NONE;
	
			for(int j=2; j< SIZEOF_FLOW_TABLE;j++) flowTableC[j][i].active = false;
			
			//schedule defined Timers 
			//scheduleTimer(0);
			//scheduleTimer(1);
			//scheduleTimer(2);
		}
		
		//} else {
		//Only for Controller


	};

	if(!this->isSink){
			//for all nodes except Sink
		//all sensor have a temperture sensor.
		sensorTable[0].sensorID=0;
		sensorTable[0].sensorType= TEMPERATURE;
		sensorTable[0].sensorType= CELSIUS;
		sensorTable[0].dataType= INT_8;
		sensorTable[0].interfaceType= ANALOGIC;
		sensorTable[0].interfaceNumber1 = 10;
		sensorTable[0].active=true;
		for(int j=1; j< SIZEOF_SENSOR_TABLE;j++) sensorTable[j].active = false;
	}	

}


#endif				//_START_CONTROLLER_TABLES_EVENT_DETECT_H_
