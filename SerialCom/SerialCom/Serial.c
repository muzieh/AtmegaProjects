/*
 * Serial.c
 *
 * Created: 2014-08-17 13:16:49
 *  Author: muzieh
 */ 
#include "Serial.h"
#include <avr/io.h>
#include <util/delay.h>



#define BaudRate 9600
#define MYUBRR (F_CPU / 16 / BaudRate ) - 1

#define RX PD0
#define TX PD1


unsigned char serialCheckRxComplete(void)
{
	return( UCSRA & (1<<RXC)) ;		// nonzero if serial data is available to read.
}

unsigned char serialCheckTxReady(void)
{
	return( UCSRA & (1<<UDRE) ) ;		// nonzero if transmit register is ready to receive new data.
}



unsigned char buffer[5];

void serialRead(void)
{
	for(int i=0; i<5; i++)
	{
		while (serialCheckRxComplete() == 0)		// While data is NOT available to read
		{;}
		buffer[i] = UDR;	
	}
	//while (serialCheckRxComplete() == 0)		// While data is NOT available to read
	//{;;}
	//return UDR;
}



void serialWrite(unsigned char DataOut)
{
	while (serialCheckTxReady() == 0)		// while NOT ready to transmit
	{;;}
	UDR = DataOut;
}

void establishContact() {
	while (serialCheckRxComplete() == 0) {
		serialWrite('A');
		_delay_ms(500);
	}
}

int SerialInit()
{
	
	//DDRD &= ~(1<<RX);
	//DDRD |= (1<<TX);

	/*Set baud rate */
	UCSRC &= ~(1 << URSEL);
	UBRRH = (unsigned char)((MYUBRR)>>8);
	UBRRL = (unsigned char) MYUBRR;
	/* Enable receiver and transmitter   */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Frame format: 8data, No parity, 1stop bit */
	UCSRC |= (1 << URSEL) | (3<<UCSZ0);

	establishContact();

	unsigned char a=0;

	while(1)
	{

		
		
		
		serialWrite('s');
		serialRead();
		
		serialWrite('#');
		for(int i=0; i<5; i++)
		{
			
			serialWrite(buffer[i]);
		}
		
		PORTD |= (1 << PD2);
		//PORTD |= (1 << PD3);
		_delay_ms(80);
		PORTD &= ~(1 << PD2);
		//PORTD &= ~(1 << PD3);
		_delay_ms(80);

	  
	}
	return 0;
}