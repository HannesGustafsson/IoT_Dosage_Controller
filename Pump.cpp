#include <Arduino.h>
#include "Pump.h"

Pump::Pump(byte p_input, byte n_input) 
{
  this->p = p_input;
  this->n = n_input;
}

// Sets pins and prints status
void Pump::init()
{
  pinMode(this->p, OUTPUT);
  pinMode(this->n, OUTPUT);
  Serial.println("PUMP: INITIALIZED");
}

// Turns on pump clock wise if set amount is higher than scale value + 1 to account for inconsistencies
void Pump::on_cw(float value)
{
  if((value + 1) < this->amount)
  {
    digitalWrite(this->p, HIGH);
    digitalWrite(this->n, LOW);
    this->running = true;
  }
}

// Turns on pump counter clock wise
void Pump::on_ccw()
{
  digitalWrite(this->p, LOW);
  digitalWrite(this->n, HIGH);
}


// Calculates amount of liquid each pump has delivered based on it's percentage of the total flow rate
// by taking the pump's personal flow rate and dividing it by the flow of all currently running pumps added together 
void Pump::off_amount(float value, float previous_value, float flow)
{
  this->amount_pumped += ((value - previous_value) * (this->flow_rate/flow));
  if(this->running == true)
  {
  Serial.print(amount_pumped);
  Serial.print(" += (");
  Serial.print(value);
  Serial.print(" - ");
  Serial.print(previous_value);
  Serial.print(") / ");
  Serial.println(this->flow_rate/flow);
  }

  // If pumped amount exceeds the set amount the pump is turned off and status set to not running
  if(this->amount_pumped > this->amount && this->running == true)
  {
    Serial.println("LIMIT REACHED");
    digitalWrite(this->p, LOW);
    digitalWrite(this->n, LOW);
    this->amount_pumped = 0;
    this->running = false;
  }
}

// Turns off pump
void Pump::off()
{
  digitalWrite(p, LOW);
  digitalWrite(n, LOW);
  this->amount_pumped = 0;
  this->running = false;
}
