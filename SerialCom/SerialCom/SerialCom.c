/*
 * SerialCom.c
 *
 * Created: 2014-08-17 12:17:33
 *  Author: muzieh
 */ 



#define LED_PIN PD2
#define LED_PIN1 PD3

#include "Settings.h"
#include <util/delay.h>
#include <avr/io.h>

#include "Serial.h"

void setUp()
{
	DDRD |= 1<<LED_PIN | 1<<LED_PIN1;
	PORTD |= 1<<LED_PIN;
	PORTD &= ~(1<<LED_PIN);
	SerialInit();
}


int main(void)
{
	setUp();
	
    while(1)
    {
		PORTD &= ~(1<<LED_PIN);
		_delay_ms(2000);
		PORTD |= 1<<LED_PIN;
		_delay_ms(20);
    }
}