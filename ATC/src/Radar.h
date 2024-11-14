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

#include "Aircraft.h"
#include <vector>

class Radar {
private:
    std::vector<Aircraft> aircraftList;

public:

    // updates the aircrafts positions in the list
    void updatePositions();

    // Controls aircrafts and triggers response from aircraft by id
    void requestPosition(int id);
};


#endif /* RADAR_H_ */
