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

#define WATER_IN 0
#define WATER_OUT 1

// Blynk code & Network Settings
char auth[] = "3bQD5Gx07mXDwURgOcgTfJF0_akMI0nA";
char ssid[] = "!";
char pass[] = "Hannes93Gustafsson";
BlynkTimer timer;
int start_timer;
bool running = false;
bool reverse = false;

unsigned long start_millis;
unsigned long current_millis;

// Creating Scale & Pump objects
Scale scale(SERIAL_DATA_OUTPUT, SERIAL_CLOCK_OUTPUT);
Pump pump_0(PUMP_0_POSITIVE, PUMP_0_NEGATIVE);
Pump pump_1(PUMP_1_POSITIVE, PUMP_1_NEGATIVE);
Pump pump_2(PUMP_2_POSITIVE, PUMP_2_NEGATIVE);

// The use of an array of Pump pointers could aso be used in this program
// to enable the use of for-loops instead of repeating code, which would be useful is more pumps were used,
// as well as creating new options for cycling through the object varables.
// WILL NOT BE USED IN THE FOLLOWING PROGRAM. DEMONSTRATION PURPOSES ONLY:
//#define ARRAY_SIZE 3
//Pump *pump_array[ARRAY_SIZE] = { new Pump(PUMP_0_POSITIVE, PUMP_0_NEGATIVE),
//                                 new Pump(PUMP_1_POSITIVE, PUMP_1_NEGATIVE),
//                                 new Pump(PUMP_2_POSITIVE, PUMP_2_NEGATIVE)};


int total_percentage; // Total liquid percentages, adding all "pump.percentage" together
int total_amount; // Total amount in mL, sets the max limit of the glass

// ==================================================< PROGRAM START >==================================================
void setup() 
{
  Serial.begin(9600);
  
  scale.init();
  pump_0.init();
  pump_1.init();
  pump_2.init();

  pinMode(WATER_OUT, OUTPUT);
  digitalWrite(WATER_OUT, LOW);
  pinMode(WATER_IN, INPUT);
  Blynk.begin(auth, ssid, pass);
  start_timer = timer.setInterval(300L, start);
  timer.disable(start_timer);
}


void loop() 
{
  Blynk.run();
  timer.run();
}
// ==================================================< PROGRAM END >==================================================


// Controls the 1st Blynk slider & and values for pump_0
BLYNK_WRITE(V0)
{
  set_percentage(param.asFloat(), pump_0, pump_1, pump_2); // Uses "V0" slider parameter as float & pointers to all three pumps
  set_total(); // Recalculates total amount (of liquid) & individual amount for the pumps
  print_status(); // Prints all pump values & total values
}


// Controls the 2nd Blynk slider & and values for pump_1
BLYNK_WRITE(V1)
{
  set_percentage(param.asFloat(), pump_1, pump_0, pump_2);
  set_total();
  print_status();
}


// Controls the 3rd Blynk slider & and values for pump_2
BLYNK_WRITE(V2)
{
  set_percentage(param.asFloat(), pump_2, pump_0, pump_1);
  set_total();
  print_status();
}


// Controls the 4th Blynk slider & the maximum liquid value
BLYNK_WRITE(V3)
{
  total_amount = param.asInt();
  set_total();
  print_status();
}

// Controls the Blynk start button which in turn enables the start function
BLYNK_WRITE(V7)
{
  if(param.asInt() == 1)
  {
    scale.on();
    timer.enable(start_timer);
    reverse = false;
  }
  else
  {
    pump_0.off();
    pump_1.off();
    pump_2.off();
    
    timer.disable(start_timer);
    scale.off();
    running = false;
  }
}

BLYNK_WRITE(V10)
{
  if(param.asInt() == 1)
  {
    bool temp = water_connection();
  }
}


void start()
{
  if (running == false)
  {
    scale.zero();
    Serial.println("STARTING");
    pump_0.on_cw(scale.value);
    pump_1.on_cw(scale.value);
    pump_2.on_cw(scale.value);
    running = true;
  }
  else if(running == true && water_connection() == false)
  {
    pump_0.off();
    pump_1.off();
    pump_2.off();
  }
  else if (running == true)
  {
    scale.measure();
    Blynk.virtualWrite(V8, scale.value);
    
    int pump_count = pumps_running();
    pump_0.off_amount(scale.value, scale.previous_value, pump_count);
    pump_1.off_amount(scale.value, scale.previous_value, pump_count);
    pump_2.off_amount(scale.value, scale.previous_value, pump_count);
    
    clean();
  }
}


// Runs the pumps once in reverse for 3.5 seconds to remove all excess liquid from the tubes
// and provide a clean start for all pump, meaning that they have to pump liquid the same distance
// after for example only one pump was used previously
void clean()
{
  if(pump_0.running == false && pump_1.running == false && pump_2.running == false)
    {
      current_millis = millis();
      
      if(reverse == false)
      {
        start_millis = millis();
        reverse = true;
        Serial.println("REVERSE START");
      }
      else if(reverse == true && (current_millis - start_millis < 3500))
      {
        pump_0.on_ccw();
        pump_1.on_ccw();
        pump_2.on_ccw();
      }
      else
      {
        pump_0.off();
        pump_1.off();
        pump_2.off();
      }
    }
}

// Sends a electrical pulse through the water sensors and controld if the signal returns
// thus implying the that all three water tubes are connected to a conductive liquid
bool water_connection()
{
  digitalWrite(WATER_OUT, HIGH);
  
  if(digitalRead(WATER_IN == HIGH))
  {
    Serial.println("WATER CONNECTION: TRUE");
    digitalWrite(WATER_OUT, LOW);
    return true;
    
  }
  else
  {
    Serial.println("WATER CONNECTION: FALSE");
    digitalWrite(WATER_OUT, LOW);
    return false;
  }
}


// Calculates the individual percentage of the total user specified amount of liquid a single pump is to deliver
// based on the value of the others, to make sure the total percentage is never over 100%
// and to enable accurate control of all three pumps individually
void set_percentage(float input, Pump& a, Pump& b, Pump& c)
{
  // If the (total amount + input) is more than 100%, only add the difference between 100% and the already specified total
  if(input + (b.percentage + c.percentage) >= 100)
  {
    a.percentage = 100 - (b.percentage + c.percentage);
    Serial.println(a.percentage);
  }
  // Otherwise just use input value
  else
  {
    a.percentage = input;
    Serial.println(a.percentage);
  }

  // Overwrites the Blynk sliders to display the correct percentage value
  Blynk.virtualWrite(V0, pump_0.percentage);
  Blynk.virtualWrite(V1, pump_1.percentage);
  Blynk.virtualWrite(V2, pump_2.percentage);
}


// Calculates amount for each pump to deliver based on total amount of liquid specified
// and percentage value of the pumps
void set_total()
{
  total_percentage = pump_0.percentage + pump_1.percentage + pump_2.percentage;
  
  // To avoid dividing by 0
  if(total_percentage > 0){
    
    pump_0.amount = (float(pump_0.percentage) * total_amount) / 100; // Cast to float during calculation since value is 0 - 1 and int would return 0
    pump_1.amount = (float(pump_1.percentage) * total_amount) / 100;
    pump_2.amount = (float(pump_2.percentage) * total_amount) / 100;
  }
  else{
    pump_0.amount = 0;
    pump_1.amount = 0;
    pump_2.amount = 0;
  }
  
  // Displays the calculated amount of mL for each pump on buttons underneath the sliders
  Blynk.virtualWrite(V4, pump_0.amount);
  Blynk.virtualWrite(V5, pump_1.amount);
  Blynk.virtualWrite(V6, pump_2.amount);
}

// Returns the number of active pumps
int pumps_running()
{
  int temp = 0;
  if(pump_0.running == true) {temp += 1;}
  if(pump_1.running == true) {temp += 1;}
  if(pump_2.running == true) {temp += 1;}
  return temp;
}

// Prints all the values of chosen pump
void pump_status(Pump p)
{
  Serial.print(p.amount);
  Serial.print("mL  \t");
  Serial.print(p.percentage);
  Serial.print("%\t running: ");
  Serial.println(p.running);
}


// Prints total values, status of all pumps, as well as current measurement
void print_status()
{
  Serial.print("Total:\t");
  Serial.print(total_amount);
  Serial.print("mL  \t");
  Serial.print(total_percentage);
  Serial.println("%\n-----");

  Serial.print("Pump0:\t");
  pump_status(pump_0);
  Serial.print("Pump1:\t");
  pump_status(pump_1);
  Serial.print("Pump2:\t");
  pump_status(pump_2);

  // CURRENT: STATUS
  Serial.print("CURRENT:");
  Serial.print(scale.value);
  Serial.println("mL\n-----------------------------------");
}

// BENCHMARK, How long it takes to pump 1dl of water
// PUMP_0 CW:1m30s CCW:1m30s
// PUMP_1 CW:1m30s CCW:1m30s
// PUMP_2 CW:1m15s CCW:1m15s
// ALL    CW:30s
