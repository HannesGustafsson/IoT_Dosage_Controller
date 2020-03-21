#include "HX711.h"
#include "Scale.h"

Scale::Scale(byte DT, byte SCK){
  hx711.begin(DT, SCK); // Setup for the hx711 signal amplifier
}

void Scale::init(){
  hx711.set_scale(cal_fact); // Sets the relationship between raw data and value in grams, calibration factor 370
  hx711.tare(); // Zeroes the scale
  Serial.println("SCALE: INITIALIZED");
}

// Enables power to the scale module
void Scale::on(){
  hx711.power_up();
  Serial.println("SCALE: ON");
}

// Disables power to the scale module to save power
void Scale::off(){
  hx711.power_down();
  Serial.println("SCALE: OFF");
}

// Sets current weight to zero point
void Scale::zero(){
  hx711.tare();
  value = 0;
  Serial.println("SCALE: TARE");
}

// Measures the weight once and saves it to "value" variable
void Scale::measure(){
  value = hx711.get_units(3);
  Serial.print(hx711.get_units(3), 1);
  Serial.println("g");
}
