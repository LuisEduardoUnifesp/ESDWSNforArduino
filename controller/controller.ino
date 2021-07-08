/**
  Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 2 as published by the Free Software Foundation.
*/

/**
   Example for Getting Started with nRF24L01+ radios.

   This is an example of how to use the RF24 class.  Write this sketch to two
   different nodes.  Put one of the nodes into 'transmit' mode by connecting
   with the serial monitor and sending a 'T'.  The ping node sends the current
   time to the pong node, which responds by sending the value back.  The ping
   node can then see how long the whole cycle took.
*/


#define DEBUG
#define USE_CSMA true
//#define DUE

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include "printf.h"




#include "Csmaca.h"
#include "Package.h"
#include "PackageQueue.h"

#if !defined(__arm__)
#include "EEPROMAnything.h"
#endif

int numberOfNeighbors = 0;
int numberOfFarNodes = 0;


#include "structs.h"


#define DELAY_SEND 5000 //microseconds
#define TOTAL_SEND 1000
#define TOTALTIME 200000 //Total time
#define BROADCAST 0xFFFF




//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio1(7, 9);


Package *pac;
uint16_t myAddress = 1;
bool isController = false;
int dataPackages = 0;
int totalDataPackages = 0;
Csmaca *csmaca;

unsigned int central = 0;
unsigned long repeat, timeCount, timezero, adjust;


int sent = 0;
bool sentAll = false;
int scenario = 0;

//Application Level
int level;
uint16_t nexthop;
uint16_t controller;
uint16_t sink;
int programmingID;
int programPackages[NUMBER_NODES];
int programmedPackages[NUMBER_NODES];
bool reportNeighborhoodReceived[NUMBER_NODES];
uint16_t controlledNodes[NUMBER_NODES];
int lastCN = -1;


enum appTypeStatus { STARTING, DISCOVERING, REPORTING, WAITING_CONFIRMATION_REPORT  };
appTypeStatus appStatus;
enum timersFase {
  TIMER_0,
  TIMER_1,
  TIMER_2,
  TIMER_3,
  TIMER_4,
  START_DISCOVERY_CONTROLLER_PHASE,
  STOP_DISCOVERY_CONTROLLER_PHASE,
  START_REPORT_PHASE,
  VERIFY_REPORT_SEND,
  STOP_REPORT_PHASE,
  START_PROGRAMMING_PHASE,
  START_PROGRAMMING_NODES,
  STOP_PROGRAMMING_PHASE,
  TX_CONTROL
};


/**
   Struct for save configuration in EEPROM
*/
struct config_CSMA configuration;

#include "wsdfn.h"
//#include "tablesReportN.h"
//#include "tablesPingPong.h"
//#include "Tables2021.1s.h"
#include "TablesPingPong1s.h"


//Application Layer =============================================================
void isTimerFired() {
  unsigned long now = millis() + adjust;

  for (int i = 0; i < SIZEOF_TIMER_TABLE; i++) {
    if ( (timerTable[i].active) && ((timerTable[i].executed < timerTable[i].looping)|| timerTable[i].looping==255 ) && (now > (timerTable[i].t0 * timerTable[i].multiply )) ) {
      //if looping==255 the looping is not stop.
      Serial.print(F("\nFired: "));
      Serial.print(timerTable[1].timerID);
      Serial.print(F(" in: "));
      Serial.println(micros());
      //call timers treatment

      //update data for next timer call
      timerTable[i].executed++;
      timerTable[i].t0 = now + (timerTable[i].countdown * timerTable[i].multiply );

      //if is programmable timer
      if ((timerTable[i].timerID >= 0) && (timerTable[i].timerID < SIZEOF_PROGRAMMABLE_TIMER_TABLE)) {
        executeAction(timerTable[i].actionID, new Package());
      } else
        timerFiredCallback(timerTable[i].timerID);



    }
  }
}



void programmerNodes() {
  programmingID = 0;

  //set timer for next node in 0,2s
  timerTable[START_PROGRAMMING_NODES].timerID = START_PROGRAMMING_NODES;
  timerTable[START_PROGRAMMING_NODES].looping = 1;
  timerTable[START_PROGRAMMING_NODES].executed = 0;
  timerTable[START_PROGRAMMING_NODES].t0 = millis() + 200ul;
  timerTable[START_PROGRAMMING_NODES].countdown = 0;
  timerTable[START_PROGRAMMING_NODES].multiply = 1;
  timerTable[START_PROGRAMMING_NODES].active = true;
}

void programmingANode(int i) {
  Package *netPacket = new Package();
  union {
    struct flowEntry fe;
    uint8_t userData[17];
  } flowEntryPacket;
  struct flowEntry fea;

  union {
    struct timerEntry te;
    uint8_t userData[17];
  } timerEntryPacket;

  union {
    struct actionEntry ae;
    uint8_t userData[17];
  } actionEntryPacket;

  uint8_t paux[17];

  int cni = controlledNodes[i];
  Serial.print("Programming node: ");
  Serial.println(cni);
  programPackages[cni] = 0;
  programmedPackages[cni] = 0;
  for (int j = 0; j < SIZEOF_FLOW_TABLE; j++)
    if (flowTableC[j][ cni ].active) {
      flowEntryPacket.fe = flowTableC[j][cni];
      printFlowEntry(flowEntryPacket.fe);
      netPacket->setFinalSource(csmaca->getMyAddress());
      netPacket->setFinalDestination(cni);
      netPacket->setDestination(getFarNodeNextHop(cni));
      netPacket->setType(PROGRAMMING_FLOW);
      toSerialFlowEntry(flowTableC[j][cni], paux);
      for (int k = 0; k < 17; k++) {
        netPacket->setUserData(paux[k], k);
      }


      csmaca->send(netPacket, getFarNodeNextHop(cni));
      netPacket->print();
      programPackages[cni]++;
    }
  for (int j = 0; j < SIZEOF_ACTION_TABLE; j++)
    if (actionTableC[j][ cni ].active) {
      actionEntryPacket.ae = actionTableC[j][cni];
      printActionEntry(actionTableC[j][cni]);
      netPacket->setFinalSource(csmaca->getMyAddress());
      netPacket->setFinalDestination(cni);
      netPacket->setType(PROGRAMMING_ACTION);
      toSerialActionEntry(actionTableC[j][cni], paux);
      for (int k = 0; k < 17; k++)
        netPacket->setUserData(paux[k], k);
      csmaca->send(netPacket, getFarNodeNextHop(cni));
      netPacket->print();
      programPackages[cni]++;
    }
  for (int j = 0; j < SIZEOF_PROGRAMMABLE_TIMER_TABLE ; j++)
    if (timerTableC[j][ cni ].active) {
      timerEntryPacket.te = timerTableC[j][cni];
      printTimerEntry(timerTableC[j][cni]);
      netPacket->setFinalSource(csmaca->getMyAddress());
      netPacket->setFinalDestination(cni);
      netPacket->setType(PROGRAMMING_TIMER);
      toSerialTimerEntry(timerTableC[j][cni], paux);
      for (int k = 0; k < 17; k++)
        netPacket->setUserData(paux[k], k);
      csmaca->send(netPacket, getFarNodeNextHop(cni));
      netPacket->print();
      programPackages[cni]++;
    }



}


void printTableControls() {
  for (int j = 0; j < SIZEOF_FLOW_TABLE; j++)
    if (flowTable[j].active) {
      printFlowEntry(flowTable[j]);
    }
  for (int j = 0; j < SIZEOF_ACTION_TABLE; j++)
    if (actionTable[j].active) {
      printActionEntry(actionTable[j]);
    }
  for (int j = 0; j < SIZEOF_TIMER_TABLE ; j++)
    if (timerTable[j].active) {
      printTimerEntry(timerTable[j]);
    }
}

boolean programmingFail() {
  //TODO programming Fail
  return false;
}
boolean afterLastID(int Id) {
  return (Id > lastCN);
}

void saveToControlledNodes(uint16_t source) {
  int x;
  for (x = 0; x <= lastCN; x++) {
    if (controlledNodes[x] == source) {
      return;
    }
  }
  lastCN++;
  if (lastCN < NUMBER_NODES) controlledNodes[lastCN] = source;
}



void timerFiredCallback(int kind) {
  //#ifdef DEBUG
  Serial.print(F("\nFireCallback:"));
  Serial.print(kind);
  //#endif
  switch (kind) {

    /** --  */
    case START_DISCOVERY_CONTROLLER_PHASE: //START_DISCOVERY_CONTROLLER_PHASE
      {
        appStatus = DISCOVERING;
        Serial.print(F("\nStarting Discovery Controller PHASE"));
        Serial.print(F("\nReceiver On..."));
        csmaca->wakeUp();

        if (isController) {
          Serial.print(F("\nDiscovery Packet Sending"));
          Package *package = new Package();
          package->setSource(csmaca->getMyAddress());
          package->setDestination(BROADCAST);
          package->setLevel(0);
          package->setType(CONTROLLER_DISCOVERY);
          package->setFinalSource(csmaca->getMyAddress());
          //package->setUserData(csmaca->getMyAddress() && 0x00ff, 0);
          //package->setUserData(csmaca->getMyAddress() >> 8, 1);
          csmaca->send(package, BROADCAST);
          adjust = 12000ul - millis();
          if (!csmaca->isTxQueueEmpty())
            Serial.print(F("\nDiscovery Packet Sended"));
          else
            Serial.print(F("\nDiscovery Packet ERROR Send"));
        }
        return;
      } //end case StartDiscovery
    /** --  */
    case STOP_DISCOVERY_CONTROLLER_PHASE: //STOP_DISCOVERY_CONTROLLER_PHASE
      {
        //this->csma->fallAsleep();
        Serial.print(F("\nStop Discovery Controller PHASE"));
        return;
      }
    /** --  */
    case START_REPORT_PHASE: //START_REPORT_PHASE
      {

        appStatus = REPORTING;
        for (int i = 0; i < NUMBER_NODES; i++) {
          reportNeighborhoodReceived[i] = false;
          controlledNodes[i] = -1;
        }
        clearFarNode();
        Serial.print(F("\nStarting Report PHASE"));
        //Wait for Reporting message from sensors
        csmaca->wakeUp();
        return;
      }
    /** --  */
    case STOP_REPORT_PHASE: //STOP_REPORT_PHASE
      {
        Serial.println(F("\nStop Report PHASE: #"));
        for (int i = 0; i < NUMBER_NODES; i++) {
          Serial.print("i:");
          Serial.println(i);
          Serial.print("ReportNReceived:" );
          Serial.println(reportNeighborhoodReceived[i]);
          Serial.print("ControlledNodes:" );
          Serial.println(controlledNodes[i]);
        }
        Serial.println(F("\nFarNode: \n"));
        printFarNode();
        Serial.println(F("\nNeighborhood: \n"));
        printNeighborhood();
        return;
      }
    /** --  */
    case START_PROGRAMMING_PHASE:
      {
        Serial.print(F("\nStart Programming PHASE"));
        programmerNodes();
        return;
      }
    case START_PROGRAMMING_NODES:
      {
        //TODO in case of transmission fails, resend programmations to last node.
        /* if (programmingID != 0) {
           if (programmedPackages[controlledNodes[programmingID - 1]] !=
               programPackages[controlledNodes[programmingID - 1]]) programmingID--;
           else if (afterLastID(programmingID)) return;
          } */

        if (afterLastID(programmingID)) {
          Serial.print(F("\nLast Id Programmed"));
          return;
        }

        Serial.print(F("\nStart Programming Node: "));
        Serial.println(programmingID);
        //set timer for next node in 1s
        timerTable[START_PROGRAMMING_NODES].timerID = START_PROGRAMMING_NODES;
        timerTable[START_PROGRAMMING_NODES].looping = 1;
        timerTable[START_PROGRAMMING_NODES].executed = 0;
        timerTable[START_PROGRAMMING_NODES].t0 = millis() + 1500ul;
        timerTable[START_PROGRAMMING_NODES].countdown = 0;
        timerTable[START_PROGRAMMING_NODES].multiply = 1;
        timerTable[START_PROGRAMMING_NODES].active = true;
        Serial.print(F("\nNext Programming after: "));
        Serial.print(millis());
        Serial.println(F(" is: "));
        printTimerEntry(timerTable[START_PROGRAMMING_NODES]);


        programmingANode(programmingID);
        programmingID++;







        return;
      }

    case STOP_PROGRAMMING_PHASE:
      {
        //TO_DO Stop radio rx
        Serial.print(F("\nStop Programming PHASE"));
        printTableControls();
        //scheduleTimers();
        //#ifdef DEBUGFLOW
        //        //displayTables();
        //#endif
        //trace() << "Total Controller Package: " << totalControllerPackages << " ; " << SELF_NETWORK_ADDRESS ;
        //        trace() << "Receiver Off...";
        //        setStateSleep();
        return;
      }

    case TX_CONTROL:
      {
        return;
      }




  } //end Switch

}


void printNeighborhood() {
  Serial.print("\nNeighbors: ");
  for (int i = 0; i < numberOfNeighbors; i++) {
    Serial.print(neighborsTable[i].address);
    Serial.print(": ");
  }
  Serial.print("\n");
}

void saveNeighbor(int neighbor) {
  if (numberOfNeighbors < SIZEOF_NEIGHBORS_TABLE) {
    neighborsTable[numberOfNeighbors].address = neighbor;
    numberOfNeighbors++;
  }
}




void saveFarNode(uint16_t farNode, uint16_t nextHop) {
  if (numberOfFarNodes < SIZEOF_FARNODE_TABLE) {
    //not duplicate destination
    for (int x = 0; x < numberOfFarNodes; x++) {
      if (farNodeTable[x].farNode = farNode) {
        return;
      }
    }
    farNodeTable[numberOfFarNodes].farNode = farNode;
    farNodeTable[numberOfFarNodes].nextHop = nextHop;
    numberOfFarNodes++;
  }
}

void clearFarNode() {
  for (int i = 0; i < SIZEOF_FARNODE_TABLE; i++) {
    farNodeTable[i].farNode = -1;
    farNodeTable[i].nextHop = -1;
  }
  numberOfFarNodes = 0;
}

void printFarNode() {
  for (int i = 0; i < numberOfFarNodes; i++) {
    Serial.print(F("\nFarNode : "));
    Serial.print(farNodeTable[i].farNode);
    Serial.print(F(", NextHop : "));
    Serial.print(farNodeTable[i].nextHop);
  }
}



void received(Package *package, bool signal) {
  Serial.print("\nReceived packet of: ");
  Serial.print(package->getSource());
  Serial.print("\nSignal: ");
  Serial.print(signal);

  //package->print();


  //======================================================
  //Checking if isn't Data Packet
  if (package->getType() == DATA ) {

    Serial.println(F("\nReceived Data Package"));

    //only Data packet here
    uint8_t action = verifyPacket(package);
    if (action == 255) Serial.println(F("\nFlow not found..."));
    else executeAction(action, package);
    return;
  }

  //========================================================
  //Checking if Report Packet to Controller
  if ( (package->getDestination() == controller ||
        package->getDestination() == csmaca->getMyAddress() ) &&
       (package->getType() ==  REPORT_NEIGHBORHOOD_PACKET)
     ) {
    Serial.print(F("\nReceived Report_Neighborhood of: "));
    Serial.print(package->getSource());

    //TODO Save data from nodes
    Serial.print(F("\nReceived Report_Neighborhood from : "));
    Serial.print(package->getFinalSource());
    Serial.print(F(", by : "));
    Serial.print(package->getSource());
    if (package->getFinalSource() != package->getSource())
      saveFarNode(package->getFinalSource(), package->getSource());
    else
      saveNeighbor(package->getSource());


    //Send Report and Report Neighborhood Confirmation
    reportNeighborhoodReceived[package->getFinalSource()] = true;

    for (int i = 0; i < (SIZEOF_NEIGHBORS_TABLE * 2); i += 2) {
      int aux = (((uint16_t) package->getUserData(i)) << 8) | package->getUserData(i + 1);
      Serial.print(F("\nNeighbor : "));
      Serial.print(aux);
    }
    return;
  }
  //========================================================

  //========================================================
  //Checking if Report_confirmation Packet to Controller to PING
  if ( (package->getDestination() == controller ||
        package->getDestination() == csmaca->getMyAddress() ) &&
       (package->getType() ==  REPORT_CONFIRMATION )
     ) {
    Serial.print(F("\nReceived Report confirmation from : "));
    Serial.print(package->getFinalSource());
  }


  //========================================================
  //Checking if Report Packet to Controller
  if ( (package->getDestination() == controller ||
        package->getDestination() == csmaca->getMyAddress() ) &&
       (package->getType() ==  REPORT )
     ) {
    Serial.print(F("\nReceived Report packet of: "));
    Serial.print(package->getSource());

    //TODO Save data from nodes
    Serial.print(F("\nReceived Report from : "));
    Serial.print(package->getFinalSource());
    Serial.print(F(", by : "));
    Serial.print(package->getSource());
    if (package->getFinalSource() != package->getSource())
      saveFarNode(package->getFinalSource(), package->getSource());
    else
      saveNeighbor(package->getSource());
    //Send Report and Report Neighborhood Confirmation
    Serial.print("Neighborhood received before: ");
    Serial.print(reportNeighborhoodReceived[package->getFinalSource()]);
    if (reportNeighborhoodReceived[package->getFinalSource()]) {
      saveToControlledNodes(package->getFinalSource());
      package->setType(REPORT_CONFIRMATION);
      package->setFinalDestination(package->getFinalSource());
      delay(100);
      csmaca->send(package, package->getSource());
      Serial.print(F("\nSend report confirmation to "));
      Serial.println(package->getFinalDestination());
      
    } else {
      Serial.print("\nNot Received Report Neighborhood Package...");
    }
    return;
  }
  //========================================================




  //====================================================
  //Checking if Programming Confirmation Packet to Controller
  if (package->getType() == PROGRAMMING_CONFIRMATION ) {
    //It isn't a destination?
    if (package->getDestination() == csmaca->getMyAddress()) {
      programmedPackages[(package->getFinalSource())]++;
    }
  }





}



//=========================================================================


void setup(void)
{

  



  //
  // Print preamble
  //

  repeat = micros();
  Serial.begin(115200);//57600);
  printf_begin();
  Serial.print(F("\n\r~/home/llima/Dropbox/Doutorado/arduino/testes/ESDWSN/controller/controller.ino/\n\r"));


 // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  startTables(NUMBER_NODES-1); //Number of tables is one unit unless the number of nodes
//  Serial.print(F("\n:out: flowTable[0].active: "));
//  Serial.println(flowTable[0].active);
  level = 1000;
  adjust = 0;
  timezero = millis();
  //START_DISCOVERY_CONTROLLER_PHASE start in 12s
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].timerID = START_DISCOVERY_CONTROLLER_PHASE;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].looping = 1;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].executed = 0;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].t0 = timezero + 12000ul;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].countdown = 0;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].multiply = 1;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].active = true;

  //STOP_DISCOVERY_CONTROLLER_PHASE stop in 40s
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].timerID = STOP_DISCOVERY_CONTROLLER_PHASE;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].looping = 1;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].executed = 0;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].t0 = timezero + 40000ul;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].countdown = 0;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].multiply = 1;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].active = true;

  //START_REPORT_PHASE start in 40s
  timerTable[START_REPORT_PHASE].timerID = START_REPORT_PHASE;
  timerTable[START_REPORT_PHASE].looping = 1;
  timerTable[START_REPORT_PHASE].executed = 0;
  timerTable[START_REPORT_PHASE].t0 = timezero + 40000ul;
  timerTable[START_REPORT_PHASE].countdown = 0;
  timerTable[START_REPORT_PHASE].multiply = 1;
  timerTable[START_REPORT_PHASE].active = true;

  timerTable[VERIFY_REPORT_SEND].active = false;

  //STOP_REPORT_PHASE stop in 120s
  timerTable[STOP_REPORT_PHASE].timerID = STOP_REPORT_PHASE;
  timerTable[STOP_REPORT_PHASE].looping = 1;
  timerTable[STOP_REPORT_PHASE].executed = 0;
  timerTable[STOP_REPORT_PHASE].t0 = timezero + 120000ul;
  timerTable[STOP_REPORT_PHASE].countdown = 0;
  timerTable[STOP_REPORT_PHASE].multiply = 1;
  timerTable[STOP_REPORT_PHASE].active = true;

  //START_PROGRAMMING_PHASE stop in 121s 2min
  timerTable[START_PROGRAMMING_PHASE].timerID = START_PROGRAMMING_PHASE;
  timerTable[START_PROGRAMMING_PHASE].looping = 1;
  timerTable[START_PROGRAMMING_PHASE].executed = 0;
  timerTable[START_PROGRAMMING_PHASE].t0 = timezero + 125000ul;
  timerTable[START_PROGRAMMING_PHASE].countdown = 0;
  timerTable[START_PROGRAMMING_PHASE].multiply = 1;
  timerTable[START_PROGRAMMING_PHASE].active = true;


  //STOP_PROGRAMMING_PHASE stop in 300s 5min
  timerTable[STOP_PROGRAMMING_PHASE].timerID = STOP_PROGRAMMING_PHASE;
  timerTable[STOP_PROGRAMMING_PHASE].looping = 1;
  timerTable[STOP_PROGRAMMING_PHASE].executed = 0;
  timerTable[STOP_PROGRAMMING_PHASE].t0 = timezero + 180000ul;
  timerTable[STOP_PROGRAMMING_PHASE].countdown = 0;
  timerTable[STOP_PROGRAMMING_PHASE].multiply = 1;
  timerTable[STOP_PROGRAMMING_PHASE].active = true;


  //TX_CONTROL start in 301s 5min1s
  timerTable[TX_CONTROL].timerID = TX_CONTROL;
  timerTable[TX_CONTROL].looping = 1;
  timerTable[TX_CONTROL].executed = 0;
  timerTable[TX_CONTROL].t0 = timezero + 181000ul;
  timerTable[TX_CONTROL].countdown = 0;
  timerTable[TX_CONTROL].multiply = 1;
  timerTable[TX_CONTROL].active = true;

  //==============================================================================



  

#if !defined(__arm__)
  // Read address from EEPROM
  EEPROM_read(0, configuration);
  Serial.print(F("Current settings in EEPROM\n"));
#endif
#if defined(__arm__)
  configuration.address = 0;
#endif
  Serial.print(F("Node address: "));
  Serial.print(configuration.address);
  myAddress = configuration.address;
  if (myAddress == 0) isController = true;
  else isController = false;

  //
  // Setup and configure rf radio
  //

  radio1.begin();
  radio1.setPALevel(RF24_PA_MAX);
  radio1.setDataRate(RF24_2MBPS);//RF24_250KBPS);
  radio1.setCRCLength(RF24_CRC_16);//RF24_CRC_DISABLED);
  radio1.setChannel(12); //124
  // optionally, increase the delay between retries & # of retries
  radio1.setRetries(0, 0);
  radio1.setAutoAck(false);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio1.setPayloadSize(8);


  csmaca = new Csmaca(&radio1, myAddress, USE_CSMA);

  //csmaca->getEsdwdn()->setIsController(isController);

  //  printf("\nIsController: %d",csmaca->getEsdwdn()->isController());
  Serial.print(F("\nIsController: "));
  Serial.print(isController);
  Serial.flush();

  // Setting parameters: timetorandom,  numberofslots, mintime
  csmaca->setTimetorandom(TIMETORANDOM);
  csmaca->setNumberofslots(NUMBEROFSLOTS);
  csmaca->setMintime(MINTIME);
  pac = new Package();
  timeCount = millis();

 


}




void loop(void)
{

  csmaca->loop();
  isTimerFired();
  /*

    //verify time for send new package
    if (micros() - repeat > DELAY_SEND) {

      //TODO 5ms what?

      if ((millis() - timeCount) > TOTALTIME) {
        Serial.println(F("--"));
        Serial.println(scenario);
        Serial.println(sent);
        csmaca->printStatus();
        csmaca->clearStatistics();
        sent = 0;
        sentAll = false;
        timeCount = millis();
        scenario++; if (scenario == 45) scenario = 0;
        // Setting parameters: timetorandom, mintime,  numberofslots
        csmaca->setTimetorandom(TIMETORANDOM);
        csmaca->setNumberofslots(NUMBEROFSLOTS);
        csmaca->setMintime(MINTIME);
        pac->setScenario(scenario);

      }


      repeat = micros();
    }*/



}
// vim:cin:ai:sts=2 sw=2 ft=cpp
