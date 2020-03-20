#include <Arduino.h>

class Scale {
  public:

  Scale(byte, byte);
  HX711 hx711;
  int value;
  int cal_fact = 370;

  void init();
  void on();
  void off();
  void zero();
  void measure();
};
