
#ifndef _START_TABLES_H_
#define _START_TABLES_H_


void WSDFN::startupTables()
{
	if(!this->isController){
		//all sensor have a temperture sensor.
		sensorTable[0].sensorID=1;
		sensorTable[0].sensorType= TEMPERATURE;
		sensorTable[0].sensorType= CELSIUS;
		sensorTable[0].dataType= INT_8;
		sensorTable[0].interfaceType= ANALOGIC;
		sensorTable[0].interfaceNumber1 = 10;
		
	} else {
		

	}
}


#endif				//_START_TABLES_H_
