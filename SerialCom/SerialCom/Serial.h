/*
 * Serial.h
 *
 * Created: 2014-08-17 13:19:04
 *  Author: muzieh
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_

#include "Settings.h"
#define MYUBRR (F_CPU / 16 / BaudRate ) - 1

int SerialInit();



#endif /* SERIAL_H_ */