#include "Radar.h"

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


// updates the aircrafts positions in the list
void Radar::updatePositions()
{
	// Radar is a process so we have to use a shared memory or message passing
//	for (auto& wAircraft : mAircraftList)
//	{
//		wAircraft.updatePosition(); // This should get called every 1 second
//	}
}

// Controls aircrafts and triggers response from aircraft by id
void Radar::requestPosition(int iId)
{
	// Requesting info from a specific aircraft(id) which is a thread
}



