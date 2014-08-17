#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "Led.h"


#define RS	PC0
#define RW	PC1
#define E	PC2
#define DB7	PB3
#define DB6	PB2
#define DB5 PB1
#define DB4	PB0



void move(unsigned char row, unsigned char column)
{
	unsigned char command = 0b10000000;
	if(row ==1)
	{
		command += 0x40;
	}
	command += column;
	send_command(command);
}

void initialize_led()
{
	DDRC |= (1<<RS) | (1<<RW) | (1<<E);
	PORTC &= ~((1 << RS) | (1 << RW) | (1 << E));
	DDRB =  0xff;
	PORTB = 0;
	_delay_ms(45);
	
	PORTC |= (1 << E);
	PORTB |= (1 << DB5) | (1 << DB4);
	PORTC &= ~(1 <<E);
	_delay_ms(4.1);
	
	PORTC |= (1 << E);
	PORTC &= ~(1 <<E);
	_delay_us(100);
	
	PORTC |= (1 << E);
	PORTC &= ~(1 <<E);
	_delay_us(100);
	
	//setup 4lines transfer
	PORTC |= (1 << E);
	PORTB &= ~(1 << DB4);
	PORTC &= ~(1 <<E);
	
	//lcd params
	send_command(0b00101000);
	send_command(0b00000110);
	send_command(0b00001100);
	
	
	clear_lcd();
	
}

void clear_lcd()
{
	PORTC &= ~(1 << RS);
	send_byte(1);
	PORTC |= 1 << RS;
	_delay_ms(1.64);
}


void send_command(unsigned char b)
{
	PORTC &= ~(1 << RS);
	send_byte(b);
	PORTC |= 1 << RS;
}

void send_byte(unsigned char b)
{	
	//send upper part
	PORTC |= (1 << E);

	PORTB = ((b >> 4) & 0x0F) | (PORTB & 0xF0);
	PORTC &= ~(1 <<E);
	asm volatile("nop");
		
	//send lower part
	PORTC |= (1 << E);

	PORTB = (b & 0x0F) | (PORTB & 0xF0);
	PORTC &= ~(1 <<E);
	_delay_us(40);
}

void write_string(char* text)
{
	char *textPointer = text;	
	while(*textPointer != '\0')
	{
		char t = *textPointer;
		send_byte(t);
		textPointer++;
	}
}

void write_characters(char* text, int length)
{
	for(int i=0; i<length;i++)
	{
		send_byte(text[i]);
	}
}