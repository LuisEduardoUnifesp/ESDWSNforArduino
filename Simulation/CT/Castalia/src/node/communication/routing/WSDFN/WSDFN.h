/*******************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2011                            *
 *  Developed at the ATP lab, Networked Systems research theme                 *
 *  Author(s): Athanassios Boulis, Yuriy Tselishchev                           *
 *  This file is distributed under the terms in the attached LICENSE file.     *
 *  If you do not find this file, copies can be found by writing to:           *
 *                                                                             *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                *
 *      Attention:  License Inquiry.                                           *
 *                                                                             *
 *******************************************************************************/

#ifndef _WSDFN_H_
#define _WSDFN_H_

#include "VirtualRouting.h"
#include "WSDFNPack_m.h"
#include "Structs.h" 



#define BROADCAST "-1"
#define MAXNODES 82

using namespace std;

enum Timers
{
	TIMER_0 = 0,
	TIMER_1 = 1,
	TIMER_2 = 2,
	TIMER_3 = 3,
	TIMER_4 = 4,
	START_DISCOVERY_CONTROLLER_PHASE = 5,
	STOP_DISCOVERY_CONTROLLER_PHASE = 6,	
	TX_CONTROL = 7,
	START_REPORT_PHASE = 8,
	STOP_REPORT_PHASE = 9,
	START_PROGRAMMING_PHASE = 10,
	STOP_PROGRAMMING_PHASE = 11,
	START_PROGRAMMING_NODES = 12,
	REPORT_PHASE = 13,
	LAST_PROGRAMMING_NODES = 14,
	VERIFY_REPORT_RECEIVER = 15
};

class WSDFN: public VirtualRouting {
 protected:
	bool isController;
	bool isSink;
	string controller;
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
	void startup();
	
	void timerFiredCallback(int kind);
	void fromMacLayerIR(cPacket *, int, double, double);
	void startupIR();
	void timerFiredCallbackIR(int kind);


	//for WSDN

	uint16_t sink;
	int npackages;
	void startTables(int );
	uint8_t verifyPacket(WSDFNPack *);
	void executeAction(uint16_t, WSDFNPack *);
	void scheduleTimer(uint16_t );
	void scheduleTimers();
	void programmerNodes();
	void programmingANode(int );
	double dataTime;
	int programmingID;
	void clearTables();
	void displayTables();
	void displayTimerEntry(int);
	void displayTimerEntry(struct timerEntry);
	void displayFlowEntry(int);
	void displayFlowEntry(struct flowEntry);
	void displayActionEntry(int);
	void displayActionEntry(struct actionEntry);
	void displayData(char *);
	void saveDataNodes(WSDFNPack *);
	void showDataNodes(WSDFNPack *);
	struct timerEntry timerTable[SIZEOF_TIMER_TABLE];
	struct sensorEntry sensorTable[SIZEOF_SENSOR_TABLE];
	struct actionEntry actionTable[SIZEOF_ACTION_TABLE];
	struct flowEntry flowTable[SIZEOF_FLOW_TABLE];


	//for IR

	void finish();
	int spaceDelay;
	int level;	
	double lowRssi;
	double reportTime;
	double reportDuration;
	double reportFirstTime;
	int reportInterval;
	double discoveryTime;
	double discoveryDuration;
	double programmingTime;
	double programmingDuration;
	double programmingFirstTime;
	double programmingInterval;
	double delayController;
	double reportVerify;
	void setStateSleep();
	void setStateRx();	
	int reportPackages;
	int reportedPackages;
	int dataPackages;
	int totalDataPackages;
	int totalControllerPackages;
	string nextHop;
	void saveToForwardTo(string destination, string nextHop);
	void saveToControlledNodes(string source);
	string getTOForwardTO(string destination);
	string neighborhood[10];
	string forwardTo[MAXNODES][2];
	string controlledNodes[MAXNODES];
	int lastNB;
	int lastFT;
	int lastCN;

};

#endif				//WSDFNMODULE
