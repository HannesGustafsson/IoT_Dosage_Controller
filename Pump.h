#include <Arduino.h>

class Pump {
  public:
  
  Pump(byte, byte);
  byte p;                  // Positive pin
  byte n;                  // Negative pin
  bool running = false;    // Is pump turned on
  bool calibration_start = false; // Is the calibration started
  bool calibrated = false; // Is it calibrated
  float amount;            // Set amount of liquid for it to pump
  float amount_pumped;     // Amount of liquid delivered
  float flow_rate = 1;     // The speed of the pump relative to the others
  int percentage;          // Percentage of the tptal amount of liquid

  void init();
  void calibrate(float, float, unsigned long&, unsigned long&, bool&);
  void on_cw(float);
  void on_ccw();
  void off_amount(float, float, float);
  void off();
};
