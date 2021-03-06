/*
 * Serial.c
 *
 * Created: 2014-08-17 13:16:49
 *  Author: muzieh
 */ 
#include "Serial.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define BaudRate 28800
#define MYUBRR (F_CPU / 16 / BaudRate ) - 1

#define RX PD0
#define TX PD1
#define SERIAL_BUFFER_SIZE 128
unsigned char buffer[SERIAL_BUFFER_SIZE];
int buffer_index = 0;
int write_buffer_index = 0;
int write_count = 0;

unsigned char ch;

ISR( USART_RXC_vect )
{
	buffer[buffer_index] = UDR;
	buffer_index = (buffer_index+1) % SERIAL_BUFFER_SIZE;
}

ISR( USART_UDRE_vect )
{
	
	UDR = buffer[write_buffer_index];
	write_count--;
	write_buffer_index = (write_buffer_index+1) % SERIAL_BUFFER_SIZE;
	if(write_count == 0)
	{
		UCSRB &= ~(1<<UDRIE);
		PORTD &= ~(1 << PD3);
	}
	
}


unsigned char serialCheckRxComplete(void)
{
	return( UCSRA & (1<<RXC)) ;		// nonzero if serial data is available to read.
}

unsigned char serialCheckTxReady(void)
{
	return( UCSRA & (1<<UDRE) ) ;		// nonzero if transmit register is ready to receive new data.
}





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

void serialBufferWrite(unsigned char *buffer, int count)
{
	PORTD |= (1 << PD3);
	write_buffer_index = 0;
	write_count = count;
	UCSRB |= 1<<UDRE;
	
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

	for(int i=0; i<SERIAL_BUFFER_SIZE; i++)
	{
		buffer[i] = '.';
	}
	/*Set baud rate */
	cli();
	UCSRC &= ~(1 << URSEL);
	UBRRH = (unsigned char)((MYUBRR)>>8);
	UBRRL = (unsigned char) MYUBRR;
	
	/* Enable receiver and transmitter   */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Frame format: 8data, No parity, 1stop bit */
	UCSRC |= (1 << URSEL) | (3<<UCSZ0);
	
	//enable receiver interrupt
	UCSRB |= (1<<RXCIE);

	sei();
	//establishContact();

	unsigned char a=0;

	while(1)
	{

		serialWrite('s');
		serialWrite('#');
		
		serialBufferWrite(buffer, SERIAL_BUFFER_SIZE);
		
		
		PORTD |= (1 << PD2);
		_delay_ms(10);
		PORTD &= ~(1 << PD2);
		_delay_ms(3000);

	  
	}
	return 0;
}