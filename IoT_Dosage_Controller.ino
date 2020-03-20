/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <WiFi101.h>
#include <BlynkSimpleMKR1000.h>

#include "HX711.h"  //HX711 Signal amplifier library
#include "Scale.h"  //The custom Scale library
#include "Pump.h"  //The custom Pump library

// Pump & Scale pins
#define SERIAL_CLOCK_OUTPUT A0
#define SERIAL_DATA_OUTPUT A1

#define PUMP_0_POSITIVE 3
#define PUMP_0_NEGATIVE 4

#define PUMP_1_POSITIVE 5
#define PUMP_1_NEGATIVE 6

#define PUMP_2_POSITIVE 7
#define PUMP_2_NEGATIVE 8

// Blynk code & Network Settings
char auth[] = "3bQD5Gx07mXDwURgOcgTfJF0_akMI0nA";
char ssid[] = "!";
char pass[] = "Hannes93Gustafsson";
BlynkTimer timer;

// Creating Scale & Pump objects
Scale scale(SERIAL_DATA_OUTPUT, SERIAL_CLOCK_OUTPUT);
Pump pump_0(PUMP_0_POSITIVE, PUMP_0_NEGATIVE);
Pump pump_1(PUMP_1_POSITIVE, PUMP_1_NEGATIVE);
Pump pump_2(PUMP_2_POSITIVE, PUMP_2_NEGATIVE);

int total_percentage; // Total liquid percentages, adding all "pump.percentages" together
int total_amount; // Total amount in mL, sets the max limit of the glass

void setup() {
  Serial.begin(9600);
  
  scale.init();
  pump_0.init();
  pump_1.init();
  pump_2.init();

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
}

BLYNK_WRITE(V0){
  pump_0.percentage = param.asInt();
  set_total();
  print_status();
}

BLYNK_WRITE(V1){
  pump_1.percentage = param.asInt();
  set_total();
  print_status();
}

BLYNK_WRITE(V2){
  pump_2.percentage = param.asInt();
  set_total();
  print_status();
}

BLYNK_WRITE(V3){
  total_amount = param.asInt();
  set_total();
  print_status();
}

void set_total(){
  total_percentage = pump_0.percentage + pump_1.percentage + pump_2.percentage;
  // To avoid dividing by 0
  if(total_percentage > 0){
    pump_0.amount = (float(pump_0.percentage)/float(total_percentage)) * total_amount;
    pump_1.amount = (float(pump_1.percentage)/float(total_percentage)) * total_amount;
    pump_2.amount = (float(pump_2.percentage)/float(total_percentage)) * total_amount;
  }
  else{
    pump_0.amount = 0;
    pump_1.amount = 0;
    pump_2.amount = 0;
  }
}

void print_status(){
  Serial.print("Total:\t");
  Serial.print(total_amount);
  Serial.print("mL  \t");
  Serial.print(total_percentage);
  Serial.println("%");
  Serial.println("------");

  // PUMP 0: STATUS
  Serial.print("Pump0:\t");
  Serial.print(pump_0.amount);
  Serial.print("mL  \t");
  Serial.print(pump_0.percentage);
  Serial.print("%\tActual: ");
  if(total_percentage > 0){
    Serial.print(int(float(pump_0.percentage)/float(total_percentage) * 100));
  }
  else{
    Serial.print("0");
  }
  Serial.print("%\t running: ");
  Serial.println(pump_0.running);

  // PUMP 1: STATUS
  Serial.print("Pump1:\t");
  Serial.print(pump_1.amount);
  Serial.print("mL  \t");
  Serial.print(pump_1.percentage);
  Serial.print("%\tActual: ");
  if(total_percentage > 0){
    Serial.print(int(float(pump_1.percentage)/float(total_percentage) * 100));
  }
  else{
    Serial.print("0");
  }
  Serial.print("%\t running: ");
  Serial.println(pump_1.running);

  // PUMP 2: STATUS
  Serial.print("Pump2:\t");
  Serial.print(pump_2.amount);
  Serial.print("mL  \t");
  Serial.print(pump_2.percentage);
  Serial.print("%\tActual: ");
  if(total_percentage > 0){
    Serial.print(int(float(pump_2.percentage)/float(total_percentage) * 100));
  }
  else{
    Serial.print("0");
  }
  Serial.print("%\t running: ");
  Serial.println(pump_2.running);

  Serial.print("CURRENT:");
  Serial.print(scale.value);
  Serial.println("mL  \t");
  Serial.println("-----------------------------------");
}
// BENCHMARK, How long it takes to pump 1dl of water
// PUMP_0 CW:1m30s CCW:1m30s
// PUMP_1 CW:1m30s CCW:1m30s
// PUMP_2 CW:1m15s CCW:1m15s
// ALL    CW:30s
