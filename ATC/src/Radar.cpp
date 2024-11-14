#include "Radar.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <mutex>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sys/dispatch.h>
#include "Aircraft.h"


extern std::mutex coutMutex;

typedef struct {
	int flightId;
	float X, Y, Z;
	float mSpeedX, mSpeedY, mSpeedZ;
} aircraft_msg;


/* Responsible for:
	PSR:
		- Is a circular radar which sends signals around, and catches them if they reflect,  putting them in a list.
	SSR:
	- Emits interrogation signals to the PSRs list of planes.
	- Transponders on the aircraft receive it, and return it. Their details are shown on the screen:
		- Flight ID.
		- Aircraft flight level.
		- Aircrafts speed. X Y Z
		- Aircrafts position. X Y Z
 */

Radar::Radar(std::vector<Aircraft> aircraftList) : initialAircraftList(aircraftList) {
};

// updates the aircrafts positions in the list
void Radar::updatePositions()
{
	// Radar is a process so we have to use a shared memory or message passing
//	for (auto& wAircraft : mAircraftList)
//	{
//		wAircraft.updatePosition(); // This should get called every 1 second
//	}
}

std::vector<Aircraft> Radar::runRadar() {
    /* I say we skip the PSR part, because:
     * 	We need to know each aircrafts location, but eachs scope is only within its
     * 	respective thread. This means we would have to ask each thread for its info,
     * 	then sort into a "closest to the middle of the plane", just to go and send
     * 	another request in SSR later. PSR isnt as important, simulating it isnt something
     * 	we'd need to do in a real ATC system. It would be a seperate system that just
     * 	feeds the ATCSystem information from its radar.
     * 							- Kyle
     */

    std::vector<Aircraft> radarFindings;

    //go through all of the aircrafts, and send a request for their information
    for (size_t i = 0; i < initialAircraftList.size(); i++) {
    	std::string channelName = "aircraft_" + std::to_string(initialAircraftList[i].getId());
    	int coid = name_open(channelName.c_str(), 0);
		if (coid == -1) {
			perror("name_open");
		}

		aircraft_msg msg;
		msg.flightId = -1;
		aircraft_msg reply;
		reply.flightId = -1;
		int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
		if(status == -1) {
			perror("MsgSend");
		}

		//if aircraft didnt reply
		if(reply.flightId == -1){
			perror("No reply from aircraft");
		}

		Aircraft aircraft(reply.flightId, reply.X, reply.Y, reply.Z, reply.mSpeedX, reply.mSpeedY, reply.mSpeedZ);
		radarFindings.push_back(aircraft);


		name_close(coid);
    }
    return radarFindings;
}


