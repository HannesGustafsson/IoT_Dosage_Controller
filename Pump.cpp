#include <Arduino.h>
#include "Pump.h"

Pump::Pump(byte p_input, byte n_input) 
{
  p = p_input;
  n = n_input;
}


void Pump::init()
{
  pinMode(p, OUTPUT);
  pinMode(n, OUTPUT);
  Serial.println("PUMP: INITIALIZED");
}


void Pump::on_cw(int scale_value)
{
  if((scale_value + 1) < amount)
  {
    digitalWrite(p, HIGH);
    digitalWrite(n, LOW);
    running = true;
  }
}

void Pump::on_ccw()
{
  digitalWrite(p, LOW);
  digitalWrite(n, HIGH);
  running = true;
}


void Pump::off_amount(int value, int previous_value, int pump_count)
{
  amount_pumped += ((float(value) - float(previous_value)) / float(pump_count));
  
  if(amount_pumped > amount && running == true)
  {
    Serial.println("LIMIT REACHED");
    digitalWrite(p, LOW);
    digitalWrite(n, LOW);
    amount_pumped = 0;
    running = false;
  }
}


void Pump::off()
{
  digitalWrite(p, LOW);
  digitalWrite(n, LOW);
  amount_pumped = 0;
  running = false;
}
