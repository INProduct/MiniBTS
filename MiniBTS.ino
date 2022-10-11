/*
 Name:		MiniBTS.ino
 Created:	11.10.2022 21:15:53
 Author:	alex
*/

#include "Arduino.h"
#include <Controllino.h>
#include <Estate.h>


//INPUT´S
int valve_opened_pin = CONTROLLINO_D0;
int valve_closed_pin = CONTROLLINO_D1;
int mode_hand_pin = CONTROLLINO_D2;
int start_signal_pin = CONTROLLINO_D3;

//OUTPUT´S
int valve_open_pin = CONTROLLINO_RELAY_00;
int valve_close_pin = CONTROLLINO_RELAY_01;
int mischer_pin = CONTROLLINO_RELAY_02;
int zufuhr_pin = CONTROLLINO_RELAY_03;
int abfuhr_pin = CONTROLLINO_RELAY_04;


EState state = INIT;



void setup() {
	pinMode(valve_open_pin, OUTPUT);
	pinMode(valve_close_pin, OUTPUT);
	pinMode(mischer_pin, OUTPUT);
	pinMode(zufuhr_pin, OUTPUT);
	pinMode(abfuhr_pin, OUTPUT);
	state = ERROR;
}



void loop() {
	switch (state)
	{
	case INIT:
		break;
	case ERROR:
		state = PAUSE;
		break;
	case CLOSE_VALVE:
		if (digitalRead(valve_closed_pin)) {
			digitalWrite(valve_close_pin, LOW);
			state = MISCHER;
		}
		else {
			digitalWrite(valve_close_pin, HIGH);
		}
		break;
	case MISCHER:
		digitalWrite(mischer_pin, HIGH);
		//todo: nach einer Zeit
		state = ZUFUHR;
		break;
	case ZUFUHR:
		digitalWrite(zufuhr_pin, HIGH);
		//todo: nach einer Zeit
		state = RUEHREN;
		break;
	case RUEHREN:
		digitalWrite(zufuhr_pin, LOW);
		//todo: nach einer Zeit
		state = ABFUHR;
		break;
	case ABFUHR:
		digitalWrite(abfuhr_pin, HIGH);
		//todo: nach einer Zeit
		state = OPEN_VALVE;
		break;
	case OPEN_VALVE:
		if (digitalRead(valve_opened_pin)) {
			digitalWrite(valve_open_pin, LOW);
			state = WAIT_TILL_EMPTY;
		}
		else {
			digitalWrite(valve_open_pin, HIGH);
		}
		break;
	case WAIT_TILL_EMPTY:
		break;
	case SHUTDOWN_MISCHER:
		digitalWrite(mischer_pin, LOW);
		//todo: nach einer Zeit
		state = SHUTDOWN_ABFUHR;
		break;
	case SHUTDOWN_ABFUHR:
		digitalWrite(abfuhr_pin, LOW);
		//todo: nach einer Zeit
		state = PAUSE;
		break;
	case PAUSE:
		if (digitalRead(start_signal_pin)) {
			state = CLOSE_VALVE;
		}
		break;
	case HAND:
		break;
	default:
		break;
	}
}
