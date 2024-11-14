#ifndef SRC_OPERATORCONSOLE_H_
#define SRC_OPERATORCONSOLE_H_

/* Responsible for:
	- Lets the controller send commands to the aircraft
	- Command can request the following:
		- Change speed
		- Change altitude
		- Change position
	- Operator can request for information about a aircraft to show in the radar display. It should show:
		- Flight level
		- Speed
		- Position
	To do this, it must sned the request to the computer system, and which then sends the info to the Data Display
 */

class OperatorConsole {
public:
	OperatorConsole();
	virtual ~OperatorConsole();
};

#endif /* SRC_OPERATORCONSOLE_H_ */
