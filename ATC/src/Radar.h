#ifndef RADAR_H_
#define RADAR_H_

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

/*
 * PSR: To simulate the radar, we have a knownAircraftList, which we sort by nearness to the center of
 * the zone.
 * SSR: With this new sorted list, we send a request to each aircraft, and store their responses in
 * radar findings, and return this radarFindings list.
 */

#include "Aircraft.h"
#include <vector>

class Radar {
private:
	std::vector<Aircraft> initialAircraftList;

public:
    Radar(std::vector<Aircraft> aircraftList);

    int getSizeOfInitialList(){
    	int n = 0;
    	n = initialAircraftList.size();
    	return n;
    }

    // Controls aircrafts and triggers response from aircraft by id
    void requestPosition(int id);

    std::vector<Aircraft> runRadar();

    void* startListener();

    static void* startListenerThread(void* context);
};


#endif /* RADAR_H_ */
