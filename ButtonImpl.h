#ifndef __BUTTONIMPL__
#define __BUTTONIMPL__

#include "Button.h"

class ButtonImpl: public Button {
 
public: 
  ButtonImpl(int pin);
  bool isPressed();
  bool att_int(void (*func)(void) );
  void dett_int(int);
  int getPin();

private:
  int pin;
};

#endif
