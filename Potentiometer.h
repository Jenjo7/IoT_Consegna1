#ifndef __POTENTIOMETER__
#define __POTENTIOMETER__

class Potentiometer {
  public:
    Potentiometer(int pin);
    int readValue();
  private:
    int pin;
};

#endif

