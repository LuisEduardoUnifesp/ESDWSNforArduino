//#include <EEPROM.h>
#include "EEPROMAnything.h"


//gets reponse from serial monitor to see if user wants to change particular setting
bool getAnswer()
{
  while (!Serial.available()) { }
  char c = Serial.read();
  while (Serial.available())  Serial.read();
  if (c == 'Y' || c == 'y') return true;
  else return false;
}

/**
   Struct for save configuration in EEPROM
*/
struct config_CSMA {
  unsigned int address;
};

struct config_CSMA configuration, configuration2;


void getSettings()
{
  int val;
  pinMode(10, INPUT_PULLUP); //set pin 8 as an input
  
  EEPROM_read(0, configuration);
  //if user make PIN10 down, change node address
  if (!digitalRead(10)) {
    Serial.begin(115200); //start serial communication, need to turn off before using sleep and WDT
    Serial.setTimeout(5000);
    //the following code reads the current settings from EEPROM and puts them in local variables
    Serial.print("Current settings in EEPROM\n");
    Serial.print("Node address: ");
    Serial.print(configuration.address);

    Serial.print("\nChange node address? (Y/N): ");
    if (getAnswer()) {
      Serial.print("\nEnter Node address to store in EEPROM: (5 seconds for timeout)");
      while (!(Serial.available())) {}
      val = Serial.parseInt();
      Serial.print("\nVal: ");
      Serial.print(val);
      configuration.address = val;
    }
    EEPROM_write(0, configuration);
    Serial.print("\nConfiguration Save...");
  }


}




void setup() {
  // put your setup code here, to run once:
  getSettings();


  EEPROM_read(0, configuration2);
  Serial.print("\nCurrent settings in EEPROM\n");
  Serial.print("Node address: ");
  Serial.print(configuration2.address);

}

void loop() {
  // put your main code here, to run repeatedly:

}
