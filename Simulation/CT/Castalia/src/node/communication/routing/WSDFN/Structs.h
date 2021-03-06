


#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#define SIZEOF_FLOW_TABLE 30        /** 17 bytes *  30 =  510 bytes */
#define SIZEOF_ACTION_TABLE 20      /** 18 bytes *  20 =  360 bytes */
#define SIZEOF_SENSOR_TABLE 5       /** 10 bytes *   5 =   50 bytes */
#define SIZEOF_TIMER_TABLE 5        /** 15 bytes *   5 =   75 bytes */
#define SIZEOF_NEIGHBORS_TABLE 20   /**  2 bytes *  20 =   40 bytes */
									/** 		 total =  1015 bytes */




/*******************
 **  Who is SINK?
 *******************/

//uint16_t sink;


/**********************
 **    Flow Tables
 **********************/
 

/**
 * @brief Flow Tables Entry for address filter
 * 
 * 
 */
struct flowAddressEntry { 	
    uint16_t sourceAddress; /**<  */ 
    uint16_t sourceMask;      /**< Address's Mask. Need to make "and" with Address for to match. */ 
    uint16_t destinationAddress; /**<  */
    uint16_t destinationMask; /**<  */ 
};   /** 8 Bytes */


/**
 * @brief Values type in comparisons
 * 
 * 
 */
enum dataType_t {
    INT_8, /**<  */  
    INT_8_2, /**<  */ 
    INT_16, /**<  */ 
    INT_16_2 /**<  */ 
};

/**
 * @brief Operators type for comparisons
 * 
 * 
 */
enum operator_t{
	NONE,       /**< Unused comparisons */
	E,			/**< */
	D,			/**< */
    GT, 		/**<  */ 
    GTOE,		/**<  */ 
    LT, 		/**<  */ 
    LTOE, 		/**<  */ 
    GT_A_LT, 	/**<  */ 
    GTOE_A_LT, 	/**<  */ 
    GT_A_LTOE, 	/**<  */ 
    GTOE_A_LTOE /**<  */ 
};


/**
 * @brief to access values in many ways
 * 
 * 
 */
struct value_t{  
    /**
	 * @brief 
	 * 
	 * 
	 */
	union{
        /**
		 * @brief 
		 * 
		 * 
		 */
		struct {
        	uint8_t valueInt8_1;  /**<  */
            uint8_t valueInt8_2; /**<  */
        };
        /**
		 * @brief 
		 * 
		 * 
		 */
		struct { 
            uint16_t valueInt16_1;  /**<  */
            uint16_t valueInt16_2; /**<  */
        };
    };
}; /** 4 Bytes */


/**
 * @brief Flow Tables Entry for sensor measure values filter
 * 
 * 
 */
struct flowComparationEntry{
    uint8_t dataType;         /**< Data type to comparison */
    uint8_t operatorC;         /**< Operator (>, >=, <, <=, > e <, >= e <; >= e <=; > e <=;) */
    struct value_t value;            /**< Value for comparison */
}; /** 6 Bytes */


/**
 * @brief General Flow Table Entry 
 * 
 * 
 */
struct flowEntry{
	uint8_t flowID;			  /**< flow identification code */
    uint8_t actionID;		  /**< Action to play for this flow */ 
    unsigned int timeout;   /**<  When the flowEntry will be drop  */ 
    uint8_t counters;         /**< Count times the flow was identified */
	bool active;	      /**< Entry active or not */
	/**
	 * @brief For access flow entries in two ways.
	 * 
	 * 
	 */
	struct flowComparationEntry fc; /** 6 Bytes */
	struct flowAddressEntry fa;     /** 8 Bytes */

}; /** 17 Bytes */

//struct flowEntry flowTable[SIZEOF_FLOW_TABLE];



/************************
 * * Actions Table
 * *********************/
 
/**
 * @brief Actions
 * 
 * 
 */ 
enum action_t{
	  A_DROP,          /**> Discard the package */
	  A_RESEND_TO,     /**> Resend the package to the destination address */
	  A_SEND_TO_SINK,  /**> Send the package to the sink */
	  A_CHANGE_SINK,   /**> Change the sensors sink */
	  A_ACTIVE_TIMER,   /**> Activate a timer */
	  A_DEACTIVE_TIMER, /**> Deactivate a timer */
	  A_READ_SENSOR,    /**> Read a sensor and send value to sink */
	  A_READ_SENSOR_AND_SEND_TO, /**> Read a sensor and send value to destination address */
	  A_SLEEP,	      /**> Make the sensor sleep */
	  A_WAKEUP,         /**> Make the sensor wake up */
	  A_REPORT,	      /**> Send report mensage to controller */
	  A_RECEIVER_ON,   /**> Activate radio receiver */
	  A_RECEIVER_OFF,   /**> Deactivate radio receiver */
	  A_DISPLAY,		/**> Display package Received (for debug)*/
	  /** Action for agregations fuctions */
	  A_STORE,          /**> Store the value to flash memory */
	  A_RETRIEVE,	  	  /**> Retrieve the value from flash memory */
	  A_COMPARE,        /**> Compare two values and perform action */
	  A_AVERAGE,		  /**> Calculates the average of store values */
	  A_MAX,			  /**> Find the highest stored value */
	  A_MIN			  /**> Find the lowest stored value */	  
};
  
  /**
 * @brief Action Table Entry
 * 
 * 
 */
struct actionEntry{ 
	uint8_t actionID;    /**< action identification code */
	uint8_t actionType;  /**< Action's Type */
	bool active;	      /**< Entry active or not */	
	uint8_t dataType;     /**< Data Type */
	uint8_t operatorC;         /**< Operator (>, >=, <, <=, > e <, >= e <; >= e <=; > e <=;) */
	struct value_t value1;     /**< values depends of type of action */
	struct value_t value2;
	struct value_t value3;
}; /** 18 Bytes */

//struct actionEntry actionTable[SIZEOF_ACTION_TABLE];  


/************************
 **  Sensors Table
 ************************/
 
/**
 * @brief Sensor Type
 * 
 * 
 */
 enum sensorType_t{
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
 * @brief Unit of measurement
 * 
 * 
 */
 enum unit_t{
	CELSIUS,
	PERCENT,
	ONOFF,
	GPM, /** for flow rate */
	LUMENS,
	MVG,  /** for HALL sensors */
	VOLTS
 }; 
 
 /**
  * @brief Interface Type between Arduino and Sensor
  *    
  */
 enum interfaceType_t{
	DIGITAL,  /**> Digital Interface (number 1 is interface ID) */
	ANALOGIC, /**> Analogical Interface (number 1 is interface ID) */
	SPI,	  /**> SPI Interface (number 1 and 2 is interface ID) */
	I2C,      /**> I2C Interface (number 1 and 2 is interface ID) */
	SERIAL	  /**> Serial Interface (number 1 and 2 is interface ID) */
 }; 
 

/**
 * @brief Sensors Table Entry
 * 
 * 
 */
struct sensorEntry{ 
	uint8_t sensorID;    /**< Sensor identification code */
	uint8_t sensorType;  /**< Sensor's Type */
	uint8_t unit;        /**< Unit of Measurement */
	uint8_t dataType;     /**< Data Type of measurement */
	uint8_t interfaceType; /**< Interface Type */
	uint8_t interfaceNumber1; /**< Interface Number 1*/
	uint8_t interfaceNumber2; /**< Interface Number 1*/
	bool active;	/**< Entry active or not */
}; /** 10 Bytes */

//struct sensorEntry sensorTable[SIZEOF_SENSOR_TABLE];

/**
 * @brief Timers Table
 * 
 * 
 */ 
 
struct timerEntry{
	uint8_t timerID; /**< Timer identification code */
	uint8_t actionID;    /**< action to execute */
	double t0;      /**< time zero */
	double time;  /**< time to execute action */
	uint8_t repeat;  /**<  number of repetitions 255 indefinitely */
	uint8_t multiply; /**< multiplication factor of time */
	bool active;	      /**< Entry active or not */	
};

//struct timerEntry timerTable[SIZEOF_TIMER_TABLE];

/**
 * @brief Neighbors Table
 * 
 */
 
 struct neighborsEntry {
		uint16_t address;	 
 }; /** 2 Bytes */
 
// struct neighborsEntry neighborsTable[SIZEOF_NEIGHBORS_TABLE];

#endif				//_STRUCTS_H_

