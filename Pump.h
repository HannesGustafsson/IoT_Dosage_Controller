#include <Arduino.h>

class Pump {
  public:
  
  Pump(byte, byte);
  byte p;
  byte n;
  bool running = false;
  int amount;
  int percentage;

  void init();
  void on_cw(int);
  void on_ccw();
  void off_amount(int);
  void off();
};
