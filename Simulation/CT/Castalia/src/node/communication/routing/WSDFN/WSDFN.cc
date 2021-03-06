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

//#define DEBUGFLOW
#include "WSDFN.h"
#include <string>
#include "Structs.h"
//#include "startTablesEventDetect.h"
//#include "startTablesReport.h"
#include "startControllerTablesReportN5.h"
//<<<#include "startControllerTablesReportNNP.h"
//#include "startControllerTablesEventDetect.h"

Define_Module(WSDFN);

#include "IR.h"
/* Definition of an initial protocol for finding routes between the controller 
 * and the nodes and return. This defined function should obligatorily call the
 * dataFromMacLayer function that will handle the data packet handling.
//void WSDFN::fromMacLayer(cPacket * pkt, int srcMacAddress, double rssi, double lqi)

/*  
 * Function automatically called when node receives data packet
 */
//void WSDFN::dataFromMacLayer(WSDFNPack *netPacket, int srcMacAddress, double rssi, double lqi)
//{

//}


/* MAC layer sends a packet together with the source MAC address and other info.
 * With BypassMAC we just filter the packet by the NET address. If this
 * node is the right destination decapsulatePacket will extract the APP packet.
 * If not, there is no need to do anything. The whole net packet (including
 * the encapsulated apppacket will be deleted by the virtual routing code
 */
void WSDFN::fromMacLayer(cPacket * pkt, int srcMacAddress, double rssi, double lqi)
{
	int x; string macSource, saida;
	stringstream out; out << srcMacAddress; macSource = out.str();
	//RoutingPacket *netPacket = dynamic_cast <RoutingPacket*>(pkt);
	WSDFNPack *netPacket = dynamic_cast <WSDFNPack*>(pkt);
	if (netPacket) {
		string destination(netPacket->getDestination());
		#ifdef DEBUGFLOW
		trace() << "Received with RSSI: " << rssi;
		#endif
		//======================================================
		//Checking if isn't Data Packet
		if (netPacket->getWSDFNPackKind()!=DATA ) {
			fromMacLayerIR(pkt, srcMacAddress, rssi, lqi);		
		} else {
			//only Data packet here
			uint8_t action=verifyPacket(netPacket);
			executeAction(action, netPacket);
		}

	}
}

/***********************************
 * Schedule the timer defined by timerID in the timerTable
 *
 */
void WSDFN::scheduleTimer(uint16_t timerID)
{
	setTimer(timerID, timerTable[timerID].t0);
	#ifdef DEBUGFLOW
	trace() << "Timer: " << (int)timerID << " scheduled in " << timerTable[timerID].t0;
	#endif

}

/***********************************
 * Schedule the timers defined  in the timerTable
 *
 */
void WSDFN::scheduleTimers()
{
	for(int i=0; i< SIZEOF_TIMER_TABLE;i++)
		if(timerTable[i].active) scheduleTimer(i);
}


/***********************************
 * Show network information receive from Node
 *
 */
void WSDFN::showDataNodes(WSDFNPack *netPacket)
{	
	union {
		uint16_t address;
		char bytes[2];
	} nb;
	union {
		char userData[30];
		struct sensorEntry se;
	} sensorEntryPacket;
		

	//neighborhood data
	if(netPacket->getUserData(0)==REPORT_NEIGHBORHOOD_PACKET){
				stringstream out;
				int i=0;
				out << "NB:";
				for(int j=1; i<5; i++, j++){
					nb.address=(int)atoi(neighborhood[i].c_str());
					nb.bytes[0]=netPacket->getUserData(j); j++;
					nb.bytes[1]=netPacket->getUserData(j); 					
					out << ": " << (int) nb.address;  			
				}
				#ifdef DEBUGFLOW
				trace() << out.str();			
				#endif
	}

	if(netPacket->getUserData(0)==SENSOR_TABLE_PACKET) {
		for(int k=1; k<30; k++)
			sensorEntryPacket.userData[k-1]=netPacket->getUserData(k);	
		#ifdef DEBUGFLOW			
		trace() << "Sensor: ";
		trace() << "sensorEntryPacket.se.sensorID " << (int) sensorEntryPacket.se.sensorID;
		trace() << "sensorEntryPacket.se.active " << (int) sensorEntryPacket.se.active;
		if (sensorEntryPacket.se.active) {
				trace() << "sensorEntryPacket.se.sensorType " << (int) sensorEntryPacket.se.sensorType;
				trace() << "sensorEntryPacket.se.unit " << (int) sensorEntryPacket.se.sensorType;
				trace() << "sensorEntryPacket.se.dataType " << (int) sensorEntryPacket.se.sensorType;
				trace() << "sensorEntryPacket.se.interfaceType " << (int) sensorEntryPacket.se.interfaceType;
				trace() << "sensorEntryPacket.se.interfaceNumber1 " << (int) sensorEntryPacket.se.interfaceNumber1;
				trace() << "sensorEntryPacket.se.interfaceNumber2 " << (int) sensorEntryPacket.se.interfaceNumber2;
		}
		#endif

	}

}

/***********************************
 * Save network information receive from Node
 *
 */
void WSDFN::saveDataNodes(WSDFNPack *netPacket)
{	
	#ifdef DEBUGFLOW
	trace() << "Receive Network Information from : " << netPacket->getSource();
	#endif
	showDataNodes(netPacket);
}



/***********************************
 * Execute the action defined by actionID in the actionTable
 *
 */
void WSDFN::executeAction(uint16_t actionID, WSDFNPack *netPacket)
{
	string ssink;
	stringstream out; out << sink; ssink = out.str();
	uint16_t sensorID, actionIDPos;
	WSDFNPack *netPacket2;

	#ifdef DEBUGFLOW
	trace() << "Action: " << (int)actionID << " executed.";
	#endif
	//TODO Perform the action itself;

	switch(actionTable[actionID].actionType)
	{
		case A_READ_SENSOR:
			for (int i=0; i<npackages;i++){
				#ifdef DEBUGFLOW
				trace() << "Read Sensor...";
				#endif
				sensorID = actionTable[actionID].value1.valueInt16_1;
				//Verify new data in FlowTable.
				netPacket2 = new WSDFNPack("Data Packet", NETWORK_LAYER_PACKET);
				netPacket2->setSource(SELF_NETWORK_ADDRESS);
				netPacket2->setDestination("-1");
				netPacket2->setWSDFNPackKind(DATA);
				netPacket2->setUserData(0, 90 +(rand()%2)); //sensor simulation between 90 and 92
				#ifdef DEBUGFLOW
				trace() << "Read >> " << (int) netPacket2->getUserData(0);
				trace() << "Verify FlowTable...";
				#endif
				actionIDPos=verifyPacket(netPacket2);
				if(actionIDPos==255){ //TODO HERE
					#ifdef DEBUGFLOW
					trace() << "Flow not found... ";
					#endif
				} else {
					#ifdef DEBUGFLOW
					trace() << "Execute Pos action: " << (int)actionIDPos << "...";
					#endif
					executeAction(actionIDPos, netPacket2);
				}
			}

			break;
		case A_RECEIVER_ON:
			trace() << "Receiver On...";
			setStateRx();
			if(this->isSink){ 
				dataPackages=0;
			}
			break;
		case A_RECEIVER_OFF:
			trace() << "Receiver Off...";
			setStateSleep();		
			
			if(this->isSink){ 
				trace() << "Packages received: " << dataPackages;
			}
			trace() << "Total Data Package : " << totalDataPackages << " ; " << SELF_NETWORK_ADDRESS;
			break;
		case A_SEND_TO_SINK:
			#ifdef DEBUGFLOW
			trace() << "Send to Sink: " << ssink << " for:" << actionTable[actionID].value1.valueInt16_1 ;
			#endif
			//resend to next hop
			netPacket->setDestination(ssink.c_str());
			//nexthop to sink:actionTable[actionID].value1.valueInt16_1;
			toMacLayer(netPacket->dup(), actionTable[actionID].value1.valueInt16_1);
			collectOutput("WSDFN >> Qtd data packet sent by everyone");
			totalDataPackages++;
			break;
		case A_DISPLAY:
			trace() << "Display...";
			trace() << "Data receive from " << netPacket->getSource();
			trace() << "D0 " << (int) netPacket->getUserData(0);
			trace() << "D1 " << (int) netPacket->getUserData(1);
			dataPackages++;
			trace() << "dataPackages: " << dataPackages;
			break;
	}
}


void WSDFN::displayFlowEntry(struct flowEntry fe){ 
		trace() << "fe.flowID " << (int) fe.flowID;
		trace() << "fe.active " << (int) fe.active;
		if (fe.active) {
			trace() << "fe.actionID " << (int) fe.actionID;
			trace() << "fe.fa.sourceAddress " << (int) fe.fa.sourceAddress; 
			trace() << "fe.fa.sourceMask " << (int)fe.fa.sourceMask;
			trace() << "fe.fa.destinationAddress " <<(int) fe.fa.destinationAddress;
			trace() << "fe.fa.destinationMask " << (int)fe.fa.destinationMask;
			trace() << "fe.fc.dataType " << (int) fe.fc.dataType;
			trace() << "fe.fc.operatorC " << (int) fe.fc.operatorC;
			trace() << "fe.fc.value.valueInt8_1 " << (int) fe.fc.value.valueInt8_1;
		}	
}

void WSDFN::displayFlowEntry(int i){
		trace() << "flowTable["<<i<<"].flowID " << (int) flowTable[i].flowID;
		trace() << "flowTable["<<i<<"].active " << (int) flowTable[i].active;
		if (flowTable[i].active) {
			trace() << "flowTable["<<i<<"].actionID " << (int) flowTable[i].actionID;
			trace() << "flowTable["<<i<<"].fa.sourceAddress " << (int)flowTable[i].fa.sourceAddress; 
			trace() << "flowTable["<<i<<"].fa.sourceMask " << (int)flowTable[i].fa.sourceMask;
			trace() << "flowTable["<<i<<"].fa.destinationAddress " <<(int) flowTable[i].fa.destinationAddress;
			trace() << "flowTable["<<i<<"].fa.destinationMask " << (int)flowTable[i].fa.destinationMask;
			trace() << "flowTable["<<i<<"].fc.dataType " << (int)flowTable[i].fc.dataType;
			trace() << "flowTable["<<i<<"].fc.operatorC " << (int)flowTable[i].fc.operatorC;
			trace() << "flowTable["<<i<<"].fc.value.valueInt8_1 " << (int)flowTable[i].fc.value.valueInt8_1;
		}
}

void WSDFN::displayActionEntry(struct actionEntry ae){
		trace() << "ae.actionID " << (int) ae.actionID;
		trace() << "ae.active " << (int) ae.active;
		if (ae.active) {
			trace() << "ae.actionType " << (int) ae.actionType; 
			trace() << "ae.dataType " << (int) ae.dataType; 
			trace() << "ae.operatorC " << (int) ae.operatorC;
			trace() << "ae.value1.valueInt16_1 " << (int) ae.value1.valueInt16_1;
		}		
}

void WSDFN::displayActionEntry(int i){
		trace() << "actionTable["<<i<<"].actionID " << (int) actionTable[i].actionID;
		trace() << "actionTable["<<i<<"].active " << (int) actionTable[i].active;
		if (actionTable[i].active) {
			trace() << "actionTable["<<i<<"].actionType " << (int) actionTable[i].actionType; 
			trace() << "actionTable["<<i<<"].dataType " << (int) actionTable[i].dataType; 
			trace() << "actionTable["<<i<<"].operatorC " << (int) actionTable[i].operatorC;
			trace() << "actionTable["<<i<<"].value1.valueInt16_1 " << (int) actionTable[i].value1.valueInt16_1;
		}
}

void WSDFN::displayTimerEntry(struct timerEntry te){
		trace() << "te.timerID " << (int) te.timerID;
		trace() << "te.active " << (int) te.active;
		if (te.active) {
			trace() << "te.actionID " << (int) te.actionID;
			trace() << "te.t0 " << te.t0;
			trace() << "te.time " << te.time;
			trace() << "te.repeat " << (int) te.repeat;
			trace() << "te.multiply " << (int) te.multiply;

		}	
}

void WSDFN::displayTimerEntry(int i){
		trace() << "timerTable["<<i<<"].timerID " << (int) timerTable[i].timerID;
		trace() << "timerTable["<<i<<"].active " << (int) timerTable[i].active;
		if (timerTable[i].active) {
			trace() << "timerTable["<<i<<"].actionID " << (int) timerTable[i].actionID;
			trace() << "timerTable["<<i<<"].t0 " << timerTable[i].t0;
			trace() << "timerTable["<<i<<"].time " << timerTable[i].time;
			trace() << "timerTable["<<i<<"].repeat " << (int) timerTable[i].repeat;
			trace() << "timerTable["<<i<<"].multiply " << (int) timerTable[i].multiply;

		}
}

void WSDFN::displayTables(){
//#ifdef DEBUGFLOW
	trace() << "Display tables of " << SELF_NETWORK_ADDRESS;
	trace() << "FlowTable: ";
	for(int i=0; i< SIZEOF_FLOW_TABLE;i++) displayFlowEntry(i);
	trace() << "ActionTable: ";
	for(int i=0; i< SIZEOF_ACTION_TABLE;i++) displayActionEntry(i);
	trace() << "TimerTable: ";
	for(int i=0; i< SIZEOF_TIMER_TABLE;i++)	displayTimerEntry(i);
//#endif
}

void WSDFN::displayData(char *userData){
	stringstream out; 
	out << ">> ";
	for(int k=0; k<30; k++)
		out << (int) userData[k] <<" : ";
	trace() << out.str();
}




/***********************************
 * 	Checks whether a data packet belogs to a dataflow in the flow table.
 *  If it belongs, the function return the ActionID corresponding to the flow 
 *	If not, return -1 
 */
uint8_t WSDFN::verifyPacket(WSDFNPack *netPacket)
{
#ifdef DEBUGFLOW
	trace() << "verifing: d0:" << (int)netPacket->getUserData(0);
	trace() << "verifing: d1:" << (int)netPacket->getUserData(1);
 	trace() << "verifing: source:" << netPacket->getSource();
 	trace() << "verifing: destination:" << netPacket->getDestination();
#endif
	bool find=false;
	uint16_t maskedSource;
	uint16_t maskedDestination;
	// retrieves the first two integers of the packet data
	union {
		int a1;
		int a2;
		struct {
			char d1;
			char e1;
			char d2;
			char e2;
		} b;
	} sixteen;
	sixteen.b.d1=netPacket->getUserData(0);
	sixteen.b.e1=netPacket->getUserData(1);
	sixteen.b.d2=netPacket->getUserData(2);
	sixteen.b.e2=netPacket->getUserData(3);

	for(int i=0;i<SIZEOF_FLOW_TABLE;i++){

#ifdef DEBUGFLOW
		trace() << "FT: " << i;
		trace() << "INT_8: " << (int)INT_8;		
		trace() << "flowTable["<<i<<"].actionID " << (int) flowTable[i].actionID;
		trace() << "flowTable["<<i<<"].fa.sourceAddress " << flowTable[i].fa.sourceAddress; 
		trace() << "flowTable["<<i<<"].fa.sourceMask " << flowTable[i].fa.sourceMask;
		trace() << "flowTable["<<i<<"].fa.destinationAddress " << flowTable[i].fa.destinationAddress;
		trace() << "flowTable["<<i<<"].fa.destinationMask " << flowTable[i].fa.destinationMask;
		trace() << "flowTable["<<i<<"].fc.dataType " << (int)flowTable[i].fc.dataType;
		trace() << "flowTable["<<i<<"].fc.operatorC " << (int)flowTable[i].fc.operatorC;
		trace() << "flowTable["<<i<<"].fc.value.valueInt8_1 " << (int)flowTable[i].fc.value.valueInt8_1;
#endif
		if(!flowTable[i].active){
#ifdef DEBUGFLOW
			trace() << "not active...";
#endif
			continue;
		}


		switch(flowTable[i].fc.operatorC)
		{
			case NONE:
				find=true;
				break;
			case E:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(netPacket->getUserData(0)==flowTable[i].fc.value.valueInt8_1) find=true;			
				if(flowTable[i].fc.dataType==INT_16) 
					if(sixteen.a1==flowTable[i].fc.value.valueInt16_1) find=true;				
				break;
			case D:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(netPacket->getUserData(0)!=flowTable[i].fc.value.valueInt8_1) find=true;			
				if(flowTable[i].fc.dataType==INT_16) 
					if(sixteen.a1!=flowTable[i].fc.value.valueInt16_1) find=true;				
				break;
			case GT:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(netPacket->getUserData(0) > flowTable[i].fc.value.valueInt8_1) {
#ifdef DEBUGFLOW
						trace()<< "find True GT";
#endif
						find=true;			
					}
				if(flowTable[i].fc.dataType==INT_16) 
					if(sixteen.a1 > flowTable[i].fc.value.valueInt16_1) find=true;
				break;
			case GTOE:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(netPacket->getUserData(0)>=flowTable[i].fc.value.valueInt8_1) find=true;			
				if(flowTable[i].fc.dataType=INT_16) 
					if(sixteen.a1>=flowTable[i].fc.value.valueInt16_1) find=true;
				break;
			case LT:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(netPacket->getUserData(0)<flowTable[i].fc.value.valueInt8_1) find=true;			
				if(flowTable[i].fc.dataType=INT_16) 
					if(sixteen.a1<flowTable[i].fc.value.valueInt16_1) find=true;
				break;
			case LTOE:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(netPacket->getUserData(0)<=flowTable[i].fc.value.valueInt8_1) find=true;			
				if(flowTable[i].fc.dataType=INT_16) 
					if(sixteen.a1<=flowTable[i].fc.value.valueInt16_1) find=true;
				break;
			case GT_A_LT:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(
					   (netPacket->getUserData(0)>flowTable[i].fc.value.valueInt8_1) and
					   (netPacket->getUserData(1)<flowTable[i].fc.value.valueInt8_2)
					  ) find=true;			
				break;
				if(flowTable[i].fc.dataType==INT_16) 
					if(
					   (sixteen.a1>flowTable[i].fc.value.valueInt16_1) and
					   (sixteen.a2<flowTable[i].fc.value.valueInt16_2)
 					  ) find=true;
				break;
			case GTOE_A_LT:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(
					   (netPacket->getUserData(0)>=flowTable[i].fc.value.valueInt8_1) and
					   (netPacket->getUserData(1)<flowTable[i].fc.value.valueInt8_2)
					  ) find=true;			
				break;
				if(flowTable[i].fc.dataType==INT_16) 
					if(
					   (sixteen.a1>=flowTable[i].fc.value.valueInt16_1) and
					   (sixteen.a2<flowTable[i].fc.value.valueInt16_2)
 					  ) find=true;
				break;
			case GT_A_LTOE:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(
					   (netPacket->getUserData(0)>flowTable[i].fc.value.valueInt8_1) and
					   (netPacket->getUserData(1)<=flowTable[i].fc.value.valueInt8_2)
					  ) find=true;			
				break;
				if(flowTable[i].fc.dataType==INT_16) 
					if(
					   (sixteen.a1>flowTable[i].fc.value.valueInt16_1) and
					   (sixteen.a2<=flowTable[i].fc.value.valueInt16_2)
 					  ) find=true;
				break;
			case GTOE_A_LTOE:
				if(flowTable[i].fc.dataType==INT_8) 					
					if(
					   (netPacket->getUserData(0)>=flowTable[i].fc.value.valueInt8_1) and
					   (netPacket->getUserData(1)<=flowTable[i].fc.value.valueInt8_2)
					  ) find=true;			
				break;
				if(flowTable[i].fc.dataType==INT_16) 
					if(
					   (sixteen.a1>=flowTable[i].fc.value.valueInt16_1) and
					   (sixteen.a2<=flowTable[i].fc.value.valueInt16_2)
 					  ) find=true;
				break;
		}

		maskedSource=(atoi(netPacket->getSource()))&flowTable[i].fa.sourceMask;
		maskedDestination=(atoi(netPacket->getDestination()))&flowTable[i].fa.destinationMask;
#ifdef DEBUGFLOW
		trace()<< "MS: " << maskedSource;
		trace()<< "MD: " << maskedDestination;
#endif
		if ((flowTable[i].fa.sourceAddress!=maskedSource) || 
			(flowTable[i].fa.destinationAddress!=maskedDestination)) find= false;				
#ifdef DEBUGFLOW
		trace() << "Find: " << find;
#endif
		if(find){
			flowTable[i].counters++;
			return flowTable[i].actionID;
		} 
	}
	return -1;
}




/* Application layer sends a packet together with a dest network layer address.
 * Network layer is responsible to route that packet by selecting an appropriate
 * MAC address. With BypassRouting we do not perform any routing function. We
 * just encapsulate the app packet and translate the net address to a MAC address
 * (e.g. "3" becomes 3, or a BROADCAST_NETWORK_ADDRESS becomes BROADCAST_MAC_ADDRESS)
 * If the destination is a 1-hop neighbor it will receive the packet.
 */
void WSDFN::fromApplicationLayer(cPacket * pkt, const char *destination)
{
	//WSDFNPack *netPacket = new WSDFNPack("WSDFNPack packet", NETWORK_LAYER_PACKET);
	//netPacket->setSource(SELF_NETWORK_ADDRESS);
	//netPacket->setDestination(destination);
	//encapsulatePacket(netPacket, pkt);
	//toMacLayer(netPacket, resolveNetworkAddress(destination));
}






/**
 * 
 *
 **/
void WSDFN::startup()
{
	
	int numNodes = par("numNodes");
	sink= par("sinkAddress");
	trace() << "Number of nodes:" << numNodes;
	npackages = par("npackages");

	//Verifying if a node is the controller
	this->isController = par("isController").boolValue();

	//Verifying if a node is the sink
	this->isSink = par("isSink").boolValue();
    //error
	//if (atoi(SELF_NETWORK_ADDRESS)==0) this->isController = true;
	//cModule *appModule= getParentModule()->getParentModule()->getSubmodule("Application"); 
	#ifdef DEBUGFLOW
	trace() << "isController? " << this->isController;
	trace() << "isSink? " << this->isSink;
	#endif
	startTables(numNodes);
	startupIR();
}




void WSDFN::timerFiredCallback(int kind)
{
//	timerFiredCallbackIR(kind);
	if(kind>4) {
		
		timerFiredCallbackIR(kind);
		
	}
	else {
//		#ifdef DEBUGFLOW
		trace()<< "Timer Activate type: " << (int) kind;
//		#endif
		timerTable[kind].repeat--;
		if(timerTable[kind].repeat>0){
			if(timerTable[kind].repeat==254) timerTable[kind].repeat++; //for infinite repetition
			setTimer(kind, timerTable[kind].time*timerTable[kind].multiply);
			executeAction(timerTable[kind].actionID, NULL);
		}
	}
}



