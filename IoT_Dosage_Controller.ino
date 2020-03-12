#include "HX711.h"  //HX711 Signal amplifier library
#include "Scale.h"  //The custom Scale library
#include "Pump.h"  //The custom Pump library

#define SERIAL_CLOCK_OUTPUT A0
#define SERIAL_DATA_OUTPUT A1

#define PUMP_0_POSITIVE 3
#define PUMP_0_NEGATIVE 4

#define PUMP_1_POSITIVE 5
#define PUMP_1_NEGATIVE 6

#define PUMP_2_POSITIVE 7
#define PUMP_2_NEGATIVE 8


/* PIN LIST
 *  SCALE
   *  SERIAL_CLOCK_OUTPUT/SCK:  A0 
   *  SERIAL_DATA_OUTPUT/DT:    A1   
   *  VCC:  5v
   *  GND:  GND
 *  ESP
   *  RX:   6
   *  TX:   7
   *  VCC:  3.3v
   *  GND:  GND
*/

Scale scale(SERIAL_DATA_OUTPUT, SERIAL_CLOCK_OUTPUT);
Pump pump_0(PUMP_0_POSITIVE, PUMP_0_NEGATIVE);
Pump pump_1(PUMP_1_POSITIVE, PUMP_1_NEGATIVE);
Pump pump_2(PUMP_2_POSITIVE, PUMP_2_NEGATIVE);

void setup() {
  Serial.begin(9600);
  
  scale.init();
  pump_0.init();
  pump_1.init();
  pump_2.init();

  pump_0.limit = 10;
  pump_1.limit = 15;
  pump_2.limit = 30;
}

void loop() {
  if(Serial.available()){
    char temp = Serial.read();
    
    if (temp == 'm'){
      scale.measure();
    }
    else if(temp == 't'){
      scale.zero();  //Reset the scale to zero
    }
    else if(temp == '+'){
      pump_0.on_cw();
      pump_1.on_cw();
      pump_2.on_cw();
    }
    else if(temp == '-'){
      pump_0.on_ccw();
      pump_1.on_ccw();
      pump_2.on_ccw();
    }
    else if(temp == '.'){
      pump_0.off();
      pump_1.off();
      pump_2.off();
    }
  }
  
  else if(scale.value > 25){
      pump_0.off();
      pump_1.off();
      pump_2.off();
      scale.zero();
  }
  
  else{
    scale.measure();
    pump_0.off_limit(scale.value);
    pump_1.off_limit(scale.value);
    pump_2.off_limit(scale.value);
  }
}

// BENCHMARK, How long it takes to pump 1dl of water
// PUMP_0 CW:1m30s CCW:1m30s
// PUMP_1 CW:1m30s CCW:1m30s
// PUMP_2 CW:1m15s CCW:1m15s
// ALL    CW:30s
