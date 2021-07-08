/**
   @file Csmaca.h

   Class declaration for Csmaca e helper enums
*/



#ifndef Csmaca_h
#define Csmaca_h
#include <stdint.h>

//#include "Csmaca_fwd.h"
//#include "Esdwdn_fwd.h"

#include "PackageQueue.h"



#define BROADCAST 0xFFFF
#define TIMETORANDOM 1000 //Slot time for the CSMA draw
#define NUMBEROFSLOTS 3 // Number of Slots for CSMA draw
#define MINTIME 300 //Minimum time for CSMA draw
#define TIMETOWAIT 100000 // Minimun Time between transmissions


void received(Package *package, bool signal);

/**
   Struct for save configuration in EEPROM
*/
struct config_CSMA {
  uint16_t address;
};



/**
   Link Level Status

   For use with Csmaca class
*/
enum typeStatus { RECEIVING, RX_READY, TRANSMITTING, BACKOFF, OFF, WAIT_TO_NEXT_SEND };



/**
   Radio pipe addresses for the nodes to communicate.
*/
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


/**
   Link Level Interface for the nRF24L01+ Radios
   CSMA implementation for media access control

*/
class Csmaca {
  private:
    
    typeStatus status; /** Status of Radio */
    bool npersistent; /** n_persistente active or not */
    int nb, be;
    RF24 *radio; /** Radios from library RF24 */
    //Esdwdn *esdwdn; /** Interface to upper level */
    PackageQueue *txQueue; /** Transmission Queue **/
    uint32_t timeToSend; /** Internal for CSMA */
    uint32_t requestToSend; /** Internal for CSMA */
    // statistical numbers
    int numberOfTransmissionsFaults; /** Number of Transmissions Faults */
    int numberOfTransmissions; /** Number of Transmissions */
    int totalNb; /** Total of Nb **/
    uint32_t totalDelayTransmission; /** total delay Transmission */
    int mintime, timetorandom, numberofslots; /** Paramaters for CSMA */

    uint16_t myAddress; /** Node Address */
  public:



    /**
       Constructor

       Create a new instance of the link level.

       @param rad The Radio object defined for library RF24. Before using, start all radio parameters
       @param myAddress The node address
    */
    Csmaca(RF24 *rad, uint16_t myAddress, bool npersistent);


    /**
       GetTxQueue status
    */
    bool isTxQueueEmpty();

    /**
       GetSettings from eeprom, or user when PIN8 is DOWN
    */
    void getSettings();

    /**
        Clear statistics for a new test.
    */
    void clearStatistics();

    /**
       Gets reponse from serial monitor to see if user wants to change particular setting

       @return True if user wants to change
    */
    bool getAnswer();


    /**
        Set Address Node

        @param myAddress The address node
    */
    void setMyAddress(uint16_t myAddress);

    /**
        Get Address Node

        @return The currently node address
    */
    uint16_t getMyAddress();

    /**
        Get Number of Transmissions Faults

        @return The Number of Transmissions Faults
    */
    int getNumberOfTransmissionsFaults();


    /**
            Get Number of Transmissions

            @return The Number of Transmissions
    */
    int getNumberOfTransmissions();


    /**
            Get Parameter of CSMA Mintime

            @return The Parameter of CSMA Mintime
    */
    int getMintime();

    
    /**
            Get Parameter of CSMA Timetorandom

            @return The Parameter of CSMA Timetorandom
    */
    int getTimetorandom();

     /**
            Get Parameter of CSMA Numberofslots

            @return The Parameter of CSMA Numberofslots
    */
    int getNumberofslots();


    
    /**
            Set Parameter of CSMA Mintime

            @param mintime The Parameter of CSMA Mintime
    */
    void setMintime(int mintime);


    /**
            Set Parameter of CSMA Timetorandom

            @param timetorandom The Parameter of CSMA Timetorandom
    */
    void setTimetorandom(int timetorandom);



    /**
            Set Parameter of CSMA Numberofslots

            @param numberofslots The Parameter of CSMA Numberofslots
    */
    void setNumberofslots(int numberofslots);



   


    


    /**
            Get Total of nb

            @return The Total of nb
    */
    int getTotalNb();

    /**
        Get Total Delay Transmission

        @return The TotalDelay Transmission
    */
    uint32_t getTotalDelayTransmission();




    /**
       Set The Network Upper Level

       @param esd The Network Upper Level
    */
    //void setEsdwdn(Esdwdn *esd);


    /**
       Get The Network Upper Level

       @return The Network Upper Level
    */
    //Esdwdn *getEsdwdn() ;


    /**
       Print the statistics of CSMACA
    */
    void printStatus();


    /**
       Set The Radio object from RF24 Libray

       @param rad The pointer to radio object
    */
    void setRadio(RF24 *rad);


    /**
       Get The Radio object from RF24 Libray

       @return The current radio object
    */
    RF24 *getRadio();


    /**
       Get The status of link level

       @return The current status of link level. The level change automaticly with sended and received packages
    */
    typeStatus getStatus();


    /**
       Set CSMA n-persistent active

       @param npersistent True for Enable n-persistent CSMA
    */
    void setNpersistent(bool npersistent);

    /**
       CSMA n-persistent active

      @return True if n-persistent CSMA is active, otherwise, false
    */
    bool isNpersistent();


    /**
       Checks if the physical medium is busy with transmissions from another node.

       @code
       if(csma.isMediumBusy()){
          ...
       }
       @endcode

       @return True if medium is busy, otherwise return True.
    */
    bool isMediumBusy();

    /**
       Checks if radio received any packet

      @return True if a packet was received in the buffer, false otherwise
    */
    bool isReceiving();


    /**
       Radio to sleep      
    */
    void fallAsleep();

    /**
       Radio power On
    */
    void wakeUp();





    /**
       Put one package in transmission queue for transmission

       @param package The Package for transmission
       @param destination The node destination for transmission. Use address BROADCAST for broadcast transmissions.
       @return False if the transmission queue is full and the package could not be stored for transmission. True if package was stored and ready for transmission
    */
    bool send(Package *package, uint16_t destination);

    /**
       Loop for Csma library

       Function must be executed in the loop function of the main program to ensure library operation.

       @warning This function is MANDATORY for library operation

       @code

       RF24 radio(7, 8);
       Csmaca csmaca(&radio, myAddress);

       void setup(void){
        radio.begin();
        radio... // another configurations
        radio.listening();
       }

       void loop(void){
        ... // another operations
        csmaca.loop(); // <<==MANDATORY for library operate
       }
       @endcode

    */
    void loop();

}; //class

/**
   @mainpage ESD-WSN (Extended Software Defined Wireless Sensor Network).

   @section Paper Subject
   The usage of the Software Defined Sensor Networks (SD-WSNs) as support for applications of Internet of Things (IoT) has been the focus of recent research to reduce the complexity of configuration and management. This simplification allows greater flexibility for the configuration of these networks in operating time. However, although more flexible than traditional approaches, the existing SD-WSN architectures found in the literature have limitations mainly with regard to to the reprogramming of devices in operating time. In this sense, in this article, we propose a new architecture in which the concept of the SDNs is extended to the application layer allowing the reprogramming of devices in operation time without the need of firmware update. The proposed architecture was validated in a simulation environment and the results showed in a preliminary way the effectiveness, flexibility and efficiency of the proposal.

   <br>
   @section Useful Useful References


   @li <a href="http://tmrh20.github.io/RF24/classRF24.html"><b>RF24</b> Class Documentation</a>
   @li <a href="https://github.com/TMRh20/RF24/archive/master.zip"><b>Download</b></a>
   @li <a href="https://github.com/tmrh20/RF24/"><b>Source Code</b></a>
   @li <a href="http://tmrh20.blogspot.com/2014/03/high-speed-data-transfers-and-wireless.html"><b>My Blog:</b> RF24 Optimization Overview</a>
   @li <a href="http://tmrh20.blogspot.com/2016/08/raspberry-pilinux-with-nrf24l01.html"><b>My Blog:</b> RPi/Linux w/RF24Gateway</a>
   @li <a href="http://www.nordicsemi.com/files/Product/data_sheet/nRF24L01_Product_Specification_v2_0.pdf">Chip Datasheet</a>

 * **Additional Information and Add-ons**

   @li <a href="http://tmrh20.github.io/RF24Network"> <b>RF24Network:</b> OSI Network Layer for multi-device communication. Create a home sensor network.</a>
   @li <a href="http://tmrh20.github.io/RF24Mesh"> <b>RF24Mesh:</b> Dynamic Mesh Layer for RF24Network</a>
   @li <a href="http://tmrh20.github.io/RF24Ethernet"> <b>RF24Ethernet:</b> TCP/IP Radio Mesh Networking (shares Arduino Ethernet API)</a>
   @li <a href="http://tmrh20.github.io/RF24Audio"> <b>RF24Audio:</b> Realtime Wireless Audio streaming</a>
   @li <a href="http://tmrh20.github.io/">All TMRh20 Documentation Main Page</a>
*/

#endif
