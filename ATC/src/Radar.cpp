#include "Radar.h"


// updates the aircrafts positions in the list
void Radar::updatePositions()
{
	// Radar is a process so we have to use a shared memory or message passing
	for (auto& wAircraft : mAircraftList)
	{
		wAircraft.updatePosition(); // This should get called every 1 second
	}
}

// Controls aircrafts and triggers response from aircraft by id
void Radar::requestPosition(int iId)
{
	// Requesting info from a specific aircraft(id) which is a thread
}



