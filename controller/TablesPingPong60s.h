#ifndef _TABLES_PINGPONG_H_
#define _TABLES_PINGPONG_H_


/**
   Initializes tables so that network works as data collector

*/
void startTables(int n)
{


  double interval;
  int spaceDelay = 1500; //time space between sensors send data (in miliseconds)
  int dataTime = 55000;  //duration of report data sensors (in miliseconds)
  int repeat = 60000; // time for repetitions
  unsigned long t0 = timezero + 183500ul; //TX_CONTROL start time 5min3s
  unsigned long transmission = 2000ul; //2seconds after
  int lastNode = n;
  //    #ifdef DEBUGFLOW
  ///    trace() << "Starting Controller Tables for " << SELF_NETWORK_ADDRESS;
  //    #endif


  for (int j = 1; j < SIZEOF_FLOW_TABLE; j++) flowTable[j].active = false;

  // Flow for received data
  flowTable[0].flowID = 0;
  flowTable[0].actionID = 2;
  flowTable[0].timeout = 0;
  flowTable[0].counters = 0;
  flowTable[0].active = true;
  //destination is the node itself, so it is received sensor data
  flowTable[0].fa.sourceAddress = 0;
  flowTable[0].fa.sourceMask = 0; //source address mask 1bit
  flowTable[0].fa.destinationAddress = 0;
  flowTable[0].fa.destinationMask = 0xFFFF;
  flowTable[0].fc.operatorC = NONE;

//  Serial.print(F("\n1: flowTable[0].active: "));
//  Serial.println(flowTable[0].active);

  for (int j = 0; j < SIZEOF_PROGRAMMABLE_TIMER_TABLE; j++) timerTable[j].active = false;

  //timer 0: wait for receive report temperature for all sensors immediately
  timerTable[0].timerID = 0;
  timerTable[0].actionID = 0;
  timerTable[0].t0 =  t0; //
  timerTable[0].countdown  = repeat; //repeat after 5 minutes
  timerTable[0].looping = 255; //repeat indefinitely
  timerTable[0].multiply = 1;
  timerTable[0].executed = 0;
  timerTable[0].active = true;

  //timer 1: +30 seconds and 2 minutes for deactivate  all sensors
  timerTable[1].timerID = 1;
  timerTable[1].actionID = 1;
  timerTable[1].t0 = t0 + dataTime; //40 seconds after
  timerTable[1].countdown  = repeat; //repeat after 5 minutes
  timerTable[1].looping = 255; //repeat indefinitely
  timerTable[1].multiply = 1;
  timerTable[1].executed = 0;
  timerTable[1].active = true;

  //action 0=Activate Radio
  actionTable[0].actionID = 0;
  actionTable[0].actionType = A_RECEIVER_ON;
  actionTable[0].active = true;
  //action 1=Deactivate Radio
  actionTable[1].actionID = 1;
  actionTable[1].actionType = A_RECEIVER_OFF;
  actionTable[1].active = true;
  //action 2=Read Sensor
  actionTable[2].actionID = 2;
  actionTable[2].actionType = A_RETURN_TO_ORIGIN;
  actionTable[2].active = true;




  for (int j = 3; j < SIZEOF_ACTION_TABLE; j++) actionTable[j].active = false;

//  Serial.print(F("\n1.2: flowTable[0].active: "));
//  Serial.println(flowTable[0].active);


  //for all nodes except controller
  for (int i = 1; i <= n; i++) {
    {


      programPackages[i] = 0;
      programmedPackages[i] = 0;

      //timer 0: 40 seconds for preparate report temperature in all sensors
      timerTableC[0][i].timerID = 0;
      timerTableC[0][i].actionID = 0;
      timerTableC[0][i].t0 = t0; //
      timerTableC[0][i].countdown  = repeat; //repeat in 5 minutes
      timerTableC[0][i].looping = 255; //repeat indefinitely
      timerTableC[0][i].executed = 0;
      timerTableC[0][i].multiply = 1;
      timerTableC[0][i].active = true;
      //timer 1: +30 seconds and 2 minutes for deactivate  all sensors
      timerTableC[1][i].timerID = 1;
      timerTableC[1][i].actionID = 1;
      timerTableC[1][i].t0 = t0 + dataTime; //
      timerTableC[1][i].countdown  = repeat; //repeat in 5 minutes
      timerTableC[1][i].looping = 255; //repeat indefinitely
      timerTableC[1][i].executed = 0;
      timerTableC[1][i].multiply = 1;
      timerTableC[1][i].active = true;
      //action 0=Activate Radio
      actionTableC[0][i].actionID = 0;
      actionTableC[0][i].actionType = A_RECEIVER_ON;
      actionTableC[0][i].active = true;
      //action 1=Deactivate Radio
      actionTableC[1][i].actionID = 1;
      actionTableC[1][i].actionType = A_RECEIVER_OFF;
      actionTableC[1][i].active = true;

//      Serial.print(F("\nL0: flowTable[0].active: "));
//      Serial.println(flowTable[0].active);





      //timer 2: 5 minutes for read temperature in all sensors
      timerTableC[2][i].timerID = 2;
      timerTableC[2][i].actionID = 2;

      //Timer of sersor with sequencial delay
      timerTableC[2][i].t0 = t0 + ((0.0 + spaceDelay) * i ) + transmission;
      //      #ifdef DEBUGFLOW
      //      trace() << "Sensor Delay: " <<  (100*timerTableC[2][i].t0);
      //      #endif

      //Timer of sensor with randomic delay
      //      interval = rand() % 300;
      //      trace() << "Sensor Delay: " << interval << " != " << 300;
      //      timerTableC[2][i].t0=1.0+ ( (double)interval / 1000 ); //inicial time 9 seconds + 0 a 100 miliseconds for avoid collision

      //Timer of sensor without delay
      //      timerTableC[2][i].t0=9.0; //inicial time 9 seconds

      timerTableC[2][i].countdown = repeat; //repeat in 2 minutes
      timerTableC[2][i].looping = 255; //repeat indefinitely
      timerTableC[2][i].multiply = 1;
      timerTableC[2][i].executed = 0;
      timerTableC[2][i].active = true;
      for (int j = 3; j < SIZEOF_PROGRAMMABLE_TIMER_TABLE; j++) timerTableC[j][i].active = false;

      //The sink not read your sensor
      if (i == 0) {
        timerTableC[2][i].active = false;
      }

//      Serial.print(F("\nL1: flowTable[0].active: "));
//      Serial.println(flowTable[0].active);

      //action 2=Read Sensor
      actionTableC[2][i].actionID = 2;
      actionTableC[2][i].actionType = A_READ_SENSOR;
      actionTableC[2][i].value1 = 0; //  = 0;
      actionTableC[2][i].active = true;

      //action 3=send temperature read to nexthop
      actionTableC[3][i].actionID = 3;
      actionTableC[3][i].actionType = A_SEND_TO_SINK;
      actionTableC[3][i].active = true;
      //for each node the nexthop is different

      //      stringstream out; out << i; string SNA = out.str(); //TODO ADJUST the nexthop

      //TODO routing decision
      actionTableC[3][i].value1  = 0;

//      Serial.print(F("\nL2: flowTable[0].active: "));
//      Serial.println(flowTable[0].active);


      //      #ifdef DEBUGFLOW
      //      trace() << "NextHop: " << actionTableC[3][i].value1  ;
      //      #endif

      //action 4=Display Sensor
      actionTableC[4][i].actionID = 4;
      actionTableC[4][i].actionType = A_DISPLAY;
      actionTableC[4][i].active = true;

      for (int j = 5; j < SIZEOF_ACTION_TABLE; j++) actionTableC[j][i].active = false;

      // Flow for read Sensor
      flowTableC[0][i].flowID = 0;
      flowTableC[0][i].actionID = 3;
      //      flowTableC[0].priority=1;
      flowTableC[0][i].timeout = 0;
      flowTableC[0][i].counters = 0;
      flowTableC[0][i].active = true;
      //source is the node itself, so it is a read sensor data
      flowTableC[0][i].fa.sourceAddress = i;
      flowTableC[0][i].fa.sourceMask = 0xFFFF; //source address mask 1bit
      flowTableC[0][i].fa.destinationAddress = -1;
      flowTableC[0][i].fa.destinationMask = 0;
      flowTableC[0][i].fc.operatorC = NONE;

      //Serial.print(F("\nL3: flowTable[0].active: "));
      //Serial.println(flowTable[0].active);




      //Flow send to Sink
      flowTableC[1][i].flowID = 1;
      //resend to sink or display
      if (i == sink)   flowTableC[1][i].actionID = 4;
      else            flowTableC[1][i].actionID = 3;
      //      flowTableC[1][i].priority=1;
      flowTableC[1][i].timeout = 0;
      flowTableC[1][i].counters = 0;
      flowTableC[1][i].active = true;
      flowTableC[1][i].fa.sourceAddress = 0;
      flowTableC[1][i].fa.sourceMask = 0;
      flowTableC[1][i].fa.destinationAddress = sink;
      flowTableC[1][i].fa.destinationMask = 0xFFFF;
      flowTableC[1][i].fc.operatorC = NONE;

      //Flow received from Sink
      // Flow for received data
      flowTableC[2][i].flowID = 2;
      flowTableC[2][i].actionID = 4;
      flowTableC[2][i].timeout = 0;
      flowTableC[2][i].counters = 0;
      flowTableC[2][i].active = true;
      //destination is the node Sink, so it is received Pong
      flowTableC[2][i].fa.sourceAddress = 0;
      flowTableC[2][i].fa.sourceMask = 0xFFFF; //source address mask 1bit
      flowTableC[2][i].fa.destinationAddress = 0;
      flowTableC[2][i].fa.destinationMask = 0;
      flowTableC[2][i].fc.operatorC = NONE;




      //Serial.print(F("\nL4: flowTable[0].active: "));
      //Serial.println(flowTable[0].active);
      //Serial.println((int)&flowTable[0].active);

      for (int j = 3; j < SIZEOF_FLOW_TABLE; j++) {
        flowTableC[j][i].active = false;
//        Serial.println(F("flowTableC[j][i].active"));
//        Serial.println((int)&flowTableC[j][i].active);
//        Serial.println(F("J:I"));
//        Serial.print(j);
//        Serial.print(F(":"));
//        Serial.println(i);
      }


//      Serial.print(F("\nL5: flowTable[0].active: "));
//      Serial.println(flowTable[0].active);

    }


  } // for



//  Serial.print(F("\n2: flowTable[0].active: "));
//  Serial.println(flowTable[0].active);



}


#endif        //_TABLES_PINGPONG_H_
