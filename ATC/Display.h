/*
 * Display.h
 *
 *  Created on: Nov. 4, 2024
 *      Author: Khale
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <iostream>
#include <vector>
#include "Aircraft.h"

class Display {
public:

	// Renders Aircraft positions from the list
	// TODO: use openGl to render the aircrafts positions
    void render(const std::vector<Aircraft>& aircraftList);

    // Display Violations
    void notifyAlert(int id);
};




#endif /* DISPLAY_H_ */
