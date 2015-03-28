#include "defs.h"
#include "serial.h"


// GETROBOTEQCONFIRM FUNCTION
int getRoboteqConfirm() {	
	roboteqResponseTime = TARGET_ASSOC_LIMIT;

	while(!rxWireFlag) {
		if (!roboteqResponseTime) {
			wireSendString("Command not acknowledged!\r\n");
			return 0;
		}
	}

	rxWireFlag = 0;
	if (wireReceived == ROBOTEQ_CONFIRM) {
		wireSendString("RoboteQ initialized!\r\n");
		roboteqStatus = 1;
		PORTC |= TARGET_LED;
		return 1;
	}
	else {
		wireSendString("Bad response!\r\n");
		return 0;
	}

}


// ROBOTEQINIT FUNCTION
// Queries Roboteq for model number and initializes hardware
// Returns 1 if successful, 0 otherwise
int roboteqInit() {
	int response;

	PORTC &= ~TARGET_LED;
	roboteqFlag = 0;
	wireSendString("Requesting RoboteQ Model #...\r\n");
	roboteqResponseTime = TARGET_ASSOC_LIMIT;

	response = wireGetCmpString(&roboteqResponseTime, ROBOTEQ_MODEL);
	if (response == 1) {
		wireSendString("Success!\r\n");
	}
	else if (response == -1) {
		wireSendString("Failure!\r\n");
		return 0;
	}
	else {
		wireSendString("Time out!\r\n");
		return 0;
	}

	rxWireFlag = 0;
	wireSendString("Turning off verbose output...\r\n");

	return getRoboteqConfirm();
}


// DATATOROBOTEQ FUNCTION
// Forms strings from sensor data and transmits to Roboteq
// Returns 1 if data acknowledged, 0 otherwise
int dataToRoboteq() {
	wireSendString("hello roboteq\r\n");
	return getRoboteqConfirm();
}