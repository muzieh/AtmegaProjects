/*
 * Board1Test.c
 *
 * Created: 2014-07-27 20:05:53
 *  Author: muzieh
 */ 

#define F_CPU 1000000UL
#include <util/delay.h>

#include <avr/io.h>

#define BaudRate 9600
#define MYUBRR (F_CPU / 16 / BaudRate ) - 1

void delayLong()
{
	unsigned int delayvar;
	delayvar = 0;
	while (delayvar <=  65500U)
	{
		asm volatile("nop");
		delayvar++;
	}
}


unsigned char serialCheckRxComplete(void)
{
	return( UCSRA & _BV(RXC)) ;		// nonzero if serial data is available to read.
}

unsigned char serialCheckTxReady(void)
{
	return( UCSRA & _BV(UDRE) ) ;		// nonzero if transmit register is ready to receive new data.
}

unsigned char serialRead(void)
{
	while (serialCheckRxComplete() == 0)		// While data is NOT available to read
	{;;}
	return UDR;
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
		//	serialWrite(65U);
		delayLong();
		delayLong();
		delayLong();
		delayLong();
		delayLong();
		delayLong();
		delayLong();
	}
}




int main(void)
{
	DDRD |= 1 << PD2; // output

	DDRD = _BV(1);
	DDRB = _BV(0) | _BV(1) | _BV(3) | _BV(5);

	//Serial Initialization

	/*Set baud rate */
	UCSRC &= ~(1 << URSEL);
	UBRRH = (unsigned char)(MYUBRR>>8);
	UBRRL = (unsigned char) MYUBRR;
	/* Enable receiver and transmitter   */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Frame format: 8data, No parity, 1stop bit */
	UCSRC |= (1 << URSEL) | (3<<UCSZ0);



	unsigned int firstSensor = 0;    // first analog sensor
	int secondSensor = 0;   // second analog sensor
	int thirdSensor = 0;    // digital sensor
	unsigned char inByte = 0;         // incoming serial byte


	PORTB |= _BV(1); // Turn on LED @ PB1


	establishContact();  // send a byte to establish contact until Processing responds

	PORTB &= 253U; // Turn off LED


    while(1)
    {
		//if (serialCheckRxComplete()) {

			
			inByte++ ;
			
			// Simulated data!
			
			
while(1) {
			serialWrite('s');
			serialWrite('e');
			serialWrite('n');
			serialWrite('s');
			serialWrite('o');
			serialWrite('r');
			serialWrite(':');
			serialWrite(inByte & 255U);
}

			//PORTB &= 253U; // Turn off LED
			
		//}
		
		PORTD |= (1 << PD2);	
		_delay_ms(50);
		PORTD &= ~(1 << PD2);
		_delay_ms(50);
        
    }
}