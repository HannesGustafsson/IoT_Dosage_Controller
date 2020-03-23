#include <Arduino.h>

class Pump {
  public:
  
  Pump(byte, byte);
  byte p;
  byte n;
  bool running = false;
  bool calibration_start = false;
  bool calibrated = false;
  float amount;
  float amount_pumped;
  float flow_rate = 1;
  int percentage;

  void init();
  void calibrate(float, float, unsigned long&, unsigned long&, bool&);
  void on_cw(float);
  void on_ccw();
  void off_amount(float, float, float);
  void off();
};
