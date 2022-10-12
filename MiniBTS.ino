/*
 Name:		MiniBTS.ino
 Created:	11.10.2022 21:15:53
 Author:	a.jurcenko
 Email:		a.jurcenko@inproduct.de
*/

#include "Arduino.h"
#include <Controllino.h>
#include "EState.h"

//TIMER´S
unsigned long next_time = 0;
int mischer_prerun = 15000;
int zufuhr_prerun = 15000;
unsigned long ruehren = 50000;
int abfuhr_prerun = 12000;
int till_empty = 15000;

//INPUT´S
int valve_opened_pin = CONTROLLINO_A0;
int valve_closed_pin = CONTROLLINO_A1;
int start_signal_pin = CONTROLLINO_A2;

int motor_1_error_pin = CONTROLLINO_A3;
int motor_2_error_pin = CONTROLLINO_A4;
int emergency_stop_pin = CONTROLLINO_A5;

//OUTPUT´S
int valve_open_pin = CONTROLLINO_RELAY_00;
int valve_close_pin = CONTROLLINO_RELAY_01;
int mischer_pin = CONTROLLINO_RELAY_02;
int zufuhr_pin = CONTROLLINO_RELAY_03;
int abfuhr_pin = CONTROLLINO_RELAY_04;
int safety_pin = CONTROLLINO_RELAY_05;

int warnlamp_pin = CONTROLLINO_D0;
//SIGNAL LAMP´S
int sl_emergency_stop = CONTROLLINO_D1;
int sl_error_lamp = CONTROLLINO_D2;
int sl_valve = CONTROLLINO_D3;
int sl_mischer = CONTROLLINO_D4;
int sl_zufuhr = CONTROLLINO_D5;
int sl_abfuhr = CONTROLLINO_D6;


EState state = INIT;



void setup() {

	Serial.begin(9600);

	pinMode(valve_opened_pin, INPUT);
	pinMode(valve_closed_pin, INPUT);
	pinMode(start_signal_pin, INPUT);
	pinMode(motor_1_error_pin, INPUT);
	pinMode(motor_2_error_pin, INPUT);
	pinMode(emergency_stop_pin, INPUT);

	pinMode(valve_open_pin, OUTPUT);
	pinMode(valve_close_pin, OUTPUT);
	pinMode(mischer_pin, OUTPUT);
	pinMode(zufuhr_pin, OUTPUT);
	pinMode(abfuhr_pin, OUTPUT);
	pinMode(safety_pin, OUTPUT);

	pinMode(warnlamp_pin, OUTPUT);
	pinMode(sl_emergency_stop, OUTPUT);
	pinMode(sl_error_lamp, OUTPUT);
	pinMode(sl_valve, OUTPUT);
	pinMode(sl_mischer, OUTPUT);
	pinMode(sl_zufuhr, OUTPUT);
	pinMode(sl_abfuhr, OUTPUT);

	state = INIT;
}



void loop() {
	//error handling
	if (!digitalRead(emergency_stop_pin)) {
		digitalWrite(safety_pin, LOW);
		digitalWrite(sl_emergency_stop, HIGH);
		state = ERROR;
		Serial.println("Emergency Stop");
	}
	else if (!digitalRead(motor_1_error_pin) || !digitalRead(motor_2_error_pin)) {
		digitalWrite(sl_error_lamp, HIGH);
		state = ERROR;
		Serial.println("Error Motor");
	}
	else if ((!digitalRead(valve_closed_pin) && !digitalRead(valve_opened_pin)) && (state != OPEN_VALVE && state != CLOSE_VALVE)) {
		digitalWrite(sl_error_lamp, HIGH);
		state = ERROR;
		Serial.println("Error Valve");
	}
	else {
		digitalWrite(safety_pin, HIGH);
		digitalWrite(sl_emergency_stop, LOW);
		digitalWrite(sl_error_lamp, LOW);
		if (state == ERROR) {
			state = INIT;
		}
	}
	switch (state)
	{
	case INIT:
		next_time = 0;
		state = PAUSE;
		Serial.println("INIT");
		break;
	case ERROR:
		digitalWrite(valve_open_pin, LOW);
		digitalWrite(valve_close_pin, LOW);
		digitalWrite(mischer_pin, LOW);
		digitalWrite(zufuhr_pin, LOW);
		digitalWrite(abfuhr_pin, LOW);

		digitalWrite(sl_mischer, LOW);
		digitalWrite(sl_valve, LOW);
		digitalWrite(sl_zufuhr, LOW);
		digitalWrite(sl_abfuhr, LOW);
		digitalWrite(warnlamp_pin, LOW);
		break;
	case PRERUN:
		Serial.println("PRERUN");
		digitalWrite(warnlamp_pin, HIGH);
		state = CLOSE_VALVE;
		break;
	case CLOSE_VALVE:
		Serial.println("CLOSE VALVE");
		if (!digitalRead(valve_closed_pin)) {
			digitalWrite(valve_close_pin, LOW);
			digitalWrite(sl_valve, LOW);
			state = MISCHER;
		}
		else {
			digitalWrite(valve_close_pin, HIGH);
			digitalWrite(sl_valve, HIGH);
		}
		break;
	case MISCHER:
		Serial.println("MISCHER");
		digitalWrite(mischer_pin, HIGH);
		digitalWrite(sl_mischer, HIGH);
		if (next_time == 0) {
			next_time = millis() + mischer_prerun;
		}
		else if (millis() > next_time) {
			state = ZUFUHR;
			next_time = 0;
		}
		break;
	case ZUFUHR:
		Serial.println("ZUFUHR");
		digitalWrite(zufuhr_pin, HIGH);
		digitalWrite(sl_zufuhr, HIGH);
		if (next_time == 0) {
			next_time = millis() + zufuhr_prerun;
		}
		else if (millis() > next_time) {
			state = RUEHREN;
			next_time = 0;
		}
		
		break;
	case RUEHREN:
		Serial.println("RUEHREN");
		digitalWrite(zufuhr_pin, LOW);
		digitalWrite(sl_zufuhr, LOW);
		if (next_time == 0) {
			next_time = millis() + ruehren;
		}
		else if (millis() > next_time) {
			state = ABFUHR;
			next_time = 0;
		}
		break;
	case ABFUHR:
		Serial.println("ABFUHR");
		digitalWrite(abfuhr_pin, HIGH);
		digitalWrite(sl_abfuhr, HIGH);
		if (next_time == 0) {
			next_time = millis() + abfuhr_prerun;
		}
		else if (millis() > next_time) {
			state = OPEN_VALVE;
			next_time = 0;
		}
		break;
	case OPEN_VALVE:
		Serial.println("OPEN VALVE");
		if (!digitalRead(valve_opened_pin)) {
			digitalWrite(valve_open_pin, LOW);
			digitalWrite(valve_close_pin, LOW);
			digitalWrite(sl_valve, LOW);
			state = WAIT_TILL_EMPTY;
		}
		else {
			digitalWrite(valve_open_pin, HIGH);
			digitalWrite(valve_close_pin, HIGH);
			digitalWrite(sl_valve, HIGH);
		}
		break;
	case WAIT_TILL_EMPTY:
		Serial.println("TILL EMPTY");
		if (next_time == 0) {
			next_time = millis() + till_empty;
		}
		else if (millis() > next_time) {
			state = SHUTDOWN_MISCHER;
			next_time = 0;
		}
		break;
	case SHUTDOWN_MISCHER:
		Serial.println("SHUTDOWN MISCHER");
		digitalWrite(mischer_pin, LOW);
		digitalWrite(sl_mischer, LOW);
		state = SHUTDOWN_ABFUHR;
		break;
	case SHUTDOWN_ABFUHR:
		Serial.println("SHUTDOWN ABFUHR");
		digitalWrite(abfuhr_pin, LOW);
		digitalWrite(sl_abfuhr, LOW);
		state = POSTRUN;
		break;
	case POSTRUN:
		Serial.println("POSTRUN");
		digitalWrite(warnlamp_pin, LOW);
		state = PAUSE;
		break;
	case PAUSE:
		Serial.println("PAUSE");
		if (digitalRead(start_signal_pin)) {
			Serial.println("START SIGNAL");
			state = PRERUN;
		}
		break;
	case HAND:
		break;
	default:
		break;
	}
}
