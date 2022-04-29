
#ifndef _START_CONTROLLER_TABLES_REPORT_H_
#define _START_CONTROLLER_TABLES_REPORT_H_

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
void WSDFN::startTables(int n)
{
	double interval;
	this->spaceDelay = par("spaceDelay");
	this->dataTime = par("dataTime").doubleValue();	
	lastNode=n;
	if(this->isController){
		#ifdef DEBUGFLOW
		trace() << "Starting Controller Tables for " << SELF_NETWORK_ADDRESS;
		#endif
		//timer 0: 2 minutes for preparate report temperature in all sensors
		timerTable[0].timerID=0;
		timerTable[0].actionID=0;
		timerTable[0].t0=0.97; //inicial time  almost 9 seconds
		timerTable[0].time=2.0; //repeat in 2 minutes
		timerTable[0].repeat=255; //repeat indefinitely
		timerTable[0].multiply=60;
		timerTable[0].active=true;
		
		//timer 1: +30 seconds and 2 minutes for deactivate  all sensors
		timerTable[1].timerID=1;
		timerTable[1].actionID=1;
		timerTable[1].t0=1.0+dataTime; //inicial time 39 seconds (9 +30 seconds)
		timerTable[1].time=2.0; //repeat in 2 minutes
		timerTable[1].repeat=255; //repeat indefinitely
		timerTable[1].multiply=60;
		timerTable[1].active=true;

		//timer 2: 5 minutes for read temperature in all sensors
		timerTable[2].timerID=2;
		timerTable[2].actionID=2;


		//Timer of sersor with sequencial delay
			timerTable[2].t0=1.0+((0.0+spaceDelay)*atoi(SELF_NETWORK_ADDRESS)/1000);
						

//		interval = rand() % 100;
//		trace() << "Sensor Delay: " << interval << " != " << 100;
//		timerTable[2].t0=1.0+ ( (double)interval / 1000 ); //inicial time 9 seconds + 0 a 100 miliseconds for avoid collision


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
		//TODO Adjust the nexthop

		if(n==81)
			actionTable[3].value1.valueInt16_1=32;
		if(n==64)
			actionTable[3].value1.valueInt16_1=20; 
		if(n==49)
			actionTable[3].value1.valueInt16_1=18; 
		if(n==36)
			actionTable[3].value1.valueInt16_1=9; 
		if(n==25)
			actionTable[3].value1.valueInt16_1=8; 
		if(n==16)
			actionTable[3].value1.valueInt16_1=0; 



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
		flowTable[0].fc.operatorC=NONE;

		//Flow send to Sink
		flowTable[1].flowID=1;
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
//		scheduleTimer(0);
//		scheduleTimer(1);
 


		for(int i=0;i<lastNode; i++){
			//if(i==atoi(controller.c_str())) continue;
			//for all nodes except controller

			programPackages[i]=0;
			programmedPackages[i]=0;

			//timer 0: 2 minutes for preparate report temperature in all sensors
			timerTableC[0][i].timerID=0;
			timerTableC[0][i].actionID=0;
			timerTableC[0][i].t0=0.97; //inicial time  almost 9 seconds
			timerTableC[0][i].time=2.0; //repeat in 2 minutes
			timerTableC[0][i].repeat=255; //repeat indefinitely
			timerTableC[0][i].multiply=60;
			timerTableC[0][i].active=true;
			//timer 1: +30 seconds and 2 minutes for deactivate  all sensors
			timerTableC[1][i].timerID=1;
			timerTableC[1][i].actionID=1;
			timerTableC[1][i].t0=1.0+dataTime; //inicial time 39 seconds (9 +30 seconds)
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

//Timer of sersor with sequencial delay
			timerTableC[2][i].t0=1.0+((0.0+spaceDelay)*i/1000);
			#ifdef DEBUGFLOW
			trace() << "Sensor Delay: " <<  (100*timerTableC[2][i].t0);
			#endif

//Timer of sensor with randomic delay
//			interval = rand() % 300;
//			trace() << "Sensor Delay: " << interval << " != " << 300;
//			timerTableC[2][i].t0=1.0+ ( (double)interval / 1000 ); //inicial time 9 seconds + 0 a 100 miliseconds for avoid collision
			
//Timer of sensor without delay
//			timerTableC[2][i].t0=9.0; //inicial time 9 seconds

			timerTableC[2][i].time=2.0; //repeat in 2 minutes
			timerTableC[2][i].repeat=255; //repeat indefinitely
			timerTableC[2][i].multiply=60;
			timerTableC[2][i].active=true;
			for(int j=3; j< SIZEOF_TIMER_TABLE;j++) timerTableC[j][i].active=false;

			//The sink not read your sensor
			if(i==0){
				timerTableC[2][i].active=false;
			}
		
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

			stringstream out; out << i; string SNA = out.str(); //TODO ADJUST the nexthop

			if(n==16){
				if(i==15)
					actionTableC[3][i].value1.valueInt16_1=11;
				if(i==14)
					actionTableC[3][i].value1.valueInt16_1=10;
				if(i==13)
					actionTableC[3][i].value1.valueInt16_1=9;
				if(i==12)
					actionTableC[3][i].value1.valueInt16_1=8;		
				if(i==11)
					actionTableC[3][i].value1.valueInt16_1=7;
				if(i==10)
					actionTableC[3][i].value1.valueInt16_1=6;		
				if(i==9)
					actionTableC[3][i].value1.valueInt16_1=4;
				if(i==8)
					actionTableC[3][i].value1.valueInt16_1=5;
				if(i==7)
					actionTableC[3][i].value1.valueInt16_1=3;
				if(i==6)
					actionTableC[3][i].value1.valueInt16_1=4;		
				// 4 is always the controller
				if(i==5)
					actionTableC[3][i].value1.valueInt16_1=4;
				if(i==3)
					actionTableC[3][i].value1.valueInt16_1=2;
				if(i==2)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==1)
					actionTableC[3][i].value1.valueInt16_1=0;		
			}


			if(n==25){
				if(i==24)
					actionTableC[3][i].value1.valueInt16_1=23;
				if(i==23)
					actionTableC[3][i].value1.valueInt16_1=22;
				if(i==22)
					actionTableC[3][i].value1.valueInt16_1=17;		
				if(i==21)
					actionTableC[3][i].value1.valueInt16_1=16;
				if(i==20)
					actionTableC[3][i].value1.valueInt16_1=15;		
				if(i==19)
					actionTableC[3][i].value1.valueInt16_1=18;
				if(i==18)
					actionTableC[3][i].value1.valueInt16_1=13;
				if(i==17)
					actionTableC[3][i].value1.valueInt16_1=4;
				if(i==16)
					actionTableC[3][i].value1.valueInt16_1=17;	

				if(i==15)
					actionTableC[3][i].value1.valueInt16_1=11;
				if(i==14)
					actionTableC[3][i].value1.valueInt16_1=13;
				if(i==13)
					actionTableC[3][i].value1.valueInt16_1=9;
				if(i==12)
					actionTableC[3][i].value1.valueInt16_1=7;		
				if(i==11)
					actionTableC[3][i].value1.valueInt16_1=12;
				if(i==10)
					actionTableC[3][i].value1.valueInt16_1=9;		
				if(i==9)
					actionTableC[3][i].value1.valueInt16_1=3;
				if(i==8)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==7)
					actionTableC[3][i].value1.valueInt16_1=2;
				if(i==6)
					actionTableC[3][i].value1.valueInt16_1=7;		
				// 4 is always the controller
				if(i==5)
					actionTableC[3][i].value1.valueInt16_1=3;
				if(i==3)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==2)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==1)
					actionTableC[3][i].value1.valueInt16_1=2;		
			}

			if(n==36){
				if(i==35)
					actionTableC[3][i].value1.valueInt16_1=34;
				if(i==34)
					actionTableC[3][i].value1.valueInt16_1=33;
				if(i==33)
					actionTableC[3][i].value1.valueInt16_1=32;
				if(i==32)
					actionTableC[3][i].value1.valueInt16_1=26;		
				if(i==31)
					actionTableC[3][i].value1.valueInt16_1=25;
				if(i==30)
					actionTableC[3][i].value1.valueInt16_1=24;		
				if(i==29)
					actionTableC[3][i].value1.valueInt16_1=28;
				if(i==28)
					actionTableC[3][i].value1.valueInt16_1=27;
				if(i==27)
					actionTableC[3][i].value1.valueInt16_1=26;
				if(i==26)
					actionTableC[3][i].value1.valueInt16_1=20;		
				if(i==25)
					actionTableC[3][i].value1.valueInt16_1=19;	
				if(i==24)
					actionTableC[3][i].value1.valueInt16_1=18;
				if(i==23)
					actionTableC[3][i].value1.valueInt16_1=22;
				if(i==22)
					actionTableC[3][i].value1.valueInt16_1=21;		
				if(i==21)
					actionTableC[3][i].value1.valueInt16_1=15;
				if(i==20)
					actionTableC[3][i].value1.valueInt16_1=4;		
				if(i==19)
					actionTableC[3][i].value1.valueInt16_1=14;
				if(i==18)
					actionTableC[3][i].value1.valueInt16_1=13;
				if(i==17)
					actionTableC[3][i].value1.valueInt16_1=16;
				if(i==16)
					actionTableC[3][i].value1.valueInt16_1=15;	
				if(i==15)
					actionTableC[3][i].value1.valueInt16_1=10;
				if(i==14)
					actionTableC[3][i].value1.valueInt16_1=8;
				if(i==13)
					actionTableC[3][i].value1.valueInt16_1=7;
				if(i==12)
					actionTableC[3][i].value1.valueInt16_1=11;		
				if(i==11)
					actionTableC[3][i].value1.valueInt16_1=10;
				if(i==10)
					actionTableC[3][i].value1.valueInt16_1=3;		
				if(i==9)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==8)
					actionTableC[3][i].value1.valueInt16_1=2;
				if(i==7)
					actionTableC[3][i].value1.valueInt16_1=1;
				if(i==6)
					actionTableC[3][i].value1.valueInt16_1=5;		
				// 4 is always the controller
				if(i==5)
					actionTableC[3][i].value1.valueInt16_1=3;
				if(i==3)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==2)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==1)
					actionTableC[3][i].value1.valueInt16_1=2;		
			}

			if(n==49){
				if(i==48)
					actionTableC[3][i].value1.valueInt16_1=47;
				if(i==47)
					actionTableC[3][i].value1.valueInt16_1=46;
				if(i==46)
					actionTableC[3][i].value1.valueInt16_1=45;	
				if(i==45)
					actionTableC[3][i].value1.valueInt16_1=38;
				if(i==44)
					actionTableC[3][i].value1.valueInt16_1=45;
				if(i==43)
					actionTableC[3][i].value1.valueInt16_1=44;
				if(i==42)
					actionTableC[3][i].value1.valueInt16_1=43;		
				if(i==41)
					actionTableC[3][i].value1.valueInt16_1=34;
				if(i==40)
					actionTableC[3][i].value1.valueInt16_1=39;		
				if(i==39)
					actionTableC[3][i].value1.valueInt16_1=38;
				if(i==38)
					actionTableC[3][i].value1.valueInt16_1=31;
				if(i==37)
					actionTableC[3][i].value1.valueInt16_1=38;
				if(i==36)
					actionTableC[3][i].value1.valueInt16_1=37;	
				if(i==35)
					actionTableC[3][i].value1.valueInt16_1=28;
				if(i==34)
					actionTableC[3][i].value1.valueInt16_1=27;
				if(i==33)
					actionTableC[3][i].value1.valueInt16_1=26;
				if(i==32)
					actionTableC[3][i].value1.valueInt16_1=25;		
				if(i==31)
					actionTableC[3][i].value1.valueInt16_1=4;
				if(i==30)
					actionTableC[3][i].value1.valueInt16_1=24;		
				if(i==29)
					actionTableC[3][i].value1.valueInt16_1=23;
				if(i==28)
					actionTableC[3][i].value1.valueInt16_1=22;
				if(i==27)
					actionTableC[3][i].value1.valueInt16_1=21;
				if(i==26)
					actionTableC[3][i].value1.valueInt16_1=20;		
				if(i==25)
					actionTableC[3][i].value1.valueInt16_1=19;	
				if(i==24)
					actionTableC[3][i].value1.valueInt16_1=17;
				if(i==23)
					actionTableC[3][i].value1.valueInt16_1=16;
				if(i==22)
					actionTableC[3][i].value1.valueInt16_1=15;		
				if(i==21)
					actionTableC[3][i].value1.valueInt16_1=14;
				if(i==20)
					actionTableC[3][i].value1.valueInt16_1=13;	
				if(i==19)
					actionTableC[3][i].value1.valueInt16_1=12;
				if(i==18)
					actionTableC[3][i].value1.valueInt16_1=11;
				if(i==17)
					actionTableC[3][i].value1.valueInt16_1=10;
				if(i==16)
					actionTableC[3][i].value1.valueInt16_1=9;	
				if(i==15)
					actionTableC[3][i].value1.valueInt16_1=8;
				if(i==14)
					actionTableC[3][i].value1.valueInt16_1=7;
				if(i==13)
					actionTableC[3][i].value1.valueInt16_1=6;
				if(i==12)
					actionTableC[3][i].value1.valueInt16_1=5;		
				if(i==11)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==10)
					actionTableC[3][i].value1.valueInt16_1=3;		
				if(i==9)
					actionTableC[3][i].value1.valueInt16_1=2;
				if(i==8)
					actionTableC[3][i].value1.valueInt16_1=1;
				if(i==7)
					actionTableC[3][i].value1.valueInt16_1=6;
				if(i==6)
					actionTableC[3][i].value1.valueInt16_1=5;		
				// 4 is always the controller
				if(i==5)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==3)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==2)
					actionTableC[3][i].value1.valueInt16_1=3;
				if(i==1)
					actionTableC[3][i].value1.valueInt16_1=2;		
			}


			if(n==64){
				if(i==63)
					actionTableC[3][i].value1.valueInt16_1=62;
				if(i==62)
					actionTableC[3][i].value1.valueInt16_1=61;		
				if(i==61)
					actionTableC[3][i].value1.valueInt16_1=60;
				if(i==60)
					actionTableC[3][i].value1.valueInt16_1=59;		
				if(i==59)
					actionTableC[3][i].value1.valueInt16_1=51;
				if(i==58)
					actionTableC[3][i].value1.valueInt16_1=59;
				if(i==57)
					actionTableC[3][i].value1.valueInt16_1=58;
				if(i==56)
					actionTableC[3][i].value1.valueInt16_1=48;	
				if(i==55)
					actionTableC[3][i].value1.valueInt16_1=54;
				if(i==54)
					actionTableC[3][i].value1.valueInt16_1=53;
				if(i==53)
					actionTableC[3][i].value1.valueInt16_1=52;
				if(i==52)
					actionTableC[3][i].value1.valueInt16_1=51;		
				if(i==51)
					actionTableC[3][i].value1.valueInt16_1=43;
				if(i==50)
					actionTableC[3][i].value1.valueInt16_1=51;		
				if(i==49)
					actionTableC[3][i].value1.valueInt16_1=41;
				if(i==48)
					actionTableC[3][i].value1.valueInt16_1=40;
				if(i==47)
					actionTableC[3][i].value1.valueInt16_1=39;
				if(i==46)
					actionTableC[3][i].value1.valueInt16_1=45;	
				if(i==45)
					actionTableC[3][i].value1.valueInt16_1=44;
				if(i==44)
					actionTableC[3][i].value1.valueInt16_1=43;
				if(i==43)
					actionTableC[3][i].value1.valueInt16_1=35;
				if(i==42)
					actionTableC[3][i].value1.valueInt16_1=34;		
				if(i==41)
					actionTableC[3][i].value1.valueInt16_1=33;
				if(i==40)
					actionTableC[3][i].value1.valueInt16_1=32;		
				if(i==39)
					actionTableC[3][i].value1.valueInt16_1=31;
				if(i==38)
					actionTableC[3][i].value1.valueInt16_1=30;
				if(i==37)
					actionTableC[3][i].value1.valueInt16_1=29;
				if(i==36)
					actionTableC[3][i].value1.valueInt16_1=28;	
				if(i==35)
					actionTableC[3][i].value1.valueInt16_1=4;
				if(i==34)
					actionTableC[3][i].value1.valueInt16_1=27;
				if(i==33)
					actionTableC[3][i].value1.valueInt16_1=26;
				if(i==32)
					actionTableC[3][i].value1.valueInt16_1=25;		
				if(i==31)
					actionTableC[3][i].value1.valueInt16_1=24;
				if(i==30)
					actionTableC[3][i].value1.valueInt16_1=23;		
				if(i==29)
					actionTableC[3][i].value1.valueInt16_1=22;
				if(i==28)
					actionTableC[3][i].value1.valueInt16_1=21;
				if(i==27)
					actionTableC[3][i].value1.valueInt16_1=19;
				if(i==26)
					actionTableC[3][i].value1.valueInt16_1=18;		
				if(i==25)
					actionTableC[3][i].value1.valueInt16_1=17;	
				if(i==24)
					actionTableC[3][i].value1.valueInt16_1=16;
				if(i==23)
					actionTableC[3][i].value1.valueInt16_1=15;
				if(i==22)
					actionTableC[3][i].value1.valueInt16_1=14;		
				if(i==21)
					actionTableC[3][i].value1.valueInt16_1=13;
				if(i==20)
					actionTableC[3][i].value1.valueInt16_1=12;	
				if(i==19)
					actionTableC[3][i].value1.valueInt16_1=11;
				if(i==18)
					actionTableC[3][i].value1.valueInt16_1=10;
				if(i==17)
					actionTableC[3][i].value1.valueInt16_1=9;
				if(i==16)
					actionTableC[3][i].value1.valueInt16_1=8;	
				if(i==15)
					actionTableC[3][i].value1.valueInt16_1=7;
				if(i==14)
					actionTableC[3][i].value1.valueInt16_1=6;
				if(i==13)
					actionTableC[3][i].value1.valueInt16_1=5;
				if(i==12)
					actionTableC[3][i].value1.valueInt16_1=0;		
				if(i==11)
					actionTableC[3][i].value1.valueInt16_1=3;
				if(i==10)
					actionTableC[3][i].value1.valueInt16_1=2;		
				if(i==9)
					actionTableC[3][i].value1.valueInt16_1=1;
				if(i==8)
					actionTableC[3][i].value1.valueInt16_1=7;
				if(i==7)
					actionTableC[3][i].value1.valueInt16_1=6;
				if(i==6)
					actionTableC[3][i].value1.valueInt16_1=5;		
				// 4 is always the controller
				if(i==5)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==3)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==2)
					actionTableC[3][i].value1.valueInt16_1=3;
				if(i==1)
					actionTableC[3][i].value1.valueInt16_1=2;		
			}

			if(n==81){
				if(i==80)
					actionTableC[3][i].value1.valueInt16_1=79;		
				if(i==79)
					actionTableC[3][i].value1.valueInt16_1=78;
				if(i==78)
					actionTableC[3][i].value1.valueInt16_1=69;
				if(i==77)
					actionTableC[3][i].value1.valueInt16_1=68;
				if(i==76)
					actionTableC[3][i].value1.valueInt16_1=67;	
				if(i==75)
					actionTableC[3][i].value1.valueInt16_1=76;
				if(i==74)
					actionTableC[3][i].value1.valueInt16_1=75;
				if(i==73)
					actionTableC[3][i].value1.valueInt16_1=74;
				if(i==72)
					actionTableC[3][i].value1.valueInt16_1=73;		
				if(i==71)
					actionTableC[3][i].value1.valueInt16_1=70;
				if(i==70)
					actionTableC[3][i].value1.valueInt16_1=69;		
				if(i==69)
					actionTableC[3][i].value1.valueInt16_1=68;
				if(i==68)
					actionTableC[3][i].value1.valueInt16_1=67;
				if(i==67)
					actionTableC[3][i].value1.valueInt16_1=58;
				if(i==66)
					actionTableC[3][i].value1.valueInt16_1=67;	
				if(i==65)
					actionTableC[3][i].value1.valueInt16_1=66;
				if(i==64)
					actionTableC[3][i].value1.valueInt16_1=65;

				if(i==63)
					actionTableC[3][i].value1.valueInt16_1=64;
				if(i==62)
					actionTableC[3][i].value1.valueInt16_1=53;		
				if(i==61)
					actionTableC[3][i].value1.valueInt16_1=52;
				if(i==60)
					actionTableC[3][i].value1.valueInt16_1=59;		
				if(i==59)
					actionTableC[3][i].value1.valueInt16_1=58;
				if(i==58)
					actionTableC[3][i].value1.valueInt16_1=49;
				if(i==57)
					actionTableC[3][i].value1.valueInt16_1=58;
				if(i==56)
					actionTableC[3][i].value1.valueInt16_1=57;	
				if(i==55)
					actionTableC[3][i].value1.valueInt16_1=46;
				if(i==54)
					actionTableC[3][i].value1.valueInt16_1=45;
				if(i==53)
					actionTableC[3][i].value1.valueInt16_1=44;
				if(i==52)
					actionTableC[3][i].value1.valueInt16_1=43;		
				if(i==51)
					actionTableC[3][i].value1.valueInt16_1=42;
				if(i==50)
					actionTableC[3][i].value1.valueInt16_1=41;		
				if(i==49)
					actionTableC[3][i].value1.valueInt16_1=4;
				if(i==48)
					actionTableC[3][i].value1.valueInt16_1=40;
				if(i==47)
					actionTableC[3][i].value1.valueInt16_1=39;
				if(i==46)
					actionTableC[3][i].value1.valueInt16_1=38;	
				if(i==45)
					actionTableC[3][i].value1.valueInt16_1=37;
				if(i==44)
					actionTableC[3][i].value1.valueInt16_1=36;
				if(i==43)
					actionTableC[3][i].value1.valueInt16_1=35;
				if(i==42)
					actionTableC[3][i].value1.valueInt16_1=34;		
				if(i==41)
					actionTableC[3][i].value1.valueInt16_1=33;
				if(i==40)
					actionTableC[3][i].value1.valueInt16_1=31;		
				if(i==39)
					actionTableC[3][i].value1.valueInt16_1=30;
				if(i==38)
					actionTableC[3][i].value1.valueInt16_1=29;
				if(i==37)
					actionTableC[3][i].value1.valueInt16_1=28;
				if(i==36)
					actionTableC[3][i].value1.valueInt16_1=27;	
				if(i==35)
					actionTableC[3][i].value1.valueInt16_1=26;
				if(i==34)
					actionTableC[3][i].value1.valueInt16_1=25;
				if(i==33)
					actionTableC[3][i].value1.valueInt16_1=24;
				if(i==32)
					actionTableC[3][i].value1.valueInt16_1=23;		
				if(i==31)
					actionTableC[3][i].value1.valueInt16_1=22;
				if(i==30)
					actionTableC[3][i].value1.valueInt16_1=21;		
				if(i==29)
					actionTableC[3][i].value1.valueInt16_1=20;
				if(i==28)
					actionTableC[3][i].value1.valueInt16_1=19;
				if(i==27)
					actionTableC[3][i].value1.valueInt16_1=18;
				if(i==26)
					actionTableC[3][i].value1.valueInt16_1=17;		
				if(i==25)
					actionTableC[3][i].value1.valueInt16_1=16;	
				if(i==24)
					actionTableC[3][i].value1.valueInt16_1=15;
				if(i==23)
					actionTableC[3][i].value1.valueInt16_1=14;
				if(i==22)
					actionTableC[3][i].value1.valueInt16_1=13;		
				if(i==21)
					actionTableC[3][i].value1.valueInt16_1=12;
				if(i==20)
					actionTableC[3][i].value1.valueInt16_1=11;	
				if(i==19)
					actionTableC[3][i].value1.valueInt16_1=10;
				if(i==18)
					actionTableC[3][i].value1.valueInt16_1=9;
				if(i==17)
					actionTableC[3][i].value1.valueInt16_1=8;
				if(i==16)
					actionTableC[3][i].value1.valueInt16_1=7;	
				if(i==15)
					actionTableC[3][i].value1.valueInt16_1=6;
				if(i==14)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==13)
					actionTableC[3][i].value1.valueInt16_1=5;
				if(i==12)
					actionTableC[3][i].value1.valueInt16_1=3;		
				if(i==11)
					actionTableC[3][i].value1.valueInt16_1=2;
				if(i==10)
					actionTableC[3][i].value1.valueInt16_1=1;		
				if(i==9)
					actionTableC[3][i].value1.valueInt16_1=8;
				if(i==8)
					actionTableC[3][i].value1.valueInt16_1=7;
				if(i==7)
					actionTableC[3][i].value1.valueInt16_1=6;
				if(i==6)
					actionTableC[3][i].value1.valueInt16_1=0;		
				// 4 is always the controller
				if(i==5)
					actionTableC[3][i].value1.valueInt16_1=0;
				if(i==3)
					actionTableC[3][i].value1.valueInt16_1=5;
				if(i==2)
					actionTableC[3][i].value1.valueInt16_1=3;
				if(i==1)
					actionTableC[3][i].value1.valueInt16_1=2;		
			}



			#ifdef DEBUGFLOW		
			trace() << "NextHop: " << actionTableC[3][i].value1.valueInt16_1 ;
			#endif

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
			flowTableC[0][i].fc.operatorC=NONE;
	





			//Flow send to Sink
			flowTableC[1][i].flowID=1;
			//resend to sink or display 
			if(i==sink) 	flowTableC[1][i].actionID=4;
			else   			    flowTableC[1][i].actionID=3;
//			flowTableC[1][i].priority=1;
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


	}
	else {
		
		//timer 0: 2 minutes for preparate report temperature in all sensors
			timerTable[0].timerID=0;
			timerTable[0].actionID=0;
			timerTable[0].t0=0.97; //inicial time  almost 9 seconds
			timerTable[0].time=2.0; //repeat in 2 minutes
			timerTable[0].repeat=255; //repeat indefinitely
			timerTable[0].multiply=60;
			timerTable[0].active=true;
			//timer 1: +30 seconds and 2 minutes for deactivate  all sensors
			timerTable[1].timerID=1;
			timerTable[1].actionID=1;
			timerTable[1].t0=1.0+dataTime; //inicial time 39 seconds (9 +30 seconds)
			timerTable[1].time=2.0; //repeat in 2 minutes
			timerTable[1].repeat=255; //repeat indefinitely
			timerTable[1].multiply=60;
			timerTable[1].active=true;
			//action 0=Activate Radio
			actionTable[0].actionID=0;
			actionTable[0].actionType=A_RECEIVER_ON;
			actionTable[0].active=true;
			//action 1=Deactivate Radio
			actionTable[1].actionID=1;
			actionTable[1].actionType=A_RECEIVER_OFF;
			actionTable[1].active=true;
		


			//timer 2: 5 minutes for read temperature in all sensors
			timerTable[2].timerID=2;
			timerTable[2].actionID=2;

			//Timer of sersor with sequencial delay
			timerTable[2].t0=1.0+((0.0+spaceDelay)*atoi(SELF_NETWORK_ADDRESS)/1000);
			
			#ifdef DEBUGFLOW
			trace() << "Sensor Delay: " <<  (100*timerTable[2].t0);
			#endif


			timerTable[2].time=2.0; //repeat in 2 minutes
			timerTable[2].repeat=255; //repeat indefinitely
			timerTable[2].multiply=60;
			timerTable[2].active=true;
			for(int j=3; j< SIZEOF_TIMER_TABLE;j++) timerTable[j].active=false;		
		//cp
		
					//The sink not read your sensor
			if(atoi(SELF_NETWORK_ADDRESS)==0){
				timerTable[2].active=false;
			}
		
			//action 2=Read Sensor
			actionTable[2].actionID=2;
			actionTable[2].actionType=A_READ_SENSOR;
			actionTable[2].value1.valueInt16_1=0;
			actionTable[2].active=true;
	
			//action 3=send temperature read to nexthop
			actionTable[3].actionID=3;
			actionTable[3].actionType=A_SEND_TO_SINK;
			actionTable[3].active=true;
			//for each node the nexthop is different

			stringstream out; out << atoi(SELF_NETWORK_ADDRESS); string SNA = out.str(); //TODO ADJUST the nexthop

			if(n==16){
				if(atoi(SELF_NETWORK_ADDRESS)==15)
					actionTable[3].value1.valueInt16_1=11;
				if(atoi(SELF_NETWORK_ADDRESS)==14)
					actionTable[3].value1.valueInt16_1=10;
				if(atoi(SELF_NETWORK_ADDRESS)==13)
					actionTable[3].value1.valueInt16_1=9;
				if(atoi(SELF_NETWORK_ADDRESS)==12)
					actionTable[3].value1.valueInt16_1=8;		
				if(atoi(SELF_NETWORK_ADDRESS)==11)
					actionTable[3].value1.valueInt16_1=7;
				if(atoi(SELF_NETWORK_ADDRESS)==10)
					actionTable[3].value1.valueInt16_1=6;		
				if(atoi(SELF_NETWORK_ADDRESS)==9)
					actionTable[3].value1.valueInt16_1=4;
				if(atoi(SELF_NETWORK_ADDRESS)==8)
					actionTable[3].value1.valueInt16_1=5;
				if(atoi(SELF_NETWORK_ADDRESS)==7)
					actionTable[3].value1.valueInt16_1=3;
				if(atoi(SELF_NETWORK_ADDRESS)==6)
					actionTable[3].value1.valueInt16_1=4;		
				// 4 is always the controller
				if(atoi(SELF_NETWORK_ADDRESS)==5)
					actionTable[3].value1.valueInt16_1=4;
				if(atoi(SELF_NETWORK_ADDRESS)==3)
					actionTable[3].value1.valueInt16_1=2;
				if(atoi(SELF_NETWORK_ADDRESS)==2)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==1)
					actionTable[3].value1.valueInt16_1=0;		
			}


			if(n==25){
				if(atoi(SELF_NETWORK_ADDRESS)==24)
					actionTable[3].value1.valueInt16_1=23;
				if(atoi(SELF_NETWORK_ADDRESS)==23)
					actionTable[3].value1.valueInt16_1=22;
				if(atoi(SELF_NETWORK_ADDRESS)==22)
					actionTable[3].value1.valueInt16_1=17;		
				if(atoi(SELF_NETWORK_ADDRESS)==21)
					actionTable[3].value1.valueInt16_1=16;
				if(atoi(SELF_NETWORK_ADDRESS)==20)
					actionTable[3].value1.valueInt16_1=15;		
				if(atoi(SELF_NETWORK_ADDRESS)==19)
					actionTable[3].value1.valueInt16_1=18;
				if(atoi(SELF_NETWORK_ADDRESS)==18)
					actionTable[3].value1.valueInt16_1=13;
				if(atoi(SELF_NETWORK_ADDRESS)==17)
					actionTable[3].value1.valueInt16_1=4;
				if(atoi(SELF_NETWORK_ADDRESS)==16)
					actionTable[3].value1.valueInt16_1=17;	

				if(atoi(SELF_NETWORK_ADDRESS)==15)
					actionTable[3].value1.valueInt16_1=11;
				if(atoi(SELF_NETWORK_ADDRESS)==14)
					actionTable[3].value1.valueInt16_1=13;
				if(atoi(SELF_NETWORK_ADDRESS)==13)
					actionTable[3].value1.valueInt16_1=9;
				if(atoi(SELF_NETWORK_ADDRESS)==12)
					actionTable[3].value1.valueInt16_1=7;		
				if(atoi(SELF_NETWORK_ADDRESS)==11)
					actionTable[3].value1.valueInt16_1=12;
				if(atoi(SELF_NETWORK_ADDRESS)==10)
					actionTable[3].value1.valueInt16_1=9;		
				if(atoi(SELF_NETWORK_ADDRESS)==9)
					actionTable[3].value1.valueInt16_1=3;
				if(atoi(SELF_NETWORK_ADDRESS)==8)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==7)
					actionTable[3].value1.valueInt16_1=2;
				if(atoi(SELF_NETWORK_ADDRESS)==6)
					actionTable[3].value1.valueInt16_1=7;		
				// 4 is always the controller
				if(atoi(SELF_NETWORK_ADDRESS)==5)
					actionTable[3].value1.valueInt16_1=3;
				if(atoi(SELF_NETWORK_ADDRESS)==3)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==2)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==1)
					actionTable[3].value1.valueInt16_1=2;		
			}

			if(n==36){
				if(atoi(SELF_NETWORK_ADDRESS)==35)
					actionTable[3].value1.valueInt16_1=34;
				if(atoi(SELF_NETWORK_ADDRESS)==34)
					actionTable[3].value1.valueInt16_1=33;
				if(atoi(SELF_NETWORK_ADDRESS)==33)
					actionTable[3].value1.valueInt16_1=32;
				if(atoi(SELF_NETWORK_ADDRESS)==32)
					actionTable[3].value1.valueInt16_1=26;		
				if(atoi(SELF_NETWORK_ADDRESS)==31)
					actionTable[3].value1.valueInt16_1=25;
				if(atoi(SELF_NETWORK_ADDRESS)==30)
					actionTable[3].value1.valueInt16_1=24;		
				if(atoi(SELF_NETWORK_ADDRESS)==29)
					actionTable[3].value1.valueInt16_1=28;
				if(atoi(SELF_NETWORK_ADDRESS)==28)
					actionTable[3].value1.valueInt16_1=27;
				if(atoi(SELF_NETWORK_ADDRESS)==27)
					actionTable[3].value1.valueInt16_1=26;
				if(atoi(SELF_NETWORK_ADDRESS)==26)
					actionTable[3].value1.valueInt16_1=20;		
				if(atoi(SELF_NETWORK_ADDRESS)==25)
					actionTable[3].value1.valueInt16_1=19;	
				if(atoi(SELF_NETWORK_ADDRESS)==24)
					actionTable[3].value1.valueInt16_1=18;
				if(atoi(SELF_NETWORK_ADDRESS)==23)
					actionTable[3].value1.valueInt16_1=22;
				if(atoi(SELF_NETWORK_ADDRESS)==22)
					actionTable[3].value1.valueInt16_1=21;		
				if(atoi(SELF_NETWORK_ADDRESS)==21)
					actionTable[3].value1.valueInt16_1=15;
				if(atoi(SELF_NETWORK_ADDRESS)==20)
					actionTable[3].value1.valueInt16_1=4;		
				if(atoi(SELF_NETWORK_ADDRESS)==19)
					actionTable[3].value1.valueInt16_1=14;
				if(atoi(SELF_NETWORK_ADDRESS)==18)
					actionTable[3].value1.valueInt16_1=13;
				if(atoi(SELF_NETWORK_ADDRESS)==17)
					actionTable[3].value1.valueInt16_1=16;
				if(atoi(SELF_NETWORK_ADDRESS)==16)
					actionTable[3].value1.valueInt16_1=15;	
				if(atoi(SELF_NETWORK_ADDRESS)==15)
					actionTable[3].value1.valueInt16_1=10;
				if(atoi(SELF_NETWORK_ADDRESS)==14)
					actionTable[3].value1.valueInt16_1=8;
				if(atoi(SELF_NETWORK_ADDRESS)==13)
					actionTable[3].value1.valueInt16_1=7;
				if(atoi(SELF_NETWORK_ADDRESS)==12)
					actionTable[3].value1.valueInt16_1=11;		
				if(atoi(SELF_NETWORK_ADDRESS)==11)
					actionTable[3].value1.valueInt16_1=10;
				if(atoi(SELF_NETWORK_ADDRESS)==10)
					actionTable[3].value1.valueInt16_1=3;		
				if(atoi(SELF_NETWORK_ADDRESS)==9)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==8)
					actionTable[3].value1.valueInt16_1=2;
				if(atoi(SELF_NETWORK_ADDRESS)==7)
					actionTable[3].value1.valueInt16_1=1;
				if(atoi(SELF_NETWORK_ADDRESS)==6)
					actionTable[3].value1.valueInt16_1=5;		
				// 4 is always the controller
				if(atoi(SELF_NETWORK_ADDRESS)==5)
					actionTable[3].value1.valueInt16_1=3;
				if(atoi(SELF_NETWORK_ADDRESS)==3)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==2)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==1)
					actionTable[3].value1.valueInt16_1=2;		
			}

			if(n==49){
				if(atoi(SELF_NETWORK_ADDRESS)==48)
					actionTable[3].value1.valueInt16_1=47;
				if(atoi(SELF_NETWORK_ADDRESS)==47)
					actionTable[3].value1.valueInt16_1=46;
				if(atoi(SELF_NETWORK_ADDRESS)==46)
					actionTable[3].value1.valueInt16_1=45;	
				if(atoi(SELF_NETWORK_ADDRESS)==45)
					actionTable[3].value1.valueInt16_1=38;
				if(atoi(SELF_NETWORK_ADDRESS)==44)
					actionTable[3].value1.valueInt16_1=45;
				if(atoi(SELF_NETWORK_ADDRESS)==43)
					actionTable[3].value1.valueInt16_1=44;
				if(atoi(SELF_NETWORK_ADDRESS)==42)
					actionTable[3].value1.valueInt16_1=43;		
				if(atoi(SELF_NETWORK_ADDRESS)==41)
					actionTable[3].value1.valueInt16_1=34;
				if(atoi(SELF_NETWORK_ADDRESS)==40)
					actionTable[3].value1.valueInt16_1=39;		
				if(atoi(SELF_NETWORK_ADDRESS)==39)
					actionTable[3].value1.valueInt16_1=38;
				if(atoi(SELF_NETWORK_ADDRESS)==38)
					actionTable[3].value1.valueInt16_1=31;
				if(atoi(SELF_NETWORK_ADDRESS)==37)
					actionTable[3].value1.valueInt16_1=38;
				if(atoi(SELF_NETWORK_ADDRESS)==36)
					actionTable[3].value1.valueInt16_1=37;	
				if(atoi(SELF_NETWORK_ADDRESS)==35)
					actionTable[3].value1.valueInt16_1=28;
				if(atoi(SELF_NETWORK_ADDRESS)==34)
					actionTable[3].value1.valueInt16_1=27;
				if(atoi(SELF_NETWORK_ADDRESS)==33)
					actionTable[3].value1.valueInt16_1=26;
				if(atoi(SELF_NETWORK_ADDRESS)==32)
					actionTable[3].value1.valueInt16_1=25;		
				if(atoi(SELF_NETWORK_ADDRESS)==31)
					actionTable[3].value1.valueInt16_1=4;
				if(atoi(SELF_NETWORK_ADDRESS)==30)
					actionTable[3].value1.valueInt16_1=24;		
				if(atoi(SELF_NETWORK_ADDRESS)==29)
					actionTable[3].value1.valueInt16_1=23;
				if(atoi(SELF_NETWORK_ADDRESS)==28)
					actionTable[3].value1.valueInt16_1=22;
				if(atoi(SELF_NETWORK_ADDRESS)==27)
					actionTable[3].value1.valueInt16_1=21;
				if(atoi(SELF_NETWORK_ADDRESS)==26)
					actionTable[3].value1.valueInt16_1=20;		
				if(atoi(SELF_NETWORK_ADDRESS)==25)
					actionTable[3].value1.valueInt16_1=19;	
				if(atoi(SELF_NETWORK_ADDRESS)==24)
					actionTable[3].value1.valueInt16_1=17;
				if(atoi(SELF_NETWORK_ADDRESS)==23)
					actionTable[3].value1.valueInt16_1=16;
				if(atoi(SELF_NETWORK_ADDRESS)==22)
					actionTable[3].value1.valueInt16_1=15;		
				if(atoi(SELF_NETWORK_ADDRESS)==21)
					actionTable[3].value1.valueInt16_1=14;
				if(atoi(SELF_NETWORK_ADDRESS)==20)
					actionTable[3].value1.valueInt16_1=13;	
				if(atoi(SELF_NETWORK_ADDRESS)==19)
					actionTable[3].value1.valueInt16_1=12;
				if(atoi(SELF_NETWORK_ADDRESS)==18)
					actionTable[3].value1.valueInt16_1=11;
				if(atoi(SELF_NETWORK_ADDRESS)==17)
					actionTable[3].value1.valueInt16_1=10;
				if(atoi(SELF_NETWORK_ADDRESS)==16)
					actionTable[3].value1.valueInt16_1=9;	
				if(atoi(SELF_NETWORK_ADDRESS)==15)
					actionTable[3].value1.valueInt16_1=8;
				if(atoi(SELF_NETWORK_ADDRESS)==14)
					actionTable[3].value1.valueInt16_1=7;
				if(atoi(SELF_NETWORK_ADDRESS)==13)
					actionTable[3].value1.valueInt16_1=6;
				if(atoi(SELF_NETWORK_ADDRESS)==12)
					actionTable[3].value1.valueInt16_1=5;		
				if(atoi(SELF_NETWORK_ADDRESS)==11)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==10)
					actionTable[3].value1.valueInt16_1=3;		
				if(atoi(SELF_NETWORK_ADDRESS)==9)
					actionTable[3].value1.valueInt16_1=2;
				if(atoi(SELF_NETWORK_ADDRESS)==8)
					actionTable[3].value1.valueInt16_1=1;
				if(atoi(SELF_NETWORK_ADDRESS)==7)
					actionTable[3].value1.valueInt16_1=6;
				if(atoi(SELF_NETWORK_ADDRESS)==6)
					actionTable[3].value1.valueInt16_1=5;		
				// 4 is always the controller
				if(atoi(SELF_NETWORK_ADDRESS)==5)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==3)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==2)
					actionTable[3].value1.valueInt16_1=3;
				if(atoi(SELF_NETWORK_ADDRESS)==1)
					actionTable[3].value1.valueInt16_1=2;		
			}


			if(n==64){
				if(atoi(SELF_NETWORK_ADDRESS)==63)
					actionTable[3].value1.valueInt16_1=62;
				if(atoi(SELF_NETWORK_ADDRESS)==62)
					actionTable[3].value1.valueInt16_1=61;		
				if(atoi(SELF_NETWORK_ADDRESS)==61)
					actionTable[3].value1.valueInt16_1=60;
				if(atoi(SELF_NETWORK_ADDRESS)==60)
					actionTable[3].value1.valueInt16_1=59;		
				if(atoi(SELF_NETWORK_ADDRESS)==59)
					actionTable[3].value1.valueInt16_1=51;
				if(atoi(SELF_NETWORK_ADDRESS)==58)
					actionTable[3].value1.valueInt16_1=59;
				if(atoi(SELF_NETWORK_ADDRESS)==57)
					actionTable[3].value1.valueInt16_1=58;
				if(atoi(SELF_NETWORK_ADDRESS)==56)
					actionTable[3].value1.valueInt16_1=48;	
				if(atoi(SELF_NETWORK_ADDRESS)==55)
					actionTable[3].value1.valueInt16_1=54;
				if(atoi(SELF_NETWORK_ADDRESS)==54)
					actionTable[3].value1.valueInt16_1=53;
				if(atoi(SELF_NETWORK_ADDRESS)==53)
					actionTable[3].value1.valueInt16_1=52;
				if(atoi(SELF_NETWORK_ADDRESS)==52)
					actionTable[3].value1.valueInt16_1=51;		
				if(atoi(SELF_NETWORK_ADDRESS)==51)
					actionTable[3].value1.valueInt16_1=43;
				if(atoi(SELF_NETWORK_ADDRESS)==50)
					actionTable[3].value1.valueInt16_1=51;		
				if(atoi(SELF_NETWORK_ADDRESS)==49)
					actionTable[3].value1.valueInt16_1=41;
				if(atoi(SELF_NETWORK_ADDRESS)==48)
					actionTable[3].value1.valueInt16_1=40;
				if(atoi(SELF_NETWORK_ADDRESS)==47)
					actionTable[3].value1.valueInt16_1=39;
				if(atoi(SELF_NETWORK_ADDRESS)==46)
					actionTable[3].value1.valueInt16_1=45;	
				if(atoi(SELF_NETWORK_ADDRESS)==45)
					actionTable[3].value1.valueInt16_1=44;
				if(atoi(SELF_NETWORK_ADDRESS)==44)
					actionTable[3].value1.valueInt16_1=43;
				if(atoi(SELF_NETWORK_ADDRESS)==43)
					actionTable[3].value1.valueInt16_1=35;
				if(atoi(SELF_NETWORK_ADDRESS)==42)
					actionTable[3].value1.valueInt16_1=34;		
				if(atoi(SELF_NETWORK_ADDRESS)==41)
					actionTable[3].value1.valueInt16_1=33;
				if(atoi(SELF_NETWORK_ADDRESS)==40)
					actionTable[3].value1.valueInt16_1=32;		
				if(atoi(SELF_NETWORK_ADDRESS)==39)
					actionTable[3].value1.valueInt16_1=31;
				if(atoi(SELF_NETWORK_ADDRESS)==38)
					actionTable[3].value1.valueInt16_1=30;
				if(atoi(SELF_NETWORK_ADDRESS)==37)
					actionTable[3].value1.valueInt16_1=29;
				if(atoi(SELF_NETWORK_ADDRESS)==36)
					actionTable[3].value1.valueInt16_1=28;	
				if(atoi(SELF_NETWORK_ADDRESS)==35)
					actionTable[3].value1.valueInt16_1=4;
				if(atoi(SELF_NETWORK_ADDRESS)==34)
					actionTable[3].value1.valueInt16_1=27;
				if(atoi(SELF_NETWORK_ADDRESS)==33)
					actionTable[3].value1.valueInt16_1=26;
				if(atoi(SELF_NETWORK_ADDRESS)==32)
					actionTable[3].value1.valueInt16_1=25;		
				if(atoi(SELF_NETWORK_ADDRESS)==31)
					actionTable[3].value1.valueInt16_1=24;
				if(atoi(SELF_NETWORK_ADDRESS)==30)
					actionTable[3].value1.valueInt16_1=23;		
				if(atoi(SELF_NETWORK_ADDRESS)==29)
					actionTable[3].value1.valueInt16_1=22;
				if(atoi(SELF_NETWORK_ADDRESS)==28)
					actionTable[3].value1.valueInt16_1=21;
				if(atoi(SELF_NETWORK_ADDRESS)==27)
					actionTable[3].value1.valueInt16_1=19;
				if(atoi(SELF_NETWORK_ADDRESS)==26)
					actionTable[3].value1.valueInt16_1=18;		
				if(atoi(SELF_NETWORK_ADDRESS)==25)
					actionTable[3].value1.valueInt16_1=17;	
				if(atoi(SELF_NETWORK_ADDRESS)==24)
					actionTable[3].value1.valueInt16_1=16;
				if(atoi(SELF_NETWORK_ADDRESS)==23)
					actionTable[3].value1.valueInt16_1=15;
				if(atoi(SELF_NETWORK_ADDRESS)==22)
					actionTable[3].value1.valueInt16_1=14;		
				if(atoi(SELF_NETWORK_ADDRESS)==21)
					actionTable[3].value1.valueInt16_1=13;
				if(atoi(SELF_NETWORK_ADDRESS)==20)
					actionTable[3].value1.valueInt16_1=12;	
				if(atoi(SELF_NETWORK_ADDRESS)==19)
					actionTable[3].value1.valueInt16_1=11;
				if(atoi(SELF_NETWORK_ADDRESS)==18)
					actionTable[3].value1.valueInt16_1=10;
				if(atoi(SELF_NETWORK_ADDRESS)==17)
					actionTable[3].value1.valueInt16_1=9;
				if(atoi(SELF_NETWORK_ADDRESS)==16)
					actionTable[3].value1.valueInt16_1=8;	
				if(atoi(SELF_NETWORK_ADDRESS)==15)
					actionTable[3].value1.valueInt16_1=7;
				if(atoi(SELF_NETWORK_ADDRESS)==14)
					actionTable[3].value1.valueInt16_1=6;
				if(atoi(SELF_NETWORK_ADDRESS)==13)
					actionTable[3].value1.valueInt16_1=5;
				if(atoi(SELF_NETWORK_ADDRESS)==12)
					actionTable[3].value1.valueInt16_1=0;		
				if(atoi(SELF_NETWORK_ADDRESS)==11)
					actionTable[3].value1.valueInt16_1=3;
				if(atoi(SELF_NETWORK_ADDRESS)==10)
					actionTable[3].value1.valueInt16_1=2;		
				if(atoi(SELF_NETWORK_ADDRESS)==9)
					actionTable[3].value1.valueInt16_1=1;
				if(atoi(SELF_NETWORK_ADDRESS)==8)
					actionTable[3].value1.valueInt16_1=7;
				if(atoi(SELF_NETWORK_ADDRESS)==7)
					actionTable[3].value1.valueInt16_1=6;
				if(atoi(SELF_NETWORK_ADDRESS)==6)
					actionTable[3].value1.valueInt16_1=5;		
				// 4 is always the controller
				if(atoi(SELF_NETWORK_ADDRESS)==5)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==3)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==2)
					actionTable[3].value1.valueInt16_1=3;
				if(atoi(SELF_NETWORK_ADDRESS)==1)
					actionTable[3].value1.valueInt16_1=2;		
			}

			if(n==81){
				if(atoi(SELF_NETWORK_ADDRESS)==80)
					actionTable[3].value1.valueInt16_1=79;		
				if(atoi(SELF_NETWORK_ADDRESS)==79)
					actionTable[3].value1.valueInt16_1=78;
				if(atoi(SELF_NETWORK_ADDRESS)==78)
					actionTable[3].value1.valueInt16_1=69;
				if(atoi(SELF_NETWORK_ADDRESS)==77)
					actionTable[3].value1.valueInt16_1=68;
				if(atoi(SELF_NETWORK_ADDRESS)==76)
					actionTable[3].value1.valueInt16_1=67;	
				if(atoi(SELF_NETWORK_ADDRESS)==75)
					actionTable[3].value1.valueInt16_1=76;
				if(atoi(SELF_NETWORK_ADDRESS)==74)
					actionTable[3].value1.valueInt16_1=75;
				if(atoi(SELF_NETWORK_ADDRESS)==73)
					actionTable[3].value1.valueInt16_1=74;
				if(atoi(SELF_NETWORK_ADDRESS)==72)
					actionTable[3].value1.valueInt16_1=73;		
				if(atoi(SELF_NETWORK_ADDRESS)==71)
					actionTable[3].value1.valueInt16_1=70;
				if(atoi(SELF_NETWORK_ADDRESS)==70)
					actionTable[3].value1.valueInt16_1=69;		
				if(atoi(SELF_NETWORK_ADDRESS)==69)
					actionTable[3].value1.valueInt16_1=68;
				if(atoi(SELF_NETWORK_ADDRESS)==68)
					actionTable[3].value1.valueInt16_1=67;
				if(atoi(SELF_NETWORK_ADDRESS)==67)
					actionTable[3].value1.valueInt16_1=58;
				if(atoi(SELF_NETWORK_ADDRESS)==66)
					actionTable[3].value1.valueInt16_1=67;	
				if(atoi(SELF_NETWORK_ADDRESS)==65)
					actionTable[3].value1.valueInt16_1=66;
				if(atoi(SELF_NETWORK_ADDRESS)==64)
					actionTable[3].value1.valueInt16_1=65;

				if(atoi(SELF_NETWORK_ADDRESS)==63)
					actionTable[3].value1.valueInt16_1=64;
				if(atoi(SELF_NETWORK_ADDRESS)==62)
					actionTable[3].value1.valueInt16_1=53;		
				if(atoi(SELF_NETWORK_ADDRESS)==61)
					actionTable[3].value1.valueInt16_1=52;
				if(atoi(SELF_NETWORK_ADDRESS)==60)
					actionTable[3].value1.valueInt16_1=59;		
				if(atoi(SELF_NETWORK_ADDRESS)==59)
					actionTable[3].value1.valueInt16_1=58;
				if(atoi(SELF_NETWORK_ADDRESS)==58)
					actionTable[3].value1.valueInt16_1=49;
				if(atoi(SELF_NETWORK_ADDRESS)==57)
					actionTable[3].value1.valueInt16_1=58;
				if(atoi(SELF_NETWORK_ADDRESS)==56)
					actionTable[3].value1.valueInt16_1=57;	
				if(atoi(SELF_NETWORK_ADDRESS)==55)
					actionTable[3].value1.valueInt16_1=46;
				if(atoi(SELF_NETWORK_ADDRESS)==54)
					actionTable[3].value1.valueInt16_1=45;
				if(atoi(SELF_NETWORK_ADDRESS)==53)
					actionTable[3].value1.valueInt16_1=44;
				if(atoi(SELF_NETWORK_ADDRESS)==52)
					actionTable[3].value1.valueInt16_1=43;		
				if(atoi(SELF_NETWORK_ADDRESS)==51)
					actionTable[3].value1.valueInt16_1=42;
				if(atoi(SELF_NETWORK_ADDRESS)==50)
					actionTable[3].value1.valueInt16_1=41;		
				if(atoi(SELF_NETWORK_ADDRESS)==49)
					actionTable[3].value1.valueInt16_1=4;
				if(atoi(SELF_NETWORK_ADDRESS)==48)
					actionTable[3].value1.valueInt16_1=40;
				if(atoi(SELF_NETWORK_ADDRESS)==47)
					actionTable[3].value1.valueInt16_1=39;
				if(atoi(SELF_NETWORK_ADDRESS)==46)
					actionTable[3].value1.valueInt16_1=38;	
				if(atoi(SELF_NETWORK_ADDRESS)==45)
					actionTable[3].value1.valueInt16_1=37;
				if(atoi(SELF_NETWORK_ADDRESS)==44)
					actionTable[3].value1.valueInt16_1=36;
				if(atoi(SELF_NETWORK_ADDRESS)==43)
					actionTable[3].value1.valueInt16_1=35;
				if(atoi(SELF_NETWORK_ADDRESS)==42)
					actionTable[3].value1.valueInt16_1=34;		
				if(atoi(SELF_NETWORK_ADDRESS)==41)
					actionTable[3].value1.valueInt16_1=33;
				if(atoi(SELF_NETWORK_ADDRESS)==40)
					actionTable[3].value1.valueInt16_1=31;		
				if(atoi(SELF_NETWORK_ADDRESS)==39)
					actionTable[3].value1.valueInt16_1=30;
				if(atoi(SELF_NETWORK_ADDRESS)==38)
					actionTable[3].value1.valueInt16_1=29;
				if(atoi(SELF_NETWORK_ADDRESS)==37)
					actionTable[3].value1.valueInt16_1=28;
				if(atoi(SELF_NETWORK_ADDRESS)==36)
					actionTable[3].value1.valueInt16_1=27;	
				if(atoi(SELF_NETWORK_ADDRESS)==35)
					actionTable[3].value1.valueInt16_1=26;
				if(atoi(SELF_NETWORK_ADDRESS)==34)
					actionTable[3].value1.valueInt16_1=25;
				if(atoi(SELF_NETWORK_ADDRESS)==33)
					actionTable[3].value1.valueInt16_1=24;
				if(atoi(SELF_NETWORK_ADDRESS)==32)
					actionTable[3].value1.valueInt16_1=23;		
				if(atoi(SELF_NETWORK_ADDRESS)==31)
					actionTable[3].value1.valueInt16_1=22;
				if(atoi(SELF_NETWORK_ADDRESS)==30)
					actionTable[3].value1.valueInt16_1=21;		
				if(atoi(SELF_NETWORK_ADDRESS)==29)
					actionTable[3].value1.valueInt16_1=20;
				if(atoi(SELF_NETWORK_ADDRESS)==28)
					actionTable[3].value1.valueInt16_1=19;
				if(atoi(SELF_NETWORK_ADDRESS)==27)
					actionTable[3].value1.valueInt16_1=18;
				if(atoi(SELF_NETWORK_ADDRESS)==26)
					actionTable[3].value1.valueInt16_1=17;		
				if(atoi(SELF_NETWORK_ADDRESS)==25)
					actionTable[3].value1.valueInt16_1=16;	
				if(atoi(SELF_NETWORK_ADDRESS)==24)
					actionTable[3].value1.valueInt16_1=15;
				if(atoi(SELF_NETWORK_ADDRESS)==23)
					actionTable[3].value1.valueInt16_1=14;
				if(atoi(SELF_NETWORK_ADDRESS)==22)
					actionTable[3].value1.valueInt16_1=13;		
				if(atoi(SELF_NETWORK_ADDRESS)==21)
					actionTable[3].value1.valueInt16_1=12;
				if(atoi(SELF_NETWORK_ADDRESS)==20)
					actionTable[3].value1.valueInt16_1=11;	
				if(atoi(SELF_NETWORK_ADDRESS)==19)
					actionTable[3].value1.valueInt16_1=10;
				if(atoi(SELF_NETWORK_ADDRESS)==18)
					actionTable[3].value1.valueInt16_1=9;
				if(atoi(SELF_NETWORK_ADDRESS)==17)
					actionTable[3].value1.valueInt16_1=8;
				if(atoi(SELF_NETWORK_ADDRESS)==16)
					actionTable[3].value1.valueInt16_1=7;	
				if(atoi(SELF_NETWORK_ADDRESS)==15)
					actionTable[3].value1.valueInt16_1=6;
				if(atoi(SELF_NETWORK_ADDRESS)==14)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==13)
					actionTable[3].value1.valueInt16_1=5;
				if(atoi(SELF_NETWORK_ADDRESS)==12)
					actionTable[3].value1.valueInt16_1=3;		
				if(atoi(SELF_NETWORK_ADDRESS)==11)
					actionTable[3].value1.valueInt16_1=2;
				if(atoi(SELF_NETWORK_ADDRESS)==10)
					actionTable[3].value1.valueInt16_1=1;		
				if(atoi(SELF_NETWORK_ADDRESS)==9)
					actionTable[3].value1.valueInt16_1=8;
				if(atoi(SELF_NETWORK_ADDRESS)==8)
					actionTable[3].value1.valueInt16_1=7;
				if(atoi(SELF_NETWORK_ADDRESS)==7)
					actionTable[3].value1.valueInt16_1=6;
				if(atoi(SELF_NETWORK_ADDRESS)==6)
					actionTable[3].value1.valueInt16_1=0;		
				// 4 is always the controller
				if(atoi(SELF_NETWORK_ADDRESS)==5)
					actionTable[3].value1.valueInt16_1=0;
				if(atoi(SELF_NETWORK_ADDRESS)==3)
					actionTable[3].value1.valueInt16_1=5;
				if(atoi(SELF_NETWORK_ADDRESS)==2)
					actionTable[3].value1.valueInt16_1=3;
				if(atoi(SELF_NETWORK_ADDRESS)==1)
					actionTable[3].value1.valueInt16_1=2;		
			}



			#ifdef DEBUGFLOW		
			trace() << "NextHop: " << actionTable[3].value1.valueInt16_1 ;
			#endif

			//action 4=Display Sensor
			actionTable[4].actionID=4;
			actionTable[4].actionType=A_DISPLAY;
			actionTable[4].active=true;	

			for(int j=5; j< SIZEOF_ACTION_TABLE;j++) actionTable[j].active = false;
	
			// Flow for read Sensor
			flowTable[0].flowID=0;
			flowTable[0].actionID=3;
//			flowTableC[0].priority=1;
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
			//resend to sink or display 
			if(atoi(SELF_NETWORK_ADDRESS)==sink) 	flowTable[1].actionID=4;
			else   			    flowTable[1].actionID=3;
//			flowTableC[1][i].priority=1;
			flowTable[1].timeout=0;
			flowTable[1].counters=0;
			flowTable[1].active=true;
			flowTable[1].fa.sourceAddress=0; 
			flowTable[1].fa.sourceMask=0; 
			flowTable[1].fa.destinationAddress=sink;
			flowTable[1].fa.destinationMask=0xFFFF;
			flowTable[1].fc.operatorC=NONE;
	
			for(int j=2; j< SIZEOF_FLOW_TABLE;j++) flowTable[j].active = false;
		
			
		
		//end
		
		
	}
	
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


#endif				//_START_CONTROLLER_TABLES_REPORT_H_
