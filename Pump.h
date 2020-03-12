#include <Arduino.h>

class Pump {
  public:
  
  Pump(byte, byte);
  byte p;
  byte n;
  bool running = false;
  byte flow_rate = 1;
  float limit;

  void init();
  void on_cw();
  void on_ccw();
  void off_limit(float);
  void off();
};
