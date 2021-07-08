



bool isNeighbor(int neighbor) {
  for (int i = 0; i < numberOfNeighbors; i++)
    if (neighborsTable[i].address == neighbor) return true;
  return false;
}

uint16_t getFarNodeNextHop(uint16_t farNode) {
  if (isNeighbor(farNode)) return farNode;
  for (int i = 0; i < numberOfFarNodes; i++) {
    if ( farNodeTable[i].farNode == farNode) {
      return farNodeTable[i].nextHop;
    }
  }
  return (BROADCAST);
}




/***********************************
     Checks whether a data packet belogs to a dataflow in the flow table.
    If it belongs, the function return the ActionID corresponding to the flow
    If not, return -1
*/
uint8_t verifyPacket(Package *netPacket)
{
#ifdef DEBUGFLOW
  //  trace() << "verifing: d0:" << (int)netPacket->getUserData(0);
  //  trace() << "verifing: d1:" << (int)netPacket->getUserData(1);
  //  trace() << "verifing: source:" << netPacket->getSource();
  //  trace() << "verifing: destination:" << netPacket->getDestination();
#endif
  bool find = false;
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
  sixteen.b.d1 = netPacket->getUserData(0);
  sixteen.b.e1 = netPacket->getUserData(1);
  sixteen.b.d2 = netPacket->getUserData(2);
  sixteen.b.e2 = netPacket->getUserData(3);

  for (int i = 0; i < SIZEOF_FLOW_TABLE; i++) {

#ifdef DEBUGFLOW
    //    trace() << "FT: " << i;
    //    trace() << "INT_8: " << (int)INT_8;
    //    trace() << "flowTable["<<i<<"].actionID " << (int) flowTable[i].actionID;
    //    trace() << "flowTable["<<i<<"].fa.sourceAddress " << flowTable[i].fa.sourceAddress;
    //    trace() << "flowTable["<<i<<"].fa.sourceMask " << flowTable[i].fa.sourceMask;
    //    trace() << "flowTable["<<i<<"].fa.destinationAddress " << flowTable[i].fa.destinationAddress;
    //    trace() << "flowTable["<<i<<"].fa.destinationMask " << flowTable[i].fa.destinationMask;
    //    trace() << "flowTable["<<i<<"].fc.dataType " << (int)flowTable[i].fc.dataType;
    //    trace() << "flowTable["<<i<<"].fc.operatorC " << (int)flowTable[i].fc.operatorC;
    //    trace() << "flowTable["<<i<<"].fc.value.valueInt8_1 " << (int)flowTable[i].fc.value.valueInt8_1;
#endif
        if(!flowTable[i].active){
    //#ifdef DEBUGFLOW
    //      trace() << "not active...";
    //#endif
          continue;
        }


    switch (flowTable[i].fc.operatorC)
    {
      case NONE:
        find = true;
        break;
      case E:
        //if (flowTable[i].fc.dataType == INT_8)
        //   if (netPacket->getUserData(0) == flowTable[i].fc.value.valueInt8_1) find = true;
        if (true)//flowTable[i].fc.dataType == INT_16)
          if (sixteen.a1 == flowTable[i].fc.value1/*.valueInt16_1*/) find = true;
        break;
      case D:
        //if (flowTable[i].fc.dataType == INT_8)
        //  if (netPacket->getUserData(0) != flowTable[i].fc.value.valueInt8_1) find = true;
        if (true)//flowTable[i].fc.dataType == INT_16)
          if (sixteen.a1 != flowTable[i].fc.value1/*.valueInt16_1*/) find = true;
        break;
      case GT:
        //if (flowTable[i].fc.dataType == INT_8)
        //  if (netPacket->getUserData(0) > flowTable[i].fc.value.valueInt8_1) {
            //#ifdef DEBUGFLOW
            //            trace()<< "find True GT";
            //#endif
        //    find = true;
        //  }
        if (true)//flowTable[i].fc.dataType == INT_16)
          if (sixteen.a1 > flowTable[i].fc.value1/*.valueInt16_1*/) find = true;
        break;
      case GTOE:
        //if (flowTable[i].fc.dataType == INT_8)
        //  if (netPacket->getUserData(0) >= flowTable[i].fc.value.valueInt8_1) find = true;
        if (true)//flowTable[i].fc.dataType = INT_16)
          if (sixteen.a1 >= flowTable[i].fc.value1/*.valueInt16_1*/) find = true;
        break;
      case LT:
        //if (flowTable[i].fc.dataType == INT_8)
        //  if (netPacket->getUserData(0) < flowTable[i].fc.value.valueInt8_1) find = true;
        if (true)//flowTable[i].fc.dataType = INT_16)
          if (sixteen.a1 < flowTable[i].fc.value1/*.valueInt16_1*/) find = true;
        break;
      case LTOE:
        //if (flowTable[i].fc.dataType == INT_8)
        //  if (netPacket->getUserData(0) <= flowTable[i].fc.value.valueInt8_1) find = true;
        if (true)//flowTable[i].fc.dataType = INT_16)
          if (sixteen.a1 <= flowTable[i].fc.value1/*.valueInt16_1*/) find = true;
        break;
      case GT_A_LT:
        //if (flowTable[i].fc.dataType == INT_8)
        //  if (
        //    (netPacket->getUserData(0) > flowTable[i].fc.value.valueInt8_1) and
        //    (netPacket->getUserData(1) < flowTable[i].fc.value.valueInt8_2)
        //  ) find = true;
        //break;
        if (true)//flowTable[i].fc.dataType == INT_16)
          if (
            (sixteen.a1 > flowTable[i].fc.value1/*.valueInt16_1*/) and
            (sixteen.a2 < flowTable[i].fc.value2/*.valueInt16_2*/)
          ) find = true;
        break;
      case GTOE_A_LT:
        //if (flowTable[i].fc.dataType == INT_8)
        //  if (
        //    (netPacket->getUserData(0) >= flowTable[i].fc.value.valueInt8_1) and
        //    (netPacket->getUserData(1) < flowTable[i].fc.value.valueInt8_2)
        //  ) find = true;
        break;
        if (true) //flowTable[i].fc.dataType == INT_16)
          if (
            (sixteen.a1 >= flowTable[i].fc.value1/*.valueInt16_1*/) and
            (sixteen.a2 < flowTable[i].fc.value2/*.valueInt16_2*/)
          ) find = true;
        break;
      case GT_A_LTOE:
        /*if (flowTable[i].fc.dataType == INT_8)
          if (
            (netPacket->getUserData(0) > flowTable[i].fc.value.valueInt8_1) and
            (netPacket->getUserData(1) <= flowTable[i].fc.value.valueInt8_2)
          ) find = true;
        break;*/
        if (true) //flowTable[i].fc.dataType == INT_16)
          if (
            (sixteen.a1 > flowTable[i].fc.value1) and
            (sixteen.a2 <= flowTable[i].fc.value2)
          ) find = true;
        break;
      case GTOE_A_LTOE:
/*        if (flowTable[i].fc.dataType == INT_8)
          if (
            (netPacket->getUserData(0) >= flowTable[i].fc.value.valueInt8_1) and
            (netPacket->getUserData(1) <= flowTable[i].fc.value.valueInt8_2)
          ) find = true;
        break;*/
        if (true)//flowTable[i].fc.dataType == INT_16)
          if (
            (sixteen.a1 >= flowTable[i].fc.value1/*.valueInt16_1*/) and
            (sixteen.a2 <= flowTable[i].fc.value2/*.valueInt16_2*/)
          ) find = true;
        break;
    }

    maskedSource = (netPacket->getSource())&flowTable[i].fa.sourceMask;
    maskedDestination = (netPacket->getDestination())&flowTable[i].fa.destinationMask;
    //#ifdef DEBUGFLOW
    //    trace()<< "MS: " << maskedSource;
    //    trace()<< "MD: " << maskedDestination;
    //#endif
    if ((flowTable[i].fa.sourceAddress != maskedSource) ||
        (flowTable[i].fa.destinationAddress != maskedDestination)) find = false;
    //#ifdef DEBUGFLOW
    //    trace() << "Find: " << find;
    //#endif
    if (find) {
      flowTable[i].counters++;
#ifdef DEBUG      
      Serial.print(F("\nFlowFind: "));
      Serial.println(flowTable[i].flowID);
#endif     
      //printFlowEntry(flowTable[i]);
      return flowTable[i].actionID;
    }
  }  
#ifdef DEBUG  
  netPacket->print();
#endif  
  return -1;
}

/***********************************
   Execute the action defined by actionID in the actionTable

*/
void executeAction(uint16_t actionID, Package *netPacket)
{
  //string ssink;
  //stringstream out; out << sink; ssink = out.str();
  uint16_t sensorID, actionIDPos;
  int npackages = 1;
  //WSDFNPack *netPacket2;

  //  #ifdef DEBUGFLOW
  //  trace() << "Action: " << (int)actionID << " executed.";
  //  #endif

  switch (actionTable[actionID].actionType)
  {
    case A_READ_SENSOR:
      delete netPacket;
      for (int i = 0; i < npackages; i++) {
        //        #ifdef DEBUGFLOW
        //        trace() << "Read Sensor...";
        //        #endif
        sensorID = actionTable[actionID].value1; //.valueInt16_1;
        //Verify new data in FlowTable.
        Package *netPacket2 = new Package();
        //netPacket2 = new WSDFNPack("Data Packet", NETWORK_LAYER_PACKET);
        netPacket2->setSource(csmaca->getMyAddress());
        netPacket2->setDestination(sink);
        netPacket2->setType(DATA);
        netPacket2->setUserData(90 + (rand() % 2), 0); //sensor simulation between 90 and 92
        netPacket2->setUserData(55, 1); //value fixed for debug

        //        #ifdef DEBUGFLOW
        //        trace() << "Read >> " << (int) netPacket2->getUserData(0);
        //        trace() << "Verify FlowTable...";
        //        #endif
        actionIDPos = verifyPacket(netPacket2);
        if (actionIDPos == 255) { //TODO HERE
          //          #ifdef DEBUGFLOW
          //          trace() << "Flow not found... ";
          //          #endif
          Serial.println(F("\nFlow not found...."));            
        } else {
          //          #ifdef DEBUGFLOW
          //          trace() << "Execute Pos action: " << (int)actionIDPos << "...";
          //          #endif
          Serial.print(F("\nExecute Pos Action: "));            
          Serial.println(actionIDPos);            
          executeAction(actionIDPos, netPacket2);
        }
      }

      break;
    case A_RECEIVER_ON:
      delete netPacket;
      //      trace() << "Receiver On...";
      //TODO setStateRx();
      Serial.println(F("\nReceiver ON..."));
      csmaca->wakeUp();
      if (isController) {
        dataPackages = 0;
      }
      break;
    case A_RECEIVER_OFF:
      delete netPacket;
      //      trace() << "Receiver Off...";
      Serial.println(F("\nReceiver OFF..."));
      csmaca->fallAsleep();
      //TODO setStateSleep();

      //      if(this->isSink){
      //        trace() << "Packages received: " << dataPackages;
      //      }
      //      trace() << "Total Data Package : " << totalDataPackages << " ; " << SELF_NETWORK_ADDRESS;
      break;
    case A_SEND_TO_SINK:
      Serial.println(F("\nSend to Sink..."));
      timeSend = micros();
      //Serial.print(F("S;"));
      //Serial.println(micros());      
      //        #ifdef DEBUGFLOW
      //        trace() << "Send to Sink: " << ssink << " for:" << actionTable[actionID].value1.valueInt16_1 ;
      //        #endif
      //resend to next hop
      netPacket->setFinalDestination(sink);
      netPacket->setFinalSource(csmaca->getMyAddress());
      //nexthop to sink:actionTable[actionID].value1.valueInt16_1;
      //Serial.println(F("\nPacket Send to Sink: "));
      //netPacket->print();
      csmaca->send(netPacket, nexthop);
      //      toMacLayer(netPacket->dup(), actionTable[actionID].value1.valueInt16_1);
      //collectOutput("WSDFN >> Qtd data packet sent by everyone");      
      totalDataPackages++;
      break;
    case A_RETURN_TO_ORIGIN:
      Serial.println(F("Return to Origin..."));
      //        #ifdef DEBUGFLOW
      //        trace() << "Send to Sink: " << ssink << " for:" << actionTable[actionID].value1.valueInt16_1 ;
      //        #endif
      //resend to next hop
      netPacket->setFinalDestination(netPacket->getFinalSource());
      //nexthop to sink:actionTable[actionID].value1.valueInt16_1;
      csmaca->send(netPacket, getFarNodeNextHop(netPacket->getFinalDestination()));
      //      toMacLayer(netPacket->dup(), actionTable[actionID].value1.valueInt16_1);
      //collectOutput("WSDFN >> Qtd data packet sent by everyone");
      totalDataPackages++;
      break;      
    case A_DISPLAY:
      Serial.println(F("Display... "));
      timeDisplay = micros(); 
      Serial.print(F("T;")); 
      Serial.print(timeSend); 
      Serial.print(F(";"));  
      Serial.print(timeDisplay);           
      Serial.print(F(";"));  
      Serial.println(timeDisplay-timeSend);                 
      Serial.print(netPacket->getUserData(0));
      Serial.print(F(":"));
      Serial.println(netPacket->getUserData(1));
      //      trace() << "Display...";
      //      trace() << "Data receive from " << netPacket->getSource();
      //      trace() << "D0 " << (int) netPacket->getUserData(0);
      //      trace() << "D1 " << (int) netPacket->getUserData(1);
      dataPackages++;
      //      trace() << "dataPackages: " << dataPackages;
      delete netPacket;
      break;
  }
}
