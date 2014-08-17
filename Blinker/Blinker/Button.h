/*
 * Button.h
 *
 * Created: 2014-07-02 21:57:56
 *  Author: muzieh
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

#define BUTTON_ON 1
#define BUTTON_OFF 0

#define PRESS_LONG 2
#define PRESS_SHORT 1
#define PRESS_OFF 0


typedef struct  {
	int transientState;
	int buttonState;
	int previousState;
	int onTime;
	int pin;
	int buttonId;
} Button;



#endif /* BUTTON_H_ */