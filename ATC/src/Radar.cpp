#include "Radar.h"


// updates the aircrafts positions in the list
void Radar::updatePositions()
{
	for (auto& wAircraft : mAircraftList)
	{
		wAircraft.updatePosition();
	}
}

// Controls aircrafts and triggers response from aircraft by id
void Radar::requestPosition(int iId)
{

}



