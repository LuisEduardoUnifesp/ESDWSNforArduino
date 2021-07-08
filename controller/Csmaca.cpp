/**
   @file Csmaca.cpp

   Class implementation for Csmaca
*/


#include <SPI.h>

#include "nRF24L01.h"
#include "RF24.h"
#include "RF24_config.h"


#include "Csmaca.h"
//#include "Esdwdn.h"

//#define DEBUG

#include <stdint.h>


/***********************************************************************************/

Csmaca::Csmaca(RF24 *rad, uint16_t myAddress, bool npersistent) {
  nb = 0;
  this->npersistent = npersistent;
#ifdef DEBUG
  Serial.print(F("\nnpersistente:"));
  Serial.print(this->npersistent);
  Serial.print(F("\n"));

#endif
  this->myAddress = myAddress;
  txQueue = new PackageQueue();
  txQueue->clear();
  Serial.print(F("\nPackage Empty?"));
  Serial.println(txQueue->isEmpty());
  //esdwdn = new Esdwdn();

  radio = rad;
  radio->startListening();
  randomSeed(analogRead(0));
  status = RX_READY;

  // Open pipes to other nodes for communication
  radio->openWritingPipe(pipes[1]);   //write in pipe ..2
  radio->openReadingPipe(1, pipes[0]); //read  in pipe ..1

  numberOfTransmissionsFaults = 0;
  numberOfTransmissions = 0;
  totalNb = 0;
  totalDelayTransmission = 0;

#ifdef DEBUG
  radio->printDetails();
#endif
}




/***********************************************************************************/

void Csmaca::clearStatistics() {
  this->numberOfTransmissionsFaults = 0;
  this->numberOfTransmissions = 0;
  this->totalNb = 0;
  this->nb = 0;
  this->totalDelayTransmission = 0;
}



/***********************************************************************************/

void Csmaca::setMyAddress(uint16_t myAddress) {
  this->myAddress = myAddress;
}

/***********************************************************************************/

uint16_t Csmaca::getMyAddress() {
  return myAddress;
}

/***********************************************************************************/

//void Csmaca::setEsdwdn(Esdwdn *esd) {
//  esdwdn = esd;
//}

/***********************************************************************************/

//Esdwdn *Csmaca::getEsdwdn() {
//  return esdwdn;
//}

/***********************************************************************************/

int Csmaca::getMintime() {
  return mintime;
}

/***********************************************************************************/

int Csmaca::getTimetorandom() {
  return timetorandom;
}

/***********************************************************************************/

int Csmaca::getNumberofslots() {
  return numberofslots;
}


/***********************************************************************************/

void Csmaca::setTimetorandom(int timetorandom) {
  this->timetorandom = timetorandom;
}

/***********************************************************************************/

void Csmaca::setMintime(int mintime) {
  this->mintime = mintime;
}

/***********************************************************************************/

void Csmaca::setNumberofslots(int numberofslots) {
  this->numberofslots = numberofslots;
}

/***********************************************************************************/

void Csmaca::setRadio(RF24 *rad) {
  radio = rad;
}

/***********************************************************************************/

RF24 *Csmaca::getRadio() {
  return radio;
}

/***********************************************************************************/

int Csmaca::getNumberOfTransmissionsFaults() {
  return numberOfTransmissionsFaults;
}

/***********************************************************************************/

int Csmaca::getNumberOfTransmissions() {
  return numberOfTransmissions;
}

/***********************************************************************************/

int Csmaca::getTotalNb() {
  return totalNb;
}

unsigned long Csmaca::getTotalDelayTransmission() {
  return totalDelayTransmission;
}


/***********************************************************************************/

void Csmaca::printStatus() {
  Serial.print(F(">>Csma parameters: \n"));
  Serial.print(F("Time of one slot: "));
  Serial.print(timetorandom);
  Serial.print(F("\nMin time to wait for verify medium: "));
  Serial.print(mintime);
  Serial.print(F("\nNumber of slots for draw: "));
  Serial.print(numberofslots);
  //Serial.print(F(">>Upper layer statistics: \n"));
  //this->esdwdn->printStatus();
  Serial.print(F("\n>>Csma statistics: \n"));
  Serial.print(F("\nTotal Success Sent Packages:"));
  Serial.print(numberOfTransmissions);
  Serial.print(F("\nTotal Sent Packages Faults:"));
  Serial.print(numberOfTransmissionsFaults);
  Serial.print(F("\nAverage Transmission Success: (%) "));
  Serial.println(100.0 * (numberOfTransmissions / (numberOfTransmissions + numberOfTransmissionsFaults)));
  Serial.print(F("\nAverage Delay Transmission: (us) "));
  Serial.println( 1.0 * (totalDelayTransmission / numberOfTransmissions));
  Serial.print(F("\nTotal NB Transmission: "));
  Serial.print( totalNb );
  Serial.print(F("\nAverage NB Transmission: (%) "));
  Serial.println(100.0 * totalNb / numberOfTransmissions);

}

/***********************************************************************************/

typeStatus Csmaca::getStatus() {
  return status;
}

/***************************************************************************************/

bool Csmaca::isMediumBusy() {

  // delay according to manual radio
  //delayMicroseconds(258);
  bool aux = radio->testRPD(); //radio->testCarrier()

  // reset the value of isMediumBusy() for new read
  radio->stopListening();
  radio->startListening();
  // delay according to manual radio
  //delayMicroseconds(258);

  //return ();
  return (aux);
}

/***************************************************************************************/

bool Csmaca::isReceiving() {
  //radio.startListening();
  return (radio->available());
}

/***************************************************************************************/

void Csmaca::setNpersistent(bool npersistent) {
  this->npersistent = npersistent;
}

/***************************************************************************************/

bool Csmaca::isNpersistent() {
  return npersistent;
}

/***************************************************************************************/

bool Csmaca::send(Package *package, uint16_t destination) {
  package->setSource(myAddress);
  package->setDestination(destination);
  return txQueue->enQueue(package);
}

/***************************************************************************************/
/*
  // adapted from https://github.com/ForceTronics/nRF24L01_Wireless_Sensor_Dev_Board/blob/master/WSNode.cpp
  void Csmaca::getSettings()
  {
  int val;
  pinMode(10, INPUT_PULLUP); //set pin 8 as an input
  struct config_CSMA *configuration = new config_CSMA;
  EEPROM_read(0, configuration);
  //if user make PIN10 down, change node address
  if (!digitalRead(10)) {
    //Serial.begin(115200); //start serial communication, need to turn off before using sleep and WDT
    //the following code reads the current settings from EEPROM and puts them in local variables
    Serial.print("Current settings in EEPROM\n");
    Serial.print("Node address: ");
    Serial.print(configuration->address);

    Serial.print("\nChange node address? (Y/N): ");
    if (getAnswer()) {
      Serial.print("\nEnter Node address to store in EEPROM:");
      while (!Serial.available()) {}
      val = Serial.parseInt();
      configuration->address = val;
    }
    EEPROM_write(0, configuration);
  }

  myAddress = configuration->address;
  delete configuration;
  }


  //gets reponse from serial monitor to see if user wants to change particular setting
  bool Csmaca::getAnswer()
  {
  while (!Serial.available()) { }
  if (Serial.read() == 'Y') return true;
  else return false;
  }

*/
/***************************************************************************************/


void Csmaca::fallAsleep() {
  radio->powerDown();
  status = OFF;
}

bool Csmaca::isTxQueueEmpty() {
  return txQueue->isEmpty();
}


void Csmaca::wakeUp() {
  radio->powerUp();
  radio->startListening();
  status = RX_READY;
}

void Csmaca::loop() {
  Package *package_aux;

  unsigned long now = micros();
  int timeToWait = 1;

  //check receiving
  if (((status == RX_READY) ||(status == WAIT_TO_NEXT_SEND)) && (isReceiving())) {
    package_aux = new Package();
    radio->read( package_aux, sizeof(*package_aux) );
#ifdef DEBUG
    Serial.print(F("\nPackage received: "));
    package_aux->print();
    Serial.print(F("\nGot payload from: "));
    Serial.print(package_aux->getSource());
    Serial.print(F("\ntime send: "));
    Serial.println(package_aux->getTime());

#endif
      Serial.print(F("\nReceived: "));
      Serial.print(micros());
    //receive packages with source is the node, or packages of BROADCAST
    if ((package_aux->getDestination() == myAddress) || (package_aux->getDestination() == BROADCAST)) {
      //this->esdwdn->received(package_aux, radio->testRPD());
      received(package_aux, radio->testRPD());
    }

    delete package_aux;
    now = micros(); //adjust the clock for transmission verify
  }




  //With n-persistent csma disabled, simply try to send the package as soon as it is requested (total-aloha)
  if (!npersistent) {
    if (!txQueue->isEmpty()) {
      Serial.print(F("por aqui... "));
      Serial.print(!txQueue->isEmpty());
    }
    if ((status == RX_READY) && (!txQueue->isEmpty())) {
      package_aux = (txQueue->getFront());
      radio->stopListening();
      Serial.print(F("\nCSMA send...>"));
      Serial.print(micros());
      bool ok = radio->write(package_aux, sizeof(Package));
      if (ok) {
        status = RX_READY;
        //removes the packet from the queue in case of successful transmission
        txQueue->deQueue();
        radio->startListening();
        numberOfTransmissions++;
        totalDelayTransmission += (now - package_aux->getTime());
      } else {
        status = RX_READY;
        //removes the packet from the queue
        txQueue->deQueue();
        numberOfTransmissionsFaults++;
      }
    }
  }
  else {

    //Checks if there are packets to be transmitted in the queue, and transmits packets after a random time if the medium is free,
    //CSMA n-persistent
    if ((status == RX_READY) && (!txQueue->isEmpty())) {
      if (!isMediumBusy()) {
        status = TRANSMITTING;
        // chooses a random time to wait until it is checked again if the medium is still free
        timeToWait = MINTIME + random(0, (NUMBEROFSLOTS - 1) ) * TIMETORANDOM;

        timeToSend = now + timeToWait;
        // Restarts "requesToSend" if it is the first transmission attempt
        if (nb == 0) {
          requestToSend = now;
        }
#ifdef DEBUG
        Serial.print(F("\nnow: "));
        Serial.print(now);
        Serial.print(F("\ntimeToWait: "));
        Serial.print(timeToWait);
        Serial.print(F("\ntimeToSend: %lu"));
        Serial.print(timeToSend);
        Serial.print(F("\nstatus: %i"));
        Serial.print(status);
#endif

      }
    } // status transmitting: wait until the medium is free and timeToWait done


    /*
       after the timeToWait checks the medium again and, if it is free, transmits.
       Otherwise, choose new random timeToWait after medium is free
    */
    if (status == TRANSMITTING) {
#ifdef DEBUG
      Serial.print(F("\nTT> now: "));
      Serial.print(now);
#endif
    }
    
    if ((status == WAIT_TO_NEXT_SEND) && (now >= timeToSend)) {
      status = RX_READY;
    }
    
    /*
       If it is drawn to the first slot it sends immediately.
       If it goes to the following slots, it first checks to see if the media is free.
    */
    if ((status == TRANSMITTING) && (now >= timeToSend)) {
      //
      if ((timeToWait == 0) || !isMediumBusy()) {
        //retrieves the first package from the queue without removing it and transmits
        package_aux = (txQueue->getFront());
#ifdef DEBUG
        Serial.print(F("\nSending on the %i attempt"));
        Serial.print(nb);
        Serial.print(F("\nTT> now: %lu"));
        Serial.print(now);
        Serial.print(F("\nTT> delay: %lu"));
        Serial.print(now - package_aux->getTime());
#endif
        radio->stopListening();
        //int address=package_aux->getAddress();
        //package_aux->setDelaycsma(now - requestToSend);
        bool ok = radio->write(package_aux, sizeof(Package));
        Serial.print(F("\nCSMA send...<<"));
        Serial.print(micros());
        if (ok) {
          status = WAIT_TO_NEXT_SEND;
          //removes the packet from the queue in case of successful transmission
          txQueue->deQueue();
          //Save metadata for statistics calculation
          numberOfTransmissions++;
          totalNb += nb;
          totalDelayTransmission += (now - package_aux->getTime());
          radio->startListening();
          timeToSend = now + TIMETOWAIT;
          
          //Restart nb
          nb = 0;
        } else {
          status = WAIT_TO_NEXT_SEND;
          //removes the packet from the queue in case of transmission failure
          radio->startListening();
          timeToSend = now + TIMETOWAIT;
          txQueue->deQueue();
          numberOfTransmissionsFaults++;
          totalNb += nb;
          //Restart nb
          nb = 0;
        }
        //delete package_aux; //package send and destroyed.
      } else {
        /**
              In case another node has drawn a smaller number,
              wait for this to be transmitted and once it has finished,
              draw a number again to try to gain access to the medium.
        */
        status = RX_READY;
#ifdef DEBUG
        Serial.print(F("\nTT> Medium Busy in: "));
        Serial.print(now);
#endif
        nb++;
      }
    }
  }


  /**
     adapted from https://github.com/ForceTronics/nRF24L01_Wireless_Sensor_Dev_Board
  */

  //this->esdwdn->loop();


} //loop

/***************************************************************************************/
