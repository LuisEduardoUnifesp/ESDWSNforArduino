/**
   @file Package.h

   Class declaration for Packages
*/



#ifndef Package_h
#define Package_h



enum EsdwdnPackTypeDef {
  DATA = 0,
  CONTROLLER_DISCOVERY = 1,
  RECOVERY_ROUTER = 2,
  REPORT = 3,
  PROGRAMMING = 4,
  ACTION_TABLE_PACKET = 5,
  FLOW_TABLE_PACKET = 6,
  TIMER_TABLE_PACKET = 7,
  REPORT_NEIGHBORHOOD_PACKET = 8,
  SENSOR_TABLE_PACKET =  9,
  PROGRAMMING_CONFIRMATION = 10,
  REPORT_CONFIRMATION = 11,
  REPORT_NEIGHBORHOOD_CONFIRMATION = 12,
  PROGRAMMING_FLOW =13,
  PROGRAMMING_TIMER = 14,
  PROGRAMMING_ACTION = 15
};

#define SIZEUSERDATA 17

class Package {
  private:
    uint16_t source; /**  */
    uint16_t destination;
    uint16_t finalSource;
    uint16_t finalDestination;
    uint32_t time;
    uint8_t type;
    uint8_t scenario;

    //uint32_t delaycsma;
    uint8_t level;    //15 bytes
    uint8_t userData[SIZEUSERDATA]; //32-15= 17





  public:
    /***********************************************************************************/
    Package() {
    }


    /***********************************************************************************/
    void setLevel(uint16_t level) {
      this->level = level;
    }



    /***********************************************************************************/
    uint16_t getLevel() {
      return level;
    }

    /***********************************************************************************/
    void setSource(uint16_t source) {
      this->source = source;
    }

    /***********************************************************************************/
    void setDestination(uint16_t destination) {
      this->destination = destination;
    }

    /***********************************************************************************/
    void setFinalSource(uint16_t finalSource) {
      this->finalSource = finalSource;
    }

    /***********************************************************************************/
    void setFinalDestination(uint16_t finalDestination) {
      this->finalDestination = finalDestination;
    }
    

    /***********************************************************************************/
    void setType(EsdwdnPackTypeDef type) {
      this->type = type;
    }


    /***********************************************************************************/
    void setScenario(uint16_t scenario) {
      this->scenario = scenario;
    }

    /***********************************************************************************/
    void setTime(uint32_t  ttime) {
      time = ttime;
    }

    /***********************************************************************************/
    uint32_t  getTime() {
      return time;
    }

    /***********************************************************************************/
    //void setDelaycsma(uint32_t  ttime) {
    //  delaycsma = ttime;
    // }

    /***********************************************************************************/
    //uint32_t  getDelaycsma() {
    //  return delaycsma;
    //}

    /***********************************************************************************/
    uint16_t getSource() {
      return source;
    }

    /***********************************************************************************/
    uint16_t getDestination() {
      return destination;
    }

    /***********************************************************************************/
    uint16_t getFinalSource() {
      return finalSource;
    }

    /***********************************************************************************/
    uint16_t getFinalDestination() {
      return finalDestination;
    }    

    /***********************************************************************************/
    uint16_t getType() {
      return type;
    }


    /***********************************************************************************/
    uint16_t getScenario() {
      return scenario;
    }

    /***********************************************************************************/
    void copy(Package *aux) {
      this->source = aux->source;
      this->destination = aux->destination;
      this->time = aux->time;
      this->type = aux->type;
    }

    /***********************************************************************************/
    void setUserData(uint8_t userData, int i) {
      if (i >= SIZEUSERDATA || i < 0) return;
      this->userData[i] = userData;
    }

    /***********************************************************************************/
    uint8_t getUserData(int i) {
      if (i >= SIZEUSERDATA || i < 0) return (0);
      else return (this->userData[i]);
    }

    /***********************************************************************************/
    uint16_t getController() {
      return ( (userData[0] << 8) && userData[1]);
    }

    /***********************************************************************************/
    void setController(uint16_t controller) {
      uint16_t aux = controller && 0xFF00;
      this->userData[1] = controller && 0x00ff;
      this->userData[0] = aux >> 8;
    }


    /***********************************************************************************/
    void print() {
      Serial.print(F("\nSize of: "));
      Serial.print(sizeof(Package));
      Serial.print(F("\nSource:"));
      Serial.print(source);
      Serial.print(F("\nDestination:"));
      Serial.print(destination);
      Serial.print(F("\nType:"));
      Serial.print(type);
      Serial.print(F("\nUserData:"));
      Serial.print(userData[0]);
      Serial.print(F(", "));
      Serial.print(userData[1]);
      Serial.print(F(", "));
      Serial.print(userData[2]);
      Serial.print(F(", "));
      Serial.print(userData[3]);
      Serial.print(F(", "));
      Serial.print(userData[4]);
      Serial.print(F(", "));
      Serial.print(userData[5]);
      Serial.print(F(", "));
      Serial.println(userData[6]);
      Serial.print(userData[7]);
      Serial.print(F(", "));
      Serial.print(userData[8]);
      Serial.print(F(", "));
      Serial.print(userData[9]);
      Serial.print(F(", "));
      Serial.print(userData[10]);
      Serial.print(F(", "));
      Serial.print(userData[11]);
      Serial.print(F(", "));
      Serial.print(userData[12]);
      Serial.print(F(", "));
      Serial.print(userData[13]);
      Serial.print(F(", "));
      Serial.print(userData[14]);
      Serial.print(F(", "));
      Serial.print(userData[15]);
      Serial.print(F(", "));
      Serial.print(userData[16]);


      
      Serial.print(F("\nUserData (int) 0: "));
      Serial.print( ( ( (uint16_t) userData[0] ) << 8) | userData[1]);
      Serial.print(F("\nUserData (int) 2: "));
      Serial.print( ( ( (uint16_t) userData[2] ) << 8) | userData[3]);
      Serial.print(F("\nUserData (int) 4: "));
      Serial.print( ( ( (uint16_t) userData[4] ) << 8) | userData[5]);
      Serial.print(F("\nUserData (int) 6: "));
      Serial.print( ( ( (uint16_t) userData[6] ) << 8) | userData[7]);
      Serial.print(F("\nUserData (int) 8: "));
      Serial.print( ( ( (uint16_t) userData[8] ) << 8) | userData[9]);
      Serial.print(F("\nUserData (int) 10: "));
      Serial.print( ( ( (uint16_t) userData[10] ) << 8) | userData[11]);



    }

};

#endif
