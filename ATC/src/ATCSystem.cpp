/*this is the Computer System
 Responsible for:
	- Does periodic computations to determine if there is or will be a violation of airspace contraints.
	- Sends alerts/notifications of violation to the operator.
	- Sends to the Data Display to show on the screen of the controller:
		- ID of aircraft
		- Position of aircraft
	- Stores info into log file every 30 seconds
*/

#include "ATCSystem.h"
#include <mutex>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sys/dispatch.h>

extern std::mutex coutMutex;

typedef struct {
	std::vector<Aircraft> aircraftData;
	bool received;
} radar_msg;

typedef struct{
	int aircraft1ID;
	int aircraft2ID;
	bool received;
} violation_msg;

ATCSystem::ATCSystem(Radar iRadar, Display iDisplay, CommunicationSystem iCommSystem):
		radar(iRadar), display(iDisplay), commSystem(iCommSystem)
{

}



// Checks for aircraft violations
	// If there are violations, it will alert the Operator and DataDisplay
void ATCSystem::checkViolations(std::vector<Aircraft>* radarOutput)
{
	// The constraint to check is if 2 aircrafts are less than 1000 feet apart in hight or
	// 3000 feet apart in width
	// This should be called every n seconds, depending on user input
	// If there is a violation that could happen within the next 3 minutes we should display
	// the alarm in Display.cpp

	std::vector<Aircraft>& radarFindings = *radarOutput; //deref the pointer

	int VERTICAL_CONSTRAINT = 1000;
	int HORIZONTAL_CONSTRAINT = 3000;

	// !! Making a copy incase the operator changes it while it runs.
	//    Might need to put a mutex on it since its shared
	int currentPredictionTimeSeconds = predictionTimeSeconds;

	//check each aircraft against each other aircraft
	for (size_t i = 0; i < radarFindings.size(); i++) {
		for (size_t j = i + 1; j < radarFindings.size(); j++) {
			Aircraft aircraft1Projection = radarFindings[i];
			Aircraft aircraft2Projection = radarFindings[j];

			//calculate airspace needed around aircraft 1
			int x1Min = aircraft1Projection.getXPos() - HORIZONTAL_CONSTRAINT;
			int x1Max = aircraft1Projection.getXPos() + HORIZONTAL_CONSTRAINT;
			int y1Min = aircraft1Projection.getYPos() - HORIZONTAL_CONSTRAINT;
			int y1Max = aircraft1Projection.getYPos() + HORIZONTAL_CONSTRAINT;
			int z1Min = aircraft1Projection.getZPos() - VERTICAL_CONSTRAINT;
			int z1Max = aircraft1Projection.getZPos() + VERTICAL_CONSTRAINT;

			//calculate airspace needed around aircraft 2
			int x2Min = aircraft2Projection.getXPos() - HORIZONTAL_CONSTRAINT;
			int x2Max = aircraft2Projection.getXPos() + HORIZONTAL_CONSTRAINT;
			int y2Min = aircraft2Projection.getYPos() - HORIZONTAL_CONSTRAINT;
			int y2Max = aircraft2Projection.getYPos() + HORIZONTAL_CONSTRAINT;
			int z2Min = aircraft2Projection.getZPos() - VERTICAL_CONSTRAINT;
			int z2Max = aircraft2Projection.getZPos() + VERTICAL_CONSTRAINT;

			//i know this is disgusting
			// predict N seconds ahead of time for aircraft 1
			aircraft1Projection.setPos(
					(radarFindings[i].getXPos() + (radarFindings[i].getXSpeed() * currentPredictionTimeSeconds)),
					(radarFindings[i].getYPos() + (radarFindings[i].getYSpeed() * currentPredictionTimeSeconds)),
					(radarFindings[i].getZPos() + (radarFindings[i].getZSpeed() * currentPredictionTimeSeconds))
			);

			// predict N seconds ahead of time for aircraft 1
			aircraft2Projection.setPos(
					(radarFindings[j].getXPos() + (radarFindings[j].getXSpeed() * currentPredictionTimeSeconds)),
					(radarFindings[j].getYPos() + (radarFindings[j].getYSpeed() * currentPredictionTimeSeconds)),
					(radarFindings[j].getZPos() + (radarFindings[j].getZSpeed() * currentPredictionTimeSeconds))
			);

			//if two boxes overlap
			if ((x1Max >= x2Min && x2Max >= x1Min)
					&& (y1Max >= y2Min && y2Max >= y1Min)
					&& (z1Max >= z2Min && z2Max >= z1Min)) {
				//VIOLATION DETECTED

				//Send violation info the Display
				std::string channelName = "atc_to_display_violations";
				int coid = name_open(channelName.c_str(), 0);
				if(coid == -1){
					perror("name_open");
				}

				violation_msg msg;
				msg.received = false;
				msg.aircraft1ID = radarFindings[i].getID();
				msg.aircraft2ID = radarFindings[j].getID();
				violation_msg reply;
				reply.received = false;
				int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
				if(status == -1){
					perror("MsgSend");
				}

				//if no reply
				if(reply.received == false){
					perror("No reply from display");
				}

				// TODO: Send violation to operator
			}

		}
	}
}

// Gives a log statement of the airspace
void ATCSystem::logState()
{
	// Should save the state of the air space every 30 seconds in a log file
}

void ATCSystem::monitorAirspace(union sigval sv){
	ATCSystem* ATCSys = static_cast<ATCSystem*>(sv.sival_ptr);

	// get info of all flights from the radar
	std::vector<Aircraft> radarFindings = ATCSys->radar.runRadar();

	// check for airspace violations
	ATCSys->checkViolations(&radarFindings);

	// send radar data to the display TODO NEED TO SEND ONLY EVERY 5 SECONDS
	std::string channelName = "radar_to_display";
	int coid = name_open(channelName.c_str(), 0);
	if (coid == -1) {
		perror("name_open");
	}

	radar_msg msg;
	msg.received = false;
	msg.aircraftData = radarFindings;
	radar_msg reply;
	msg.received = false;
	reply.aircraftData = radarFindings;
	int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
	if(status == -1){
		perror("MsgSend");
	}

	if(reply.received == false){
		perror("No reply from Display");
	}

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
		//sending data to DataDisplay (1 second)
		//logging info to text file (30 seconds)



	timer_t collisionCheck_timer_id;
	struct sigevent sevC;
	struct itimerspec itsC;

	sevC.sigev_notify = SIGEV_THREAD;
	sevC.sigev_notify_function = ATCSystem::monitorAirspace;
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
