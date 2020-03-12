#include <Arduino.h>
#include "Pump.h"

Pump::Pump(byte p_input, byte n_input) {
  p = p_input;
  n = n_input;
}

void Pump::init(){
  pinMode(p, OUTPUT);
  pinMode(n, OUTPUT);
  Serial.println("PUMP: INITIALIZED");
}

void Pump::on_cw(){
  digitalWrite(p, HIGH);
  digitalWrite(n, LOW);
  running = true;
}

void Pump::on_ccw(){
  digitalWrite(p, LOW);
  digitalWrite(n, HIGH);
  running = true;
}

void Pump::off_limit(float limit_input){
  if(limit < limit_input && running == true){
    Serial.println("LIMIT REACHED");
    digitalWrite(p, LOW);
    digitalWrite(n, LOW);
    running = false;
  }
}

void Pump::off(){
  digitalWrite(p, LOW);
  digitalWrite(n, LOW);
  running = false;
}
