#include "Arduino.h"
#include "Potentiometer.h"

Potentiometer::Potentiometer(int pin) {
  this->pin = pin;
  pinMode(pin, INPUT);
}

int Potentiometer::readValue() {
  return analogRead(pin);
}

