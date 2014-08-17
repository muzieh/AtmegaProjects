/*
 * Led.h
 *
 * Created: 2014-07-02 21:12:00
 *  Author: muzieh
 */ 


#ifndef LED_H_
#define LED_H_

void initialize_led();
void send_byte(unsigned char byte);
void clear_lcd();
void move(unsigned char row, unsigned char column);
void send_command(unsigned char);
void write_string(char* text);
void write_characters(char* text, int length);



#endif /* LED_H_ */