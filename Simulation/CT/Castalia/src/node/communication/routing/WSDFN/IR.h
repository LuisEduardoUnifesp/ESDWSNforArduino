



#ifndef _IR_H_
#define _IR_H_

/* MAC layer sends a packet together with the source MAC address and other info.
 * With BypassMAC we just filter the packet by the NET address. If this
 * node is the right destination decapsulatePacket will extract the APP packet.
 * If not, there is no need to do anything. The whole net packet (including
 * the encapsulated apppacket will be deleted by the virtual routing code
 */
void WSDFN::fromMacLayerIR(cPacket * pkt, int srcMacAddress, double rssi, double lqi)
{
	union {
		char userData[30];
		struct flowEntry fe;
	} flowEntryPacket;

	union {
		char userData[30];
		struct timerEntry te;
	} timerEntryPacket;

	union {
		char userData[30];
		struct actionEntry ae;
	} actionEntryPacket;


	int x; string macSource, saida;
	stringstream out; out << srcMacAddress; macSource = out.str();
	//RoutingPacket *netPacket = dynamic_cast <RoutingPacket*>(pkt);
	WSDFNPack *netPacket = dynamic_cast <WSDFNPack*>(pkt);
	if (netPacket) {
		string destination(netPacket->getDestination());

		//====================================================
		//Checking if Report Confirmation Packet from Controller
		if (netPacket->getWSDFNPackKind()==REPORT_CONFIRMATION ) {
			//It isn't a destination?
			if(destination.compare(SELF_NETWORK_ADDRESS) != 0){
				//resend to next hop
				string nh = getTOForwardTO(destination);
				
				toMacLayer(netPacket->dup(), atoi(nh.c_str()));
				collectOutput("WSDFN >> Qtd controller packet sent by everyone");
				totalControllerPackages++;
				#ifdef DEBUGFLOW
				trace() << "Report Confirmation resend from " << netPacket->getSource() << " to " << destination;
				#endif
			} else {
				#ifdef DEBUGFLOW
	            trace() << "Report Confirmation RECEIVE from " << netPacket->getSource() << " to " << destination
				<< " > " << reportedPackages;
				#endif
				reportedPackages++;
			}
		}


		//Checking if Report Packet to Controller
		if ( (destination.compare(controller) == 0 ||
			  destination.compare(SELF_NETWORK_ADDRESS) == 0) &&
			netPacket->getWSDFNPackKind()==REPORT ) {
			if(!this->isController){
				//resend to next hop
				toMacLayer(netPacket->dup(), atoi(nextHop.c_str()));
				collectOutput("WSDFN >> Qtd controller packet sent by everyone");
				totalControllerPackages++;
				#ifdef DEBUGFLOW
				trace() << "Report from " << netPacket->getSource() << " resend to nH:" << nextHop;
				#endif
				//save route for messages from Controller to Sensor
				saveToForwardTo(netPacket->getSource(), macSource);
				saida = " ForwardTo of ";					
				saida += SELF_NETWORK_ADDRESS;
				saida += ": ";
				for(x=0; x<=lastFT; x++){
					saida+= forwardTo[x][0] +", " + forwardTo[x][1] + "; ";
				}
				#ifdef DEBUGFLOW
				trace() << saida;
				#endif
			} else {
				#ifdef DEBUGFLOW
				trace() << "Report received from " << netPacket->getSource();
				#endif
				//save route for messages from Controller to Sensor
				saveToForwardTo(netPacket->getSource(), macSource);
				//register nodes controlled by this Controller
				saveToControlledNodes(netPacket->getSource());
				//save information from node
				saveDataNodes(netPacket);
				saida = " Controlled by ";					
				saida += SELF_NETWORK_ADDRESS;
				saida += ": ";
				for(x=0; x<=lastCN; x++){
					saida+= "<"+controlledNodes[x] +">";
				}
				#ifdef DEBUGFLOW
				trace() << saida;
				#endif
				saida = " ForwardTo of ";					
				saida += SELF_NETWORK_ADDRESS;
				saida += ": ";
				for(x=0; x<=lastFT; x++){
					saida+= forwardTo[x][0] +", " + forwardTo[x][1] + "; ";
				}
				#ifdef DEBUGFLOW
				trace() << saida;
				#endif
				//resend to node
				#ifdef DEBUGFLOW
				trace() << "Confirmation Report to " << netPacket->getSource() << " From " << SELF_NETWORK_ADDRESS;
				#endif
				netPacket->setWSDFNPackKind(REPORT_CONFIRMATION);
				string nh = getTOForwardTO(netPacket->getSource());
				netPacket->setDestination(netPacket->getSource());
				netPacket->setSource(SELF_NETWORK_ADDRESS);
				toMacLayer(netPacket->dup(), atoi(nh.c_str()));
				collectOutput("WSDFN >> Qtd controller packet sent by everyone");
				totalControllerPackages++;

			}

		}

		//====================================================
		//Checking if Programming Confirmation Packet to Controller
		if (netPacket->getWSDFNPackKind()==PROGRAMMING_CONFIRMATION ) {
			//It isn't a destination?
			if(destination.compare(SELF_NETWORK_ADDRESS) != 0){
				//resend to next hop
				toMacLayer(netPacket->dup(), atoi(nextHop.c_str()));
				collectOutput("WSDFN >> Qtd controller packet sent by everyone");
				totalControllerPackages++;
				#ifdef DEBUGFLOW
				trace() << "Programming Confirmation from " << netPacket->getSource() << " to " << destination;
				#endif
			} else {
				#ifdef DEBUGFLOW
	            trace() << "Programming Confirmation RECEIVE from " << netPacket->getSource() << " to " << destination;
	            #endif
				programmedPackages[atoi(netPacket->getSource())]++;
			}
		}


		//====================================================
		//Checking if Programming Packet from Controller
		if (netPacket->getWSDFNPackKind()==PROGRAMMING ) {
			//It isn't a destination?
			if(destination.compare(SELF_NETWORK_ADDRESS) != 0){
				//resend to next hop
				toMacLayer(netPacket->dup(), atoi(getTOForwardTO(destination).c_str()));
				collectOutput("WSDFN >> Qtd controller packet sent by everyone");
				totalControllerPackages++;
				#ifdef DEBUGFLOW
				trace() << "Programming from " << netPacket->getSource() << " to " << destination;
				#endif
			} else {
				#ifdef DEBUGFLOW
				trace() << "Programming received from " << netPacket->getSource() << " by: "<<  macSource <<" for: "<< SELF_NETWORK_ADDRESS;
				#endif
				//TO DO PROGRAMMING TABLES
				if(netPacket->getUserData(0)==FLOW_TABLE_PACKET){
					for(int k=1; k<30; k++)
						flowEntryPacket.userData[k-1]=netPacket->getUserData(k);
					flowTable[flowEntryPacket.fe.flowID]=flowEntryPacket.fe;
					//displayFlowEntry(flowEntryPacket.fe);
				}
				if(netPacket->getUserData(0)==ACTION_TABLE_PACKET){
					for(int k=1; k<30; k++)
						actionEntryPacket.userData[k-1]=netPacket->getUserData(k);
					actionTable[actionEntryPacket.ae.actionID]=actionEntryPacket.ae;
					//displayActionEntry(actionEntryPacket.ae);
				}
				if(netPacket->getUserData(0)==TIMER_TABLE_PACKET){
					for(int k=1; k<30; k++)
						timerEntryPacket.userData[k-1]=netPacket->getUserData(k);
					timerTable[timerEntryPacket.te.timerID]=timerEntryPacket.te;
					//displayTimerEntry(timerEntryPacket.te);
					//scheduleTimer(timerEntryPacket.te.timerID);
				}
				//resend to controler
				netPacket->setWSDFNPackKind(PROGRAMMING_CONFIRMATION);
				netPacket->setSource(SELF_NETWORK_ADDRESS);
				netPacket->setDestination(controller.c_str());
				toMacLayer(netPacket->dup(), atoi(nextHop.c_str()));
				collectOutput("WSDFN >> Qtd controller packet sent by everyone");
				totalControllerPackages++;
				#ifdef DEBUGFLOW
				trace() << "Programming from " << netPacket->getSource() << " to " << destination;
				#endif
			}
		}		
		//====================================================
		//Checking if Controller_Discovery Packet
		if ( (destination.compare(SELF_NETWORK_ADDRESS) == 0 ||
		     destination.compare(BROADCAST_NETWORK_ADDRESS) == 0) &&
			netPacket->getWSDFNPackKind()==CONTROLLER_DISCOVERY &&
			rssi > lowRssi) {

			if(netPacket->getWSDFNPackKind()==CONTROLLER_DISCOVERY){
				#ifdef DEBUGFLOW
				trace() << "Discovery Packet of: "<< netPacket->getSource()  <<" received from: " << macSource;
				#endif
				//save neighborhood
				if(lastNB<10){
					lastNB++;
					//neighborhood.push_back(source);
					neighborhood[lastNB] = macSource;
					saida = " Neighborhood of ";					
					saida += SELF_NETWORK_ADDRESS;
					saida += ": ";
					for(x=0; x<=lastNB; x++){
						saida+= neighborhood[x] +" ";
					}
					#ifdef DEBUGFLOW
					trace() << saida;
					#endif
				}
				#ifdef DEBUGFLOW
				trace() << "Level Packet: " << netPacket->getLevel() << " Level node: " << level;
				#endif
				//Checking if node already received a packet with level equal to or less than received now
				if(netPacket->getLevel() < (level-1)){
					level = netPacket->getLevel()+1;
					nextHop = macSource;
					controller = netPacket->getSource();
					#ifdef DEBUGFLOW
					trace() << "NextHop of " << SELF_NETWORK_ADDRESS << " is " << nextHop;
					trace() << "Controller of " << SELF_NETWORK_ADDRESS << " is " << controller;
					#endif
					//Send new Controller_Discovery Packet to next level
					netPacket->setLevel(level);
					//netPacket->setSource(SELF_NETWORK_ADDRESS);
					toMacLayer(netPacket->dup(), BROADCAST_MAC_ADDRESS);
					collectOutput("WSDFN >> Qtd controller packet sent by everyone");
					totalControllerPackages++;
					#ifdef DEBUGFLOW
					trace() << "Resend Controller_Discovery hop:" << level << ", sender:" << SELF_NETWORK_ADDRESS; 
					trace() << "Final Level node: " << level;
					#endif
				}
			}
		}

	}
}


void WSDFN::saveToControlledNodes(string source)
{
	int x;
	//Not save in ControlledNode if source is in CN
	for(x=0; x<=lastCN; x++){
		if(controlledNodes[x].compare(source)==0){
			return;
		}
	}
	lastCN++;
	if(lastCN<MAXNODES)	controlledNodes[lastCN]=source;
}

void WSDFN::saveToForwardTo(string destination, string nextHop)
{
	int x;
	bool found = false;
	//Not save in forwardTO if destination is in Neighborhood
	//for(x=0; x<=lastNB; x++){
	//	if(neighborhood[x].compare(destination)==0){
	//		return;
	//	}
	//}
	//not duplicate destination
	for(x=0; x<=lastFT; x++){
		if(forwardTo[x][0].compare(destination)==0){
			forwardTo[x][1]=nextHop;
			found = true;
			break;
		}
	}	
	if(!found && lastFT<MAXNODES){
		lastFT++;
		forwardTo[lastFT][0]=destination;
		forwardTo[lastFT][1]=nextHop;
	}	
}

string WSDFN::getTOForwardTO(string destination)
{
	int x;
	for(x=0; x<=lastFT; x++){
		if(forwardTo[x][0].compare(destination)==0){
			return forwardTo[x][1];
		}
	}
	return "-1";
}


void WSDFN::timerFiredCallbackIR(int kind){
	union {
		uint16_t address;
		char bytes[2];
	} nb;
	union {
		char userData[30]; 
		struct sensorEntry se;
	} sensorEntryPacket;

	switch(kind)
	{
		case START_DISCOVERY_CONTROLLER_PHASE:
		{			
			trace() << "Starting Discovery Controller PHASE";
			trace() << "Receiver On...";
//			setStateSleep();
/** -- 	*/		
			setStateRx();

			if(this->isController){
				trace() << "Discovery Packet Sending";
			
				WSDFNPack *netPacket = new WSDFNPack("Discovery Packet", NETWORK_LAYER_PACKET);
				netPacket->setSource(SELF_NETWORK_ADDRESS);
				netPacket->setDestination(BROADCAST);
				netPacket->setLevel(0);
				netPacket->setWSDFNPackKind(CONTROLLER_DISCOVERY);
				//Adjust power send
				//if(twoPower) toMacLayer(createRadioCommand(SET_TX_OUTPUT,minPower));
				//register and send Discovery package
				toMacLayer(netPacket, BROADCAST_MAC_ADDRESS);
				collectOutput("WSDFN >> Qtd controller packet sent by everyone");
				totalControllerPackages++;
				//Readjust power send
 				//if(twoPower) toMacLayer(createRadioCommand(SET_TX_OUTPUT,maxPower));

				//register informations
  				//trace() << "Send Pacote Descoberta de : " << SELF_NETWORK_ADDRESS <<"\n";
				//collectOutput("app4 >> Total of messages descoberta", SELF_NETWORK_ADDRESS);

				//Restart Discovery PHASE after n time.
				//setTimer(START_DISCOVERY_CONTROLLER_PHASE, this->timeRedescovery); 
			}
/** -- 	*/
			
			
			return;
		}
		case STOP_DISCOVERY_CONTROLLER_PHASE:
		{
			//TO_DO Stop radio rx
			trace() << "Stop Discovery Controller PHASE";
			return;
		}
		

		case START_REPORT_PHASE:
		{			
			double interval = rand() % reportInterval;
			#ifdef DEBUGFLOW
			trace() << "Report Delay: " << interval << " != " << reportInterval;
			#endif
			setTimer(REPORT_PHASE, ((double) interval) /1000);
			return;
		}
		case REPORT_PHASE:
		{
			trace() << "Starting Report PHASE";
/** -- 	*/		
			if(!this->isController){
				
				
				reportPackages=0;
				reportedPackages=0;
				//send 5 firsts neighborhoods informations to sink
				WSDFNPack *netPacket = new WSDFNPack("Report Packet [NB]", NETWORK_LAYER_PACKET);
				netPacket->setSource(SELF_NETWORK_ADDRESS);
				netPacket->setDestination(controller.c_str());
				netPacket->setWSDFNPackKind(REPORT);
				netPacket->setUserData(0,REPORT_NEIGHBORHOOD_PACKET);
				int i=0, j;
				for(j=1; (i<=lastNB)&&(i<5); i++, j++){
					nb.address=(int)atoi(neighborhood[i].c_str());
					netPacket->setUserData(j, nb.bytes[0] ); j++;
					netPacket->setUserData(j, nb.bytes[1] );
				}
				//if neighbors are less than 5 complete with broadcast (0xFFFF)
				if (i<5){
					for(; i<5; i++, j++){
						netPacket->setUserData(j, 0xFF ); j++;
						netPacket->setUserData(j, 0xFF );
					}
				}
				toMacLayer(netPacket, atoi(nextHop.c_str()));
				collectOutput("WSDFN >> Qtd controller packet sent by everyone");
				totalControllerPackages++;
				#ifdef DEBUGFLOW
				trace() << "Report NR[1] from " << SELF_NETWORK_ADDRESS << " send ";
				#endif
				reportPackages++;
				
				if(lastNB>=5){
					//send last neighborhoods informations to sink
					netPacket = new WSDFNPack("Report Packet [NB]", NETWORK_LAYER_PACKET);
					netPacket->setSource(SELF_NETWORK_ADDRESS);
					netPacket->setDestination(controller.c_str());
					netPacket->setWSDFNPackKind(REPORT);
					netPacket->setUserData(0,REPORT_NEIGHBORHOOD_PACKET);
					for(j=1; (i<=lastNB)&&(i<10); i++, j++){
						nb.address=(int)atoi(neighborhood[i].c_str());
						netPacket->setUserData(j, nb.bytes[0] ); j++;
						netPacket->setUserData(j, nb.bytes[1] );
					}
					//if neighbors are less than 10 complete with broadcast (0xFFFF)
					if (i<10){
						for(; i<10; i++, j++){
							netPacket->setUserData(j, 0xFF ); j++;
							netPacket->setUserData(j, 0xFF );
						}
					}
					toMacLayer(netPacket, atoi(nextHop.c_str()));
					collectOutput("WSDFN >> Qtd controller packet sent by everyone");
					totalControllerPackages++;
					#ifdef DEBUGFLOW
					trace() << "Report NR[2] from " << SELF_NETWORK_ADDRESS << " send ";
					#endif
					reportPackages++;
				}
				// Send sensors informations
				// TODO SENSORS
				for(int j=0; j< SIZEOF_SENSOR_TABLE;j++) 						
					if(sensorTable[j].active) {
						sensorEntryPacket.se=sensorTable[j];
						netPacket = new WSDFNPack("Reporting Sensor Packet", NETWORK_LAYER_PACKET);
						netPacket->setSource(SELF_NETWORK_ADDRESS);
						netPacket->setDestination(controller.c_str());
						netPacket->setWSDFNPackKind(REPORT);
						netPacket->setUserData(0,SENSOR_TABLE_PACKET);
						for(int k=0; k<29; k++)
							netPacket->setUserData(k+1,sensorEntryPacket.userData[k]);
						toMacLayer(netPacket, atoi(nextHop.c_str()));
						collectOutput("WSDFN >> Qtd controller packet sent by everyone");
						totalControllerPackages++; 
						reportPackages++;
						#ifdef DEBUGFLOW
						trace() << 	"Reporting ST[" <<j<< "] from " << SELF_NETWORK_ADDRESS << " to " << controller <<
						" using " << nextHop;
						#endif
					} 

			}
			if(!this->isController){ 
				setTimer(VERIFY_REPORT_RECEIVER, reportVerify);
			}
/** -- 	*/		 
			setTimer(STOP_REPORT_PHASE, reportDuration);
			setTimer(START_REPORT_PHASE, reportTime);
			return;
		}
		case VERIFY_REPORT_RECEIVER:
		{
			#ifdef DEBUGFLOW
			trace() << "Verify Report Receiver- Report:" << reportPackages << ", Reported:" << reportedPackages;
			#endif
			//resend report in case of fail
			if(reportPackages!=reportedPackages){
				trace() << "Report Resend.";
				setTimer(REPORT_PHASE, 0);	
			}				
			return;
		}
		case STOP_REPORT_PHASE:
		{
			//TO_DO Stop radio rx
			trace() << "Stop Report PHASE";
			return;
		}

		case START_PROGRAMMING_PHASE:
		{
			trace() << "Starting Programming PHASE";
/** -- */			
			if(this->isController){
				programmerNodes();
			} else {
				clearTables();	
			}
/** -- */ 
			trace() << "Programming Duration: " << programmingDuration;
			setTimer(STOP_PROGRAMMING_PHASE, programmingDuration);
			setTimer(START_PROGRAMMING_PHASE, programmingTime);
			return;
		}
		case STOP_PROGRAMMING_PHASE:
		{
			//TO_DO Stop radio rx
			trace() << "Stop Programming PHASE";
			scheduleTimers();
			#ifdef DEBUGFLOW
			//displayTables();
			#endif
			trace() << "Total Controller Package: " << totalControllerPackages << " ; " << SELF_NETWORK_ADDRESS ;
			trace() << "Receiver Off...";
			setStateSleep();
			return;
		}
		case START_PROGRAMMING_NODES:
		{	
			
			#ifdef DEBUGFLOW
			trace() << "lastCN: "<< lastCN <<", ProgrammingID: " << programmingID;
			#endif
			if(programmingID!=0){
				int cni=atoi(controlledNodes[programmingID-1].c_str());
				#ifdef DEBUGFLOW
				trace() << "#Program packages to " << cni << ": " << programPackages[cni];
				trace() << "#Programmed packages to " << cni << ": " << programmedPackages[cni];
				#endif
				//in case of transmission fails, resend programmations to last node.
	            if(programmedPackages[cni]!=programPackages[cni]){
					programmingID--;
				}
			    else if(programmingID==(lastCN+1)){
					return;
				}
			}
			programmingANode(programmingID);
			programmingID++;
			setTimer(START_PROGRAMMING_NODES, programmingInterval);

		}


		case TX_CONTROL:
		{
			return;
		}
	}

}


void WSDFN::setStateRx()
{
	send(createRadioCommand(SET_STATE, RX), "toMacModule");	
}

//void WSDFN::setPowerLevel(double powerLevel)
//{
//	send(createRadioCommand(SET_TX_OUTPUT, powerLevel), "toMacModule");
//}

void WSDFN::setStateSleep()
{
	send(createRadioCommand(SET_STATE, SLEEP), "toMacModule");
}


void WSDFN::clearTables(){
	#ifdef DEBUGFLOW
	trace() << "Clear table of " << SELF_NETWORK_ADDRESS;
	#endif
	for(int j=0; j< SIZEOF_FLOW_TABLE;j++) 						
		flowTable[j].active=false;
	for(int j=0; j< SIZEOF_ACTION_TABLE;j++) 						
		actionTable[j].active=false;
	for(int j=0; j< SIZEOF_TIMER_TABLE;j++) 						
		timerTable[j].active=false;	
}

void WSDFN::programmerNodes()
{
	programmingID=0;
	setTimer(START_PROGRAMMING_NODES, 0);
}

void WSDFN::programmingANode(int i){
	WSDFNPack *netPacket;
	union {
		char userData[30];
		struct flowEntry fe;
	} flowEntryPacket;

	union {
		char userData[30];
		struct timerEntry te;
	} timerEntryPacket;

	union {
		char userData[30];
		struct actionEntry ae;
	} actionEntryPacket;
	
	int cni=atoi(controlledNodes[i].c_str());
	#ifdef DEBUGFLOW
	trace() << "Programming CNT: " <<i << ", node: " << cni;
	trace() << "sizeof(timerEntryPacket.te): "<< sizeof(timerEntryPacket.te) ;
	trace() << "sizeof(actionEntryPacket.ae): "<< sizeof(actionEntryPacket.ae) ;
	trace() << "sizeof(flowEntryPacket.fe): "<< sizeof(flowEntryPacket.fe) ;
	#endif
	programPackages[cni]=0;
	programmedPackages[cni]=0;
	for(int j=0; j< SIZEOF_FLOW_TABLE;j++) 						
		if(flowTableC[j][ cni ].active) {
			flowEntryPacket.fe=flowTableC[j][cni];
			netPacket = new WSDFNPack("Programming FT Packet", NETWORK_LAYER_PACKET);
			netPacket->setSource(SELF_NETWORK_ADDRESS);
			netPacket->setDestination(controlledNodes[i].c_str());
			netPacket->setWSDFNPackKind(PROGRAMMING);
			netPacket->setUserData(0,FLOW_TABLE_PACKET);
			for(int k=0; k<29; k++)
				netPacket->setUserData(k+1,flowEntryPacket.userData[k]);
			toMacLayer(netPacket, atoi(getTOForwardTO(controlledNodes[i]).c_str()));
			collectOutput("WSDFN >> Qtd controller packet sent by everyone");
			totalControllerPackages++;
			programPackages[cni]++;
			#ifdef DEBUGFLOW
			trace() << 	"Programming FT[" <<j<< "] from " << SELF_NETWORK_ADDRESS << " to " << controlledNodes[i] <<
						" using " << getTOForwardTO(controlledNodes[i]);
			#endif
		}
	for(int j=0; j< SIZEOF_TIMER_TABLE;j++) 						
		if(timerTableC[j][ cni ].active) {
			timerEntryPacket.te=timerTableC[j][cni];
			netPacket = new WSDFNPack("Programming TT Packet", NETWORK_LAYER_PACKET);
			netPacket->setSource(SELF_NETWORK_ADDRESS);
			netPacket->setDestination(controlledNodes[i].c_str());
			netPacket->setWSDFNPackKind(PROGRAMMING);
			netPacket->setUserData(0,TIMER_TABLE_PACKET);
			for(int k=0; k<29; k++)
				netPacket->setUserData(k+1,timerEntryPacket.userData[k]);
			toMacLayer(netPacket, atoi(getTOForwardTO(controlledNodes[i]).c_str()));
			collectOutput("WSDFN >> Qtd controller packet sent by everyone");
			totalControllerPackages++;
			programPackages[cni]++;
			#ifdef DEBUGFLOW
			trace() << 	"Programming TT[" <<j<< "] from " << SELF_NETWORK_ADDRESS << " to " << controlledNodes[i] <<
						" using " << getTOForwardTO(controlledNodes[i]);
			#endif
			//displayTimerEntry(timerEntryPacket.te);
			//trace() << "Data Send: ";
			//displayData(timerEntryPacket.userData);
		}
	for(int j=0; j< SIZEOF_ACTION_TABLE;j++) 						
		if(actionTableC[j][ cni ].active) {
			actionEntryPacket.ae=actionTableC[j][cni];
			netPacket = new WSDFNPack("Programming AT Packet", NETWORK_LAYER_PACKET);
			netPacket->setSource(SELF_NETWORK_ADDRESS);
			netPacket->setDestination(controlledNodes[i].c_str());
			netPacket->setWSDFNPackKind(PROGRAMMING);
			netPacket->setUserData(0,ACTION_TABLE_PACKET);
			for(int k=0; k<29; k++)
				netPacket->setUserData(k+1,actionEntryPacket.userData[k]);
			toMacLayer(netPacket, atoi(getTOForwardTO(controlledNodes[i]).c_str()));
			collectOutput("WSDFN >> Qtd controller packet sent by everyone");
			totalControllerPackages++;
			programPackages[cni]++;
			#ifdef DEBUGFLOW
			trace() << "Programming AT[" <<j<< "] from " << SELF_NETWORK_ADDRESS << " to " << controlledNodes[i] <<
			" using " << getTOForwardTO(controlledNodes[i]);
			#endif
		}
	#ifdef DEBUGFLOW
	trace() << "#Total Programming packages to " << controlledNodes[i] << " is " << programPackages[atoi(controlledNodes[i].c_str())];
	#endif

} 


void WSDFN::startupIR()
{

	declareOutput("WSDFN >> Qtd controller packet sent by everyone");
	//Using: collectOutput("WSDFN >> Qtd controller packet sent by everyone");
	declareOutput("WSDFN >> Qtd data packet sent by everyone");
	//Using: collectOutput(""WSDFN >> Qtd data packet sent by everyone"");
	
	totalDataPackages=0;
	totalControllerPackages=0;

	this->lowRssi = par("lowRssi").doubleValue();
	this->discoveryTime = par("discoveryTime").doubleValue();	
	this->discoveryDuration = par("discoveryDuration").doubleValue();
	this->reportTime = par("reportTime").doubleValue();
	this->reportDuration = par("reportDuration").doubleValue();
	this->reportFirstTime = par("reportFirstTime").doubleValue();
	this->reportInterval = par("reportInterval");				
	this->reportVerify = par("reportVerify").doubleValue();		
	this->programmingTime = par("programmingTime").doubleValue();	
	this->programmingDuration = par("programmingDuration").doubleValue();
	this->programmingFirstTime = par("programmingFirstTime").doubleValue();
	this->programmingInterval = par("programmingInterval").doubleValue();
	

			

	this->delayController = par("delayController").doubleValue();


	lastFT=-1;	
	if(this->isController){
			level=0;
			lastCN=-1;	
			setTimer(START_DISCOVERY_CONTROLLER_PHASE, discoveryTime); //Wait discoveryTime seconds for Startup of Sensors
			setTimer(STOP_DISCOVERY_CONTROLLER_PHASE, discoveryDuration);
			setTimer(START_REPORT_PHASE, reportFirstTime);
			setTimer(START_PROGRAMMING_PHASE, programmingFirstTime+delayController);
		}
	else {
		level=1000;
		lastNB=-1;
		setTimer(START_DISCOVERY_CONTROLLER_PHASE, 0); //Start Discovery Controller Phase immediately
		setTimer(STOP_DISCOVERY_CONTROLLER_PHASE, discoveryDuration);
		setTimer(START_REPORT_PHASE, reportFirstTime+delayController);
		setTimer(START_PROGRAMMING_PHASE, programmingFirstTime);
	
	}


	trace() << "Startup Network Protocol ";



}

void WSDFN::finish()
{

}



#endif				//_IR_H_
