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


//#define DEBUG
#define USE_CSMA true
//#define DUE

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"
#include "printf.h"


bool endSetup = false;




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
int totalProgrammingPackages;
Csmaca *csmaca;

unsigned int central = 0;
unsigned long repeat, timeCount, adjust;

uint32_t timeSend, timeDisplay;

int sent = 0;
bool sentAll = false;
int scenario = 0;

//Application Level
int level;
uint16_t nexthop;
uint16_t controller;
uint16_t sink;

bool reportConfirmed = false;
bool reportNeighborhoodConfirmed = false;


enum appTypeStatus { STARTING, DISCOVERING, REPORTING, NODE_PROGRAMMING  };
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


//Application Layer =============================================================

void clearTables() {
  totalProgrammingPackages = 0;
  //TODO ClearTables;
  for (int j = 0; j < SIZEOF_FLOW_TABLE; j++)
    flowTable[j].active = false;

  for (int j = 0; j < SIZEOF_ACTION_TABLE; j++)
    actionTable[j].active = false;


  for (int j = 0; j < SIZEOF_PROGRAMMABLE_TIMER_TABLE ; j++)
    timerTable[j].active = false;

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

      //if is programmable timer
      if ((timerTable[i].timerID >= 0) && (timerTable[i].timerID < SIZEOF_PROGRAMMABLE_TIMER_TABLE)) {
        executeAction(timerTable[i].actionID, new Package());
      } else
        timerFiredCallback(timerTable[i].timerID);
      //update data for next timer call
      timerTable[i].executed++;
      timerTable[i].t0 = now + (timerTable[i].countdown * timerTable[i].multiply );

    }
  }
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
        // Wait for CONTROLLER_DISCOVERY message
        return;
      } //end case StartDiscovery
    /** --  */
    case STOP_DISCOVERY_CONTROLLER_PHASE: //STOP_DISCOVERY_CONTROLLER_PHASE
      {
        //this->csma->fallAsleep();
        Serial.println(F("\nStop Discovery Controller PHASE"));
        return;        
        
      }
    /** --  */
    case START_REPORT_PHASE: //START_REPORT_PHASE
      {
        appStatus = REPORTING;
        reportConfirmed = false;
        reportNeighborhoodConfirmed = false;
        Serial.println(F("\nStarting Report PHASE"));

        //send neighboirs informations to sink
        Package *package = new Package();
        package->setFinalSource(csmaca->getMyAddress());
        package->setSource(csmaca->getMyAddress());
        package->setFinalDestination(controller);
        package->setType(REPORT_NEIGHBORHOOD_PACKET);
        int i, j;
        for (i = 0, j = 0; i < numberOfNeighbors; i++) {
          package->setUserData(neighborsTable[i].address >> 8, j); j++;
          package->setUserData(neighborsTable[i].address & 0x00ff, j); j++;
        }
        //if neighbors are less than SIZEOF_NEIGHBORS_TABLE complete table with broadcast (0xFFFF)
        if (j < (SIZEOF_NEIGHBORS_TABLE * 2)) {
          for (; j < (SIZEOF_NEIGHBORS_TABLE * 2); j++) {
            package->setUserData(0xFF, j);
          }
        }

        csmaca->send(package, nexthop);
        //package->print();



        //send outhers informations report to sink
        Package *package2 = new Package();
        package2 = new Package();
        package2->setFinalSource(csmaca->getMyAddress());
        package2->setSource(csmaca->getMyAddress());
        package2->setFinalDestination(controller);

        package2->setType(REPORT); //report

        //TODO Report package construction
        package2->setUserData(144, 0);
        package2->setUserData(133, 1);


        csmaca->send(package2, nexthop);
        //package2->print();

        return;
      }
    /** --  */
    case VERIFY_REPORT_SEND: //VERIFY_REPORT_SEND
      {
        if (appStatus == REPORTING) {
          boolean resend = false;
          /*if (reportNeighborhoodConfirmed) {
            Serial.print(F("\nReceived Report Neighborhood Confirmation in time: "));
            Serial.print(reportNeighborhoodConfirmed);
            } else {
            //resend neighboirs informations to sink
            Package *package = new Package();
            package->setFinalSource(csmaca->getMyAddress());
            package->setSource(csmaca->getMyAddress());
            package->setFinalDestination(controller);
            package->setType(REPORT_NEIGHBORHOOD_PACKET);
            int i, j;
            for (i = 0, j = 0; i < numberOfNeighbors; i++) {
              package->setUserData(neighborsTable[i].address & 0x00ff, j); j++;
              package->setUserData(neighborsTable[i].address >> 8, j); j++;
            }
            //if neighbors are less than SIZEOF_NEIGHBORS_TABLE complete table with broadcast (0xFFFF)
            if (j < (SIZEOF_NEIGHBORS_TABLE * 2)) {
              for (; j < (SIZEOF_NEIGHBORS_TABLE * 2); j++) {
                package->setUserData(0xFF, j);
              }
            }
            package->print();
            csmaca->send(package, nexthop);
            Serial.print(F("Neighborhood resended..."));
            resend = true;

            }*/

          if (reportConfirmed) {
            Serial.print(F("\nReceived Report Confirmation in time: "));
            Serial.print(reportConfirmed);
          } else {

            //TODO RESEND REPORT_PACKAGE and REPORT_NEIGHBORHOOD_PACKAGE
            //resend report informations to sink
            //send neighboirs informations to sink
            Package *package = new Package();
            package->setFinalSource(csmaca->getMyAddress());
            package->setSource(csmaca->getMyAddress());
            package->setFinalDestination(controller);
            package->setType(REPORT_NEIGHBORHOOD_PACKET);
            int i, j;
            for (i = 0, j = 0; i < numberOfNeighbors; i++) {
              package->setUserData(neighborsTable[i].address >> 8, j); j++;
              package->setUserData(neighborsTable[i].address & 0x00ff, j); j++;
            }
            //if neighbors are less than SIZEOF_NEIGHBORS_TABLE complete table with broadcast (0xFFFF)
            if (j < (SIZEOF_NEIGHBORS_TABLE * 2)) {
              for (; j < (SIZEOF_NEIGHBORS_TABLE * 2); j++) {
                package->setUserData(0xFF, j);
              }
            }

            csmaca->send(package, nexthop);
            //package->print();



            //send outhers informations report to sink
            Package *package2 = new Package();
            package2 = new Package();
            package2->setFinalSource(csmaca->getMyAddress());
            package2->setSource(csmaca->getMyAddress());
            package2->setFinalDestination(controller);

            package2->setType(REPORT); //report

            //TODO Report package construction
            package2->setUserData(144, 0);
            package2->setUserData(133, 0);


            csmaca->send(package2, nexthop);
            //package2->print();
            Serial.print(F("Report resended..."));
            resend = true;



          }
          if (resend) {
            //VERIFY_REPORT_SEND in 2s
            timerTable[VERIFY_REPORT_SEND].timerID = VERIFY_REPORT_SEND;
            timerTable[VERIFY_REPORT_SEND].looping = 1;
            timerTable[VERIFY_REPORT_SEND].executed = 0;
            timerTable[VERIFY_REPORT_SEND].t0 = millis() + 2000ul;
            timerTable[VERIFY_REPORT_SEND].countdown = 0;
            timerTable[VERIFY_REPORT_SEND].multiply = 1;
            timerTable[VERIFY_REPORT_SEND].active = true;
          }



          Serial.print(F("\nVerified Report PHASE"));
          return;
        }
      }
    /** --  */
    case STOP_REPORT_PHASE: //STOP_REPORT_PHASE
      {
        Serial.print(F("\nReceived Confirmation: "));
        Serial.print(reportConfirmed);
        Serial.print(F("\nReceived NeighborhoodConfirmation: "));
        Serial.print(reportNeighborhoodConfirmed);
        Serial.println(F("\nStop Report PHASE"));
        appStatus = NODE_PROGRAMMING;
        return;
      }
    case START_PROGRAMMING_PHASE:
      {
        Serial.print(F("\nStart Programming PHASE"));
        clearTables();
        return;
      }

    case STOP_PROGRAMMING_PHASE:
      {
        //TO_DO Stop radio rx
        Serial.println(F("\nStop Programming PHASE"));
        Serial.print(F("\nTotal Programming Packages; "));
        Serial.print(totalProgrammingPackages);
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

void saveNeighbor(uint16_t neighbor) {
  if (numberOfNeighbors < SIZEOF_NEIGHBORS_TABLE) {
    neighborsTable[numberOfNeighbors].address = neighbor;
    numberOfNeighbors++;
  }
}

void clearNeighbor() {
  for (int i = 0; i < SIZEOF_NEIGHBORS_TABLE; i++) {
    neighborsTable[i].address = -1;
  }
  numberOfNeighbors = 0;
}

void printNeighbor() {
  for (int i = 0; i < numberOfNeighbors; i++) {
    Serial.print(F("\nNeighbor : "));
    Serial.print(neighborsTable[i].address);
  }
}

void saveFarNode(uint16_t farNode, uint16_t nextHop) {
  if (numberOfFarNodes < SIZEOF_FARNODE_TABLE) {
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
  union {
    struct flowEntry fe;
    uint8_t userData[17];
  } flowEntryPacket;
  struct flowEntry fea;


  union {
    struct timerEntry te;
    uint8_t userData[17];
  } timerEntryPacket;
  struct timerEntry tea;

  union {
    struct actionEntry ae;
    uint8_t userData[17];
  } actionEntryPacket;
  struct actionEntry aea;

  uint8_t paux[17];

#ifdef DEBUG
  Serial.print(F("\nReceived packet of: "));
  Serial.print(package->getSource());
  Serial.print(F("\nSignal: "));
  Serial.print(signal);
#endif  

  //======================================================
  //Checking if isn't Data Packet
  if (package->getType() == DATA ) {

    //only Data packet here
    uint8_t action = verifyPacket(package);
    if (action == 255) Serial.println(F("\nFlow not found..."));
    else executeAction(action, package);
    return;
  }



  //========================================================
  //Checking if Controller_Discovery Packet
  if ( package->getDestination() == BROADCAST &&
       package->getType() == CONTROLLER_DISCOVERY and signal ) {
    adjust = 12000ul - millis();
    Serial.print(F("\nReceived discovery packet of: "));
    Serial.print(package->getSource());
    //save information about neighborhood
    saveNeighbor(package->getSource());
    //Checking if node already received a packet with level equal to or less than received now
    if (package->getLevel() < (level - 1)) {
      level = package->getLevel() + 1;
      nexthop = package->getSource();
      controller =  package->getFinalSource(); //( ((uint16_t)package->getUserData(1)) << 8) | package->getUserData(0);
      sink = controller;
      Serial.print(F("\nController: "));
      Serial.print(controller);
      package->setSource(csmaca->getMyAddress());
      package->setLevel(level);
      // TODO make a random delay here?
      csmaca->send(package, BROADCAST); //TODO duplicate package?


      //START_REPORT_PHASE start in 41s
      timerTable[START_REPORT_PHASE].timerID = START_REPORT_PHASE;
      timerTable[START_REPORT_PHASE].looping = 1;
      timerTable[START_REPORT_PHASE].executed = 0;
      timerTable[START_REPORT_PHASE].t0 = millis() + 41000ul - 12000ul + (csmaca->getMyAddress() * 200);
      timerTable[START_REPORT_PHASE].countdown = 0;
      timerTable[START_REPORT_PHASE].multiply = 1;
      timerTable[START_REPORT_PHASE].active = true;

      //VERIFY_REPORT_SEND in 45s
      timerTable[VERIFY_REPORT_SEND].timerID = VERIFY_REPORT_SEND;
      timerTable[VERIFY_REPORT_SEND].looping = 1;
      timerTable[VERIFY_REPORT_SEND].executed = 0;
      timerTable[VERIFY_REPORT_SEND].t0 = millis() + 53000ul - 12000ul + (csmaca->getMyAddress() * 200);
      timerTable[VERIFY_REPORT_SEND].countdown = 0;
      timerTable[VERIFY_REPORT_SEND].multiply = 1;
      timerTable[VERIFY_REPORT_SEND].active = true;

      //STOP_REPORT_PHASE stop in 120s
      timerTable[STOP_REPORT_PHASE].timerID = STOP_REPORT_PHASE;
      timerTable[STOP_REPORT_PHASE].looping = 1;
      timerTable[STOP_REPORT_PHASE].executed = 0;
      timerTable[STOP_REPORT_PHASE].t0 = millis() + 120000ul - 12000ul;
      timerTable[STOP_REPORT_PHASE].countdown = 0;
      timerTable[STOP_REPORT_PHASE].multiply = 1;
      timerTable[STOP_REPORT_PHASE].active = true;

      //START_PROGRAMMING_PHASE stop in 121s
      timerTable[START_PROGRAMMING_PHASE].timerID = START_PROGRAMMING_PHASE;
      timerTable[START_PROGRAMMING_PHASE].looping = 1;
      timerTable[START_PROGRAMMING_PHASE].executed = 0;
      timerTable[START_PROGRAMMING_PHASE].t0 = millis() + 121000ul - 12000ul;
      timerTable[START_PROGRAMMING_PHASE].countdown = 0;
      timerTable[START_PROGRAMMING_PHASE].multiply = 1;
      timerTable[START_PROGRAMMING_PHASE].active = true;

      //STOP_PROGRAMMING_PHASE stop in 180s 3min
      timerTable[STOP_PROGRAMMING_PHASE].timerID = STOP_PROGRAMMING_PHASE;
      timerTable[STOP_PROGRAMMING_PHASE].looping = 1;
      timerTable[STOP_PROGRAMMING_PHASE].executed = 0;
      timerTable[STOP_PROGRAMMING_PHASE].t0 = millis() + 180000ul - 12000ul;
      timerTable[STOP_PROGRAMMING_PHASE].countdown = 0;
      timerTable[STOP_PROGRAMMING_PHASE].multiply = 1;
      timerTable[STOP_PROGRAMMING_PHASE].active = true;


      //TX_CONTROL start in 181s
      timerTable[TX_CONTROL].timerID = TX_CONTROL;
      timerTable[TX_CONTROL].looping = 1;
      timerTable[TX_CONTROL].executed = 0;
      timerTable[TX_CONTROL].t0 = millis() + 181000ul - 12000ul;
      timerTable[TX_CONTROL].countdown = 0;
      timerTable[TX_CONTROL].multiply = 1;
      timerTable[TX_CONTROL].active = true;






    }
  }

  //=========================================================
  //Checking if REPORT_CONFIRMATION Packet
  if ( package->getDestination() == csmaca->getMyAddress() &&
       package->getType() == REPORT_CONFIRMATION ) {
    Serial.print(F("\nReceived Report Confirmation packet of: "));
    Serial.print(package->getSource());
    Serial.print(F(" for : "));
    Serial.print(package->getFinalDestination());
    //save information about neighborhood
    saveNeighbor(package->getSource());

    if (package->getFinalDestination() == csmaca->getMyAddress()) {
      reportConfirmed = true;
      package->setFinalSource(csmaca->getMyAddress()); //change PING
      package->setFinalDestination(controller);
      csmaca->send(package, nexthop);                  //change PING
    } else {
      if (package->getFinalDestination() == controller) { //change PING
        csmaca->send(package, nexthop); //change PING
      } else if ( getFarNodeNextHop(package->getFinalDestination()) != BROADCAST) {
        csmaca->send(package, getFarNodeNextHop(package->getFinalDestination()));
      }
    }



  }


  //========================================================
  //Checking if Report Packet to Controller
  //  if (  package->getFinalDestination() == controller &&
  //        package->getDestination() == csmaca->getMyAddress()  &&
  //        package->getType() ==  REPORT_NEIGHBORHOOD_PACKET) {
  //    Serial.print(F("\nReceived Report_Neighborhood packet of : "));
  //    Serial.print(package->getSource());
  //
  //    csmaca->send(package, nexthop);
  //    Serial.print(F("\nForward..."));
  //    saveFarNode(package->getFinalSource(), package->getSource());
  //
  //  }
  //========================================================


  //====================================================
  //Checking if Programming Confirmation Packet to Controller
  if ( package->getFinalDestination() == controller &&
       package->getDestination() == csmaca->getMyAddress()  &&
       package->getType() ==  PROGRAMMING_CONFIRMATION) {
    //resend to next hop
    csmaca->send(package, nexthop);
  }


  //====================================================
  //Checking if Programming Packet from Controller
  if (
    ( package->getType() == PROGRAMMING_FLOW ||
      package->getType() == PROGRAMMING_TIMER ||
      package->getType() == PROGRAMMING_ACTION ) &&
    package->getDestination() == csmaca->getMyAddress()
  ) {
    //It isn't a final destination?
    if (package->getFinalDestination() != csmaca->getMyAddress()) {
      //resend to next hop
      csmaca->send(package,  getFarNodeNextHop(package->getFinalDestination()));
    } else {
      Serial.println(F("Programming package Received..."));
      //package->print();
      //TO DO PROGRAMMING TABLES
      if (package->getType() == PROGRAMMING_FLOW) {
        for (int k = 0; k < 17; k++)
          paux[k] = package->getUserData(k);
        fea = fromSerialFlowEntry(paux);
        flowTable[fea.flowID] = fea;
        printFlowEntry(fea);
      }
      if (package->getType() == PROGRAMMING_ACTION) {
        for (int k = 0; k < 17; k++)
          paux[k] = package->getUserData(k);
        aea = fromSerialActionEntry(paux);
        actionTable[aea.actionID] = aea;
        printActionEntry(aea);
      }
      if (package->getType() == PROGRAMMING_TIMER) {
        for (int k = 1; k < 17; k++)
          paux[k] = package->getUserData(k);
        tea = fromSerialTimerEntry(paux);
        timerTable[tea.timerID] = tea;
        printTimerEntry(tea);
        //scheduleTimer(timerEntryPacket.te.timerID);
      }

      //resend to controler
      //package->setType(PROGRAMMING_CONFIRMATION);
      //package->setFinalSource(csmaca->getMyAddress());
      //package->setFinalDestination(controller);
      //csmaca->send(package, nexthop);
      totalProgrammingPackages++;

    }
  }





}



//=========================================================================


void setup(void)
{
  adjust = 0;


  // Aplication layer ====================================================
  //clearStatistics();
  appStatus = STARTING;
  level = 10000;
  //START_DISCOVERY_CONTROLLER_PHASE start in 9s
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].timerID = START_DISCOVERY_CONTROLLER_PHASE;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].looping = 1;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].executed = 0;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].t0 = millis() + 9000ul;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].countdown = 0;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].multiply = 1;
  timerTable[START_DISCOVERY_CONTROLLER_PHASE].active = true;

  //STOP_DISCOVERY_CONTROLLER_PHASE stop in 41s
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].timerID = STOP_DISCOVERY_CONTROLLER_PHASE;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].looping = 1;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].executed = 0;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].t0 = millis() + 41000ul;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].countdown = 0;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].multiply = 1;
  timerTable[STOP_DISCOVERY_CONTROLLER_PHASE].active = true;

  // Programation after discovery package



  clearNeighbor();


  //all sensor have a temperture sensor.
  sensorTable[0].sensorID = 0;
  sensorTable[0].sensorType = TEMPERATURE;
  sensorTable[0].sensorType = CELSIUS;
  sensorTable[0].dataType = INT_8;
  sensorTable[0].interfaceType = ANALOGIC_I;
  sensorTable[0].interfaceNumber1 = 10;
  sensorTable[0].active = true;
  for (int j = 1; j < SIZEOF_SENSOR_TABLE; j++) sensorTable[j].active = false;

  //==============================================================================



  //
  // Print preamble
  //

  repeat = micros();
  Serial.begin(115200);//57600);
  printf_begin();
  Serial.print(F("\n\r~ / home / llima / Dropbox / Doutorado / arduino / testes / ESDWSN / sensor / sensor.ino / \n\r"));


#if !defined(__arm__)
  // Read address from EEPROM
  EEPROM_read(0, configuration);
  Serial.print(F("Current settings in EEPROM\n"));
#endif
#if defined(__arm__)
  configuration.address = 0;
#endif
  Serial.print(F("Node address : "));
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

  //  printf("\nIsController : % d",csmaca->getEsdwdn()->isController());
  Serial.print(F("\nIsController : "));
  Serial.print(isController);
  Serial.flush();

  // Setting parameters: timetorandom,  numberofslots, mintime
  csmaca->setTimetorandom(TIMETORANDOM);
  csmaca->setNumberofslots(NUMBEROFSLOTS);
  csmaca->setMintime(MINTIME);
  pac = new Package();
  timeCount = millis();

  endSetup = true;
}

void loop(void)
{

  if (!endSetup) return;
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
