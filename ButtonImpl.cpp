#include "ButtonImpl.h"
#include "Arduino.h"

ButtonImpl::ButtonImpl(int pin){
  this->pin = pin;
  pinMode(pin, INPUT_PULLUP);     
} 
  
bool ButtonImpl::isPressed(){
  return digitalRead(pin) == HIGH;
}

bool ButtonImpl::att_int(void (*func)(void) ){
  if(this->pin == 2 || this->pin == 3) {
    attachInterrupt(digitalPinToInterrupt(pin), &(*func), RISING);
    return true;
  } else {
    
    return false;
  }
}

void ButtonImpl::dett_int(int pin) {
  detachInterrupt(digitalPinToInterrupt(pin));
}

int ButtonImpl::getPin() {
  return this->pin;
}

