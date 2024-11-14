#include "ATCSystem.h"
#include <mutex>
#include <vector>


extern std::mutex coutMutex;

/*this is the Computer System
 Responsible for:
	- Does periodic computations to determine if there is or will be a violation of airspace contraints.
	- Sends alerts/notifications of violation to the operator.
	- Sends to the Data Display to show on the screen of the controller:
		- ID of aircraft
		- Position of aircraft
	- Stores info into log file every 30 seconds
*/

ATCSystem::ATCSystem(Radar iRadar, Display iDisplay, CommunicationSystem iCommSystem):
		radar(iRadar), display(iDisplay), commSystem(iCommSystem)
{

}



// Checks for aircraft violations
void ATCSystem::checkViolations()
{
	// The constraint to check is if 2 aircrafts are less than 1000 feet apart in hight or
	// 3000 feet apart in width
	// This should be called every n seconds, depending on user input
	// If there is a violation that could happen within the next 3 minutes we should display
	// the alarm in Display.cpp
}

// Gives a log statement of the airspace
void ATCSystem::logState()
{
	// Should save the state of the air space every 30 seconds in a log file
}

void ATCSystem::checkAirspaceViolations(union sigval sv){
	ATCSystem* ATCSys = static_cast<ATCSystem*>(sv.sival_ptr);

	// Checks for collisions by using the radar to get plane locations, then calculating
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "ATC System checking collisions" << std::endl;
	}

	// get info of all flights from the radar
	std::vector<Aircraft> radarFindings = ATCSys->radar.runRadar();
}


void* ATCSystem::start() {
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "ATC System started" << std::endl;
	}

	//start timers for
		//collision check (1 second)
			//PSR, SSR, check
				// NO MORE PSR, NOT WORTH THE HEADACHE
		//logging info to text file (30 seconds)



	timer_t collisionCheck_timer_id;
	struct sigevent sevC;
	struct itimerspec itsC;

	sevC.sigev_notify = SIGEV_THREAD;
	sevC.sigev_notify_function = ATCSystem::checkAirspaceViolations;
	sevC.sigev_value.sival_ptr = this; // Pass this ATCSystem instance
	sevC.sigev_notify_attributes = nullptr;

	if(timer_create(CLOCK_REALTIME, &sevC, &collisionCheck_timer_id) == -1){
		std::cerr << "Error creating timer for ATCSystem Collision Check: " << strerror(errno) << std::endl;
	}

	//repeats every second
	itsC.it_value.tv_sec = 1;
	itsC.it_value.tv_nsec = 0;
	itsC.it_interval.tv_sec = 1;
	itsC.it_interval.tv_nsec = 0;

	if(timer_settime(collisionCheck_timer_id, 0, &itsC, nullptr) == -1){
		std::cerr << "Error setting timer for ATCSystem Collision Check: " << strerror(errno) << std::endl;
	}


	return nullptr;

}

void* ATCSystem::startThread(void* context){
	return static_cast<ATCSystem*>(context)->start();
}
