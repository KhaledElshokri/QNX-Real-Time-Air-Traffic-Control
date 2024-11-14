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
    void render(const std::vector<Aircraft>& aircraftList);

    // Display Violations
    void notifyAlert(int id);
};




#endif /* DISPLAY_H_ */
