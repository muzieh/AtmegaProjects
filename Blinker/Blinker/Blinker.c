/*
 * Blinker.c
 *
 * Created: 2014-06-30 21:24:23
 *  Author: muzieh
 */ 

#include <stdlib.h>
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "Led.h"
#include "Button.h"
#include <stdio.h>
#include <avr/interrupt.h>



typedef unsigned char  u8;
typedef   signed char  s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;


u32 t2_soft;
u32 getSeconds();
//int digitalRead(int port, int pin);
int digitalRead(int pin);

#define buttonWorkPin PB7;
#define buttonBreakPin PB6;

#define STATUS_STOP 1
#define STATUS_WORK 2
#define STATUS_BREAK 3
#define STATUS_LONG_BREAK 4

int currentStatus = STATUS_STOP;
long startTime;
long counterTime = 60 * 60 * 1000L; 

Button workButton;
Button breakButton;

ISR( TIMER2_OVF_vect )
{
	t2_soft += 256;
}


void t2_init( void )
{
	t2_soft = 0;
	TCCR2 = 1<<CS22 | 1<<CS21|1<<CS20;	// mode 0, prescaler 8
	TIMSK |= 1<<TOIE2;	// enable overflow interrupt
}


u32 get_ticks( void )			  // read T2 as 32 bit timer
{
	u32 val;
	u8 tifr;

	cli();
	val = t2_soft + TCNT2;
	tifr = TIFR;				  // read interrupt flags
	sei();
	if( (tifr & 1<<TOV2) && !(val & 0x80) ) // overflow prior reading TCNT2 ?
	val += 256;				  // then add overflow

	return val;

}
int millis()
{
	return 0;	
}



int digitalRead(int pin)
{
	return 0;
}


int updateButtonState(Button *button) 
{
  button->previousState = button->buttonState;
  int state = digitalRead(button->pin);
  _delay_ms(10);
  if(state == digitalRead(button->pin))
  {
    button->buttonState = state;
  }
  /*
  Serial.print("Button after ");
  Serial.print(button->buttonId);
  Serial.print(" ");
  Serial.print(button->previousState);
  Serial.print(" ");
  Serial.println(button->buttonState);
  */
  
  if ( button->previousState == BUTTON_OFF  && button->buttonState == BUTTON_ON )
  {
    button->onTime = millis();
  }
  else if ((button->previousState == BUTTON_ON) && (button->buttonState == BUTTON_OFF))
  {
    int onTime = millis() - button->onTime;
    if( onTime > 1000) {
      return PRESS_LONG;
    }
    else {
      return PRESS_SHORT;
    }
  }
  return PRESS_OFF;
}

void setUp()
{
	DDRD |= 1 << PD0;
	PORTD |= 1 << PD0;
	
	initialize_led();
	write_string("initializing...");
	t2_init();
	sei();
	_delay_ms(1000);
	clear_lcd();
	startTime = getSeconds();
}

void setUpButtons()
{
	 workButton.buttonState = BUTTON_OFF;
	 workButton.pin = buttonWorkPin;
	 workButton.transientState = BUTTON_OFF;
	 workButton.previousState = BUTTON_OFF;
	 
	 breakButton.buttonState = BUTTON_OFF;
	 breakButton.pin = buttonBreakPin;
	 breakButton.transientState = BUTTON_OFF;
	 breakButton.previousState = BUTTON_OFF;
}

void printHeaderLine() {
	switch(currentStatus) {
		case STATUS_STOP:
		write_string("idle...");
		move(1,0);
		write_string("ready to work?");
		return;
		case STATUS_WORK:
		write_string("Working");
		break;
		case STATUS_BREAK:
		write_string("Break");
		break;
		case STATUS_LONG_BREAK:
		write_string("Long break");
		break;
	}
}

void printTime() {
	clear_lcd();

	long timeFromStart = millis() - startTime;
	long remaining = counterTime - timeFromStart;

	printHeaderLine();
	if(currentStatus == STATUS_STOP)
	return;

	//lcd.print(remaining);
	move(1,0);

	int remainingSeconds = abs(remaining) / 1000;
	if(remaining < 0 )
	{
		write_string("-");
	}

	int minutes = remainingSeconds / 60;
	int seconds = remainingSeconds - minutes * 60;

	char lineBuffer[16];
	sprintf(lineBuffer,"%02d:%02d", minutes, seconds);
	write_string(lineBuffer);
}

u32 getSeconds()
{
	return (get_ticks()) / (u32)15625;
}

void startCounting(int countType) {
	currentStatus = countType;
	switch(currentStatus) {
		case STATUS_WORK:
		counterTime = 25 * 60 * 1000L;
		break;
		case STATUS_BREAK:
		counterTime = 5 * 60 * 1000L;
		break;
		case STATUS_LONG_BREAK:
		counterTime = 20 * 60 * 1000L;
		break;
	}
	startTime = millis();
}


int main(void)
{
	setUp();
	u32 timerticks;
	char buffer[100];
	char ledStatus = 1;
	clear_lcd();
	while(1)
	{
		
		ledStatus = ledStatus == 1 ? 0 : 1;
		move(0,0);
		if(PIND & (1<<PD1))
		{
			
			PORTD |= 1<<PD0;
			write_string("high");
		}
		else
		{
			PORTD &= ~(1<<PD0);
			write_string("low ");
		}
		move(1,0);
		u32 seconds = getSeconds();
		int minutes = seconds / 60;
		int remSeconds = seconds - minutes * 60;
		sprintf(buffer,"%02d:%02d", minutes, remSeconds);
		
		
		//ultoa(seconds, buffer, 10);
		write_string(buffer);
		
		_delay_ms(50);
	}
	
	
    while(1)
    {
		printTime();	
		
		switch(updateButtonState(&workButton))
		{
		case PRESS_LONG:
			currentStatus = STATUS_STOP;
			//Serial.println("trigger long work");
			break;
		case PRESS_SHORT:
			startCounting(STATUS_WORK);
			//Serial.println("trigger short work");
			break;
		case PRESS_OFF:
			//Serial.println("trigger nothing");
			break;
		}
  
		switch(updateButtonState(&breakButton))
		{
		case PRESS_LONG:
			startCounting(STATUS_LONG_BREAK);
			//Serial.println("trigger long work");
			break;
		case PRESS_SHORT:
			startCounting(STATUS_BREAK);
			//Serial.println("trigger short work");
			break;
		case PRESS_OFF:
			//Serial.println("trigger nothing");
			break;
		}
  
		/*
		Serial.print("Button ---- ");
		Serial.print(workButton.buttonId);
		Serial.print(" ");
		Serial.print(workButton.previousState);
		Serial.print(" ");
		Serial.println(workButton.buttonState);
		*/
		//if(workButton.buttonState == BUTTON_ON && workButton.previousState == BUTTON_OFF)
		//{
    
		//}

		/*
		updateButtonState(&breakButton);
		if(breakButton.buttonState == 1 && breakButton.previousState == 0)
		{
		startCounting(STATUS_BREAK);
		}
		*/
		_delay_ms(50);
  
		/*
		Serial.print(digitalRead(buttonWorkPin));
		Serial.write(" ");
		Serial.print(digitalRead(buttonBreakPin));
		Serial.write(" status");
		Serial.print(currentStatus);
		Serial.println();
		*/
		
    }
	
	//int digitalRead(int port, int pin)
	//{
		//
		//
	//}
	
	
	
	
}
