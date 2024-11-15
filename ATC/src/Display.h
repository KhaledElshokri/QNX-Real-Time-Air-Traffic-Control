#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <iostream>
#include <vector>
#include "Aircraft.h"

/* Responsible for:
	- Shows incoming collisions
	- Simple 2d grid
*/

class Display {
public:

	// Renders Aircraft positions from the list
	// TODO: use openGl to render the aircrafts positions
    void renderGrid(std::vector<Aircraft> givenAircraftData);

    // Display Violations
    void notifyAlert(int id);

    void* start();
    void* startRadarListener();
    void* startViolationListener();

    static void* startThread(void* context);
    static void* startRadarListenerThread(void* context);
    static void* startViolationListenerThread(void* context);
};




#endif /* DISPLAY_H_ */
