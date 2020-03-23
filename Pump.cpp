#include <Arduino.h>
#include "Pump.h"

Pump::Pump(byte p_input, byte n_input) 
{
  this->p = p_input;
  this->n = n_input;
}


void Pump::init()
{
  pinMode(this->p, OUTPUT);
  pinMode(this->n, OUTPUT);
  Serial.println("PUMP: INITIALIZED");
}


void Pump::on_cw(float value)
{
  if((value + 1) < this->amount)
  {
    digitalWrite(this->p, HIGH);
    digitalWrite(this->n, LOW);
    this->running = true;
  }
}

void Pump::on_ccw()
{
  digitalWrite(this->p, LOW);
  digitalWrite(this->n, HIGH);
}


// Calculates amount of liquid each pump has delivered
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

  if(this->amount_pumped > this->amount && this->running == true)
  {
    Serial.println("LIMIT REACHED");
    digitalWrite(this->p, LOW);
    digitalWrite(this->n, LOW);
    this->amount_pumped = 0;
    this->running = false;
  }
}


void Pump::off()
{
  digitalWrite(p, LOW);
  digitalWrite(n, LOW);
  this->amount_pumped = 0;
  this->running = false;
}
