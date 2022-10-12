/*
 Name:		MiniBTS.ino
 Created:	11.10.2022 21:15:53
 Author:	a.jurcenko
 Email:		a.jurcenko@inproduct.de
*/
enum EState {
	INIT,
	ERROR,
	PRERUN,
	CLOSE_VALVE,
	MISCHER,
	ZUFUHR,
	RUEHREN,
	ABFUHR,
	OPEN_VALVE,
	WAIT_TILL_EMPTY,
	SHUTDOWN_MISCHER,
	SHUTDOWN_ABFUHR,
	POSTRUN,
	PAUSE,
	HAND,
};

