#ifndef __BUTTON__
#define __BUTTON__

class Button {
 
public: 
  virtual bool isPressed() = 0;
  virtual bool att_int(void (*func) (void)) = 0;
  virtual void dett_int(int)= 0;
  virtual bool isInterruptStart() = 0;
  virtual int getPin() = 0;
  
};

#endif
