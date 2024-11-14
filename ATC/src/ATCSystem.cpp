#include "ATCSystem.h"

/*this is the Computer System
 Responsible for:
	- Does periodic computations to determine if there is or will be a violation of airspace contraints.
	- Sends alerts/notifications of violation to the operator.
	- Sends to the Data Display to show on the screen of the controller:
		- ID of aircraft
		- Position of aircraft
	- Stores info into log file every 30 seconds
*/

//ATCSystem::ATCSystem(Display iDisplay, Radar iRadar):
//	mDisplay(iDisplay), mRadar(iRadar)
//{
//
//}

// While loop to continously run the system
void ATCSystem::run()
{
	// We should log all the operator's requests in a seperate log file
}

// Checks for aircraft violations
void ATCSystem::checkViolations()
{
	// The constraint to check is if 2 aircrafts are less than 1000 feet apart in hight or
	// 3000 feet apart in width
	// This should be called every n seconds, depending on user input
	// If there is a violation that could happen within the next 3 minutes we should display the alarm in Display.cpp
}

// Gives a log statement of the airspace
void ATCSystem::logState()
{
	// Should save the state of the air space every 30 seconds in a log file
}
