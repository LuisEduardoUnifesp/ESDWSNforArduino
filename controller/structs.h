


#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#define SIZEOF_FLOW_TABLE 5        /** 17 bytes *  5 =  85 bytes */
#define SIZEOF_ACTION_TABLE 5      /** 15 bytes *  20 =  300 bytes */
#define SIZEOF_SENSOR_TABLE 2       /** 10 bytes *   5 =   50 bytes */
#define SIZEOF_TIMER_TABLE 16        /** 15 bytes *   13 =   195 bytes */
#define SIZEOF_PROGRAMMABLE_TIMER_TABLE 5
#define SIZEOF_NEIGHBORS_TABLE 10   /**  2 bytes *  6 =   40 bytes */
#define SIZEOF_FARNODE_TABLE 100    /** 4 bytes * 10 = 40 bytes */

/**      total =  1015 bytes */

#define NUMBER_NODES 5




/*******************
 **  Who is SINK?
 *******************/

//int sink;


/**********************
 **    Flow Tables
 **********************/


/**
   @brief Flow Tables Entry for address filter


*/
struct flowAddressEntry {
  uint16_t sourceAddress; /**<  */
  uint16_t sourceMask;      /**< Address's Mask. Need to make "and" with Address for to match. */
  uint16_t destinationAddress; /**<  */
  uint16_t destinationMask; /**<  */
};   /** 8 Bytes */


/**
   @brief Values type in comparisons


*/
enum dataType_t {
  INT_8, /**<  */
  INT_8_2, /**<  */
  INT_16, /**<  */
  INT_16_2 /**<  */
};

/**
   @brief Operators type for comparisons


*/
enum operator_t {
  NONE,       /**< Unused comparisons */
  E,      /**< */
  D,      /**< */
  GT,     /**<  */
  GTOE,   /**<  */
  LT,     /**<  */
  LTOE,     /**<  */
  GT_A_LT,  /**<  */
  GTOE_A_LT,  /**<  */
  GT_A_LTOE,  /**<  */
  GTOE_A_LTOE /**<  */
};


/**
   @brief to access values in many ways


*/
struct value_t {
  /**
    @brief


  */
  union {
        /**
      @brief


    */
    struct {
      uint16_t valueInt16_1;  /**<  */
      uint16_t valueInt16_2; /**<  */
    };
    /**
      @brief


    */
    struct {
      uint8_t valueInt8_1;  /**<  */
      uint8_t valueInt8_2; /**<  */
    };

  };
}; /** 4 Bytes */


/**
   @brief Flow Tables Entry for sensor measure values filter


*/
struct flowComparationEntry{
    uint8_t value1;           /**< Value for comparison */
    uint8_t value2;           /**< Value for comparison */
//    uint8_t dataType;         /**< Data type to comparison */
    uint8_t operatorC;         /**< Operator (>, >=, <, <=, > e <, >= e <; >= e <=; > e <=;) */
    //struct value_t value;            /**< Value for comparison */
}; /** 6 Bytes */


/**
   @brief General Flow Table Entry

 
*/
struct flowEntry {
  struct flowAddressEntry fa;     /** 8 Bytes */
  struct flowComparationEntry fc; /** 6 Bytes */
  
  uint8_t timeout;   /**<  When the flowEntry will be drop  */
  uint8_t flowID;       /**< flow identification code */
  uint8_t actionID;     /**< Action to play for this flow */
  uint8_t counters;         /**< Count times the flow was identified */
  bool active;        /**< Entry active or not */
  /**
     @brief For access flow entries in two ways.


  */

  

}; /** 17 Bytes */

void toSerialFlowEntry(struct flowEntry fe, uint8_t aux[]){
  aux[1]= (uint8_t)(fe.fa.sourceAddress & 0x00ff);  
  aux[0]= (uint8_t)(fe.fa.sourceAddress >> 8);  
  aux[3]= (uint8_t)(fe.fa.sourceMask & 0x00ff);
  aux[2]= (uint8_t)(fe.fa.sourceMask >> 8);
  aux[5]= (uint8_t)(fe.fa.destinationAddress & 0x00ff);
  aux[4]= (uint8_t)(fe.fa.destinationAddress >> 8);
  aux[7]= (uint8_t)(fe.fa.destinationMask & 0x00ff);
  aux[6]= (uint8_t)(fe.fa.destinationMask >> 8); 
  aux[8]=fe.fc.value1;
  aux[9]=fe.fc.value2;
  aux[10]=fe.fc.operatorC;
  aux[11]=fe.timeout;
  aux[12]=fe.flowID;
  aux[13]=fe.actionID;
  aux[14]=fe.counters;
  aux[15]=fe.active;
}

struct flowEntry fromSerialFlowEntry(uint8_t aux[]){
  struct flowEntry fe;
  fe.fa.sourceAddress = (((uint16_t)aux[0]) << 8) | aux[1];
  fe.fa.sourceMask =(((uint16_t)aux[2]) << 8) | aux[3];
  fe.fa.destinationAddress =(((uint16_t)aux[4]) << 8) | aux[5];
  fe.fa.destinationMask =(((uint16_t)aux[6]) << 8) | aux[7];  
  fe.fc.value1=aux[8];
  fe.fc.value2=aux[9];
  fe.fc.operatorC=aux[10];
  fe.timeout=aux[11];
  fe.flowID=aux[12];
  fe.actionID=aux[13];
  fe.counters=aux[14];
  fe.active=aux[15];
  return fe;
}

void printFlowEntry(struct flowEntry fe){ 
  Serial.print(F("\nFlowEntry:\n"));  
  Serial.print(F("\nSource Address:\n"));  
  Serial.print(fe.fa.sourceAddress);  
  Serial.print(F("\nSource Mask:\n"));  
  Serial.print(fe.fa.sourceMask);  
  Serial.print(F("\nDestination Address:\n"));  
  Serial.print(fe.fa.destinationAddress);  
  Serial.print(F("\nDestination Mask:\n"));  
  Serial.print(fe.fa.destinationMask);  
  Serial.print(F("\nValue1:\n"));  
  Serial.print(fe.fc.value1);  
  Serial.print(F("\nValue2:\n"));  
  Serial.print(fe.fc.value2);  
  Serial.print(F("\nOperatorC:\n"));  
  Serial.print(fe.fc.operatorC);  
  Serial.print(F("\nTimeout:\n"));  
  Serial.print(fe.timeout);  
  Serial.print(F("\nFlow ID:\n"));  
  Serial.print(fe.flowID);  
  Serial.print(F("\nAction ID:\n"));  
  Serial.print(fe.actionID);  
  Serial.print(F("\nCounters:\n"));  
  Serial.print(fe.counters);  
  Serial.print(F("\nActive:\n"));  
  Serial.println(fe.active);  
  uint8_t aux[17];
  aux[1]= (uint8_t)(fe.fa.sourceAddress & 0x00ff);  
  aux[0]= (uint8_t)(fe.fa.sourceAddress >> 8);  
  aux[3]= (uint8_t)(fe.fa.sourceMask & 0x00ff);
  aux[2]= (uint8_t)(fe.fa.sourceMask >> 8);
  aux[5]= (uint8_t)(fe.fa.destinationAddress & 0x00ff);
  aux[4]= (uint8_t)(fe.fa.destinationAddress >> 8);
  aux[7]= (uint8_t)(fe.fa.destinationMask & 0x00ff);
  aux[6]= (uint8_t)(fe.fa.destinationMask >> 8); 
  aux[8]=fe.fc.value1;
  aux[9]=fe.fc.value2;
  aux[10]=fe.fc.operatorC;
  aux[11]=fe.timeout;
  aux[12]=fe.flowID;
  aux[13]=fe.actionID;
  aux[14]=fe.counters;
  aux[15]=fe.active;
  for (int k = 0; k < 16; k++){
        Serial.print(aux[k]);
        Serial.print(F("::"));
  }
  
  
}




/************************
 * * Actions Table
 * *********************/

/**
   @brief Actions


*/
enum action_t {
  A_DROP,          /**> Discard the package */
  A_RESEND_TO,     /**> Resend the package to the destination address */
  A_SEND_TO_SINK,  /**> Send the package to the sink */
  A_RETURN_TO_ORIGIN, /**> Return the package to the origim */
  A_CHANGE_SINK,   /**> Change the sensors sink */
  A_ACTIVE_TIMER,   /**> Activate a timer */
  A_DEACTIVE_TIMER, /**> Deactivate a timer */
  A_READ_SENSOR,    /**> Read a sensor and send value to sink */
  A_READ_SENSOR_AND_SEND_TO, /**> Read a sensor and send value to destination address */
  A_SLEEP,        /**> Make the sensor sleep */
  A_WAKEUP,         /**> Make the sensor wake up */
  A_REPORT,       /**> Send report mensage to controller */
  A_RECEIVER_ON,   /**> Activate radio receiver */
  A_RECEIVER_OFF,   /**> Deactivate radio receiver */
  A_DISPLAY,    /**> Display package Received (for debug)*/
  /** Action for agregations fuctions */
  A_STORE,          /**> Store the value to flash memory */
  A_RETRIEVE,       /**> Retrieve the value from flash memory */
  A_COMPARE,        /**> Compare two values and perform action */
  A_AVERAGE,      /**> Calculates the average of store values */
  A_MAX,        /**> Find the highest stored value */
  A_MIN       /**> Find the lowest stored value */
};

/**
  @brief Action Table Entry


*/
struct actionEntry {
//  struct value_t value1;     /**< values depends of type of action */
//  struct value_t value2;
  uint16_t value1;
  uint16_t value2;
  uint8_t actionID;    /**< action identification code */
  uint8_t actionType;  /**< Action's Type */
  uint8_t dataType;     /**< Data Type */
  uint8_t operatorC;         /**< Operator (>, >=, <, <=, > e <, >= e <; >= e <=; > e <=;) */
  bool active;        /**< Entry active or not */
  uint8_t aux;
    
}; /** 15 Bytes */

void toSerialActionEntry(struct actionEntry ae, uint8_t aux[]){
  aux[1]= (uint8_t)(ae.value1 & 0x00ff);  
  aux[0]= (uint8_t)(ae.value1 >> 8);  
  aux[3]= (uint8_t)(ae.value2 & 0x00ff);
  aux[2]= (uint8_t)(ae.value2 >> 8);
  aux[4]=ae.actionID;
  aux[5]=ae.actionType;
  aux[6]=ae.dataType;
  aux[7]=ae.operatorC;
  aux[8]=ae.active;
}

struct actionEntry fromSerialActionEntry(uint8_t aux[]){
  struct actionEntry ae;
  
  ae.value1 = (((uint16_t)aux[0]) << 8) | aux[1];
  ae.value2 =(((uint16_t)aux[2]) << 8) | aux[3];
  ae.actionID=aux[4];
  ae.actionType=aux[5];
  ae.dataType=aux[6];
  ae.operatorC=aux[7];
  ae.active=aux[8];
  
  return ae;
}

void printActionEntry(struct actionEntry ae){  
  Serial.print(F("\nActionEntry:\n"));  
  Serial.print(F("\nValue 1:\n"));  
  Serial.print(ae.value1);  
  Serial.print(F("\nValue 2:\n"));  
  Serial.print(ae.value2);
  Serial.print(F("\nAction Id:\n"));  
  Serial.print(ae.actionID);    
  Serial.print(F("\nAction Type:\n"));  
  Serial.print(ae.actionType);    
  Serial.print(F("\nOperatorC:\n"));  
  Serial.print(ae.operatorC);    
  Serial.print(F("\nActive:\n"));  
  Serial.println(ae.active);    
  uint8_t aux[17];
  toSerialActionEntry(ae, aux);
  for (int k = 0; k < 16; k++){
        Serial.print(aux[k]);
        Serial.print(F("::"));
  }
  
  
}



/************************
 **  Sensors Table
 ************************/

/**
   @brief Sensor Type


*/
enum sensorType_t {
  TEMPERATURE,
  HUMIDITY,
  PRESENCE,
  FLOW,
  LUMINANCE,
  HALL,
  FLAME,
  BATTERY
};

/**
   @brief Unit of measurement


*/
enum unit_t {
  CELSIUS,
  PERCENT,
  ONOFF,
  GPM, /** for flow rate */
  LUMENS,
  MVG,  /** for HALL sensors */
  VOLTS
};

/**
   @brief Interface Type between Arduino and Sensor

*/
enum interfaceType_t {
  DIGITAL_I,  /**> Digital Interface (number 1 is interface ID) */
  ANALOGIC_I, /**> Analogical Interface (number 1 is interface ID) */
  SPI_I,    /**> SPI Interface (number 1 and 2 is interface ID) */
  I2C_I,      /**> I2C Interface (number 1 and 2 is interface ID) */
  SERIAL_I    /**> Serial Interface (number 1 and 2 is interface ID) */
};


/**
   @brief Sensors Table Entry


*/
struct sensorEntry {
  uint8_t sensorID;    /**< Sensor identification code */
  uint8_t sensorType;  /**< Sensor's Type */
  uint8_t unit;        /**< Unit of Measurement */
  uint8_t dataType;     /**< Data Type of measurement */
  uint8_t interfaceType; /**< Interface Type */
  uint8_t interfaceNumber1; /**< Interface Number 1*/
  uint8_t interfaceNumber2; /**< Interface Number 1*/
  bool active;  /**< Entry active or not */
}; /** 10 Bytes */

//struct sensorEntry sensorTable[SIZEOF_SENSOR_TABLE];

/**
   @brief Timers Table


*/

struct timerEntry {
  uint32_t t0;      /**< time zero */
  uint32_t countdown;  /**< time to execute action */  
  uint8_t timerID; /**< Timer identification code */
  uint8_t actionID;    /**< action to execute */
  uint8_t looping;  /**<  number of repetitions 255 indefinitely */
  uint8_t executed; /**< number executions performed */
  uint8_t multiply; /**< multiplication factor of time */
  bool active;        /**< Entry active or not */
  uint16_t aux; 
};

void toSerialTimerEntry(struct timerEntry te, uint8_t aux[]) {
  aux[3] = (uint8_t)(te.t0 & 0x000000ff);
  aux[2] = (uint8_t)((te.t0 >> 8) & 0x000000ff);
  aux[1] = (uint8_t)((te.t0 >> 16) & 0x000000ff);
  aux[0] = (uint8_t)((te.t0 >> 24) & 0x000000ff);
  aux[7] = (uint8_t)(te.countdown & 0x000000ff);
  aux[6] = (uint8_t)((te.countdown >> 8) & 0x000000ff);
  aux[5] = (uint8_t)((te.countdown >> 16) & 0x000000ff);
  aux[4] = (uint8_t)((te.countdown >> 24) & 0x000000ff);
  aux[8] = te.timerID;
  aux[9] = te.actionID;
  aux[10] = te.looping;
  aux[11] = te.executed;
  aux[12] = te.multiply;
  aux[13] = te.active;
}

struct timerEntry fromSerialTimerEntry(uint8_t aux[]) {
  struct timerEntry te;

  te.timerID = aux[8];
  te.actionID = aux[9];
  te.looping = aux[10];
  te.executed = aux[11];
  te.multiply = aux[12];
  te.active = aux[13];
  te.t0 = (((uint32_t)aux[0]) << 24) | (((uint32_t)aux[1]) << 16) | (((uint32_t)aux[2]) << 8) | aux[3];
  te.countdown = (((uint32_t)aux[4]) << 24) | (((uint32_t)aux[5]) << 16) | (((uint32_t)aux[6]) << 8) | aux[7];
  
  return te;
}

void printTimerEntry(struct timerEntry te) {
  Serial.print(F("\nTimerEntry:\n"));
  Serial.print(F("\nT0:\n"));
  Serial.print(te.t0);
  Serial.print(F("\nCountdown:\n"));
  Serial.print(te.countdown);
  Serial.print(F("\nTimer ID:\n"));
  Serial.print(te.timerID);
  Serial.print(F("\nAction ID:\n"));
  Serial.print(te.actionID);
  Serial.print(F("\nLooping:\n"));
  Serial.print(te.looping);
  Serial.print(F("\nExecuted:\n"));
  Serial.print(te.executed);
  Serial.print(F("\nMultiply:\n"));
  Serial.print(te.multiply);  
  Serial.print(F("\nActive:\n"));
  Serial.println(te.active);
  uint8_t aux[17];
  toSerialTimerEntry(te, aux);
  for (int k = 0; k < 16; k++) {
    Serial.print(aux[k]);
    Serial.print(F("::"));
  }
}


/**
   @brief Neighbors Table

*/

struct neighborsEntry {
  uint16_t address;
}; /** 2 Bytes */

struct farNodeNextHopEntry {
  uint16_t farNode;
  uint16_t nextHop;
};

struct neighborsEntry neighborsTable[SIZEOF_NEIGHBORS_TABLE];
struct farNodeNextHopEntry farNodeTable[SIZEOF_FARNODE_TABLE];


struct timerEntry timerTable[SIZEOF_TIMER_TABLE];


//struct timerEntry timerTableC[SIZEOF_TIMER_TABLE][NUMBER_NODES];
struct timerEntry timerTableC[5][NUMBER_NODES];
struct sensorEntry sensorTableC[SIZEOF_SENSOR_TABLE][NUMBER_NODES];
struct actionEntry actionTableC[SIZEOF_ACTION_TABLE][NUMBER_NODES];
struct flowEntry flowTableC[SIZEOF_FLOW_TABLE][NUMBER_NODES];


// verify size of memory

// verify size of memory

struct sensorEntry sensorTable[SIZEOF_SENSOR_TABLE];
struct actionEntry actionTable[SIZEOF_ACTION_TABLE];
struct flowEntry flowTable[SIZEOF_FLOW_TABLE];






#endif        //_STRUCTS_H_
