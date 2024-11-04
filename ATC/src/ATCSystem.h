#ifndef ATCSYSTEM_H_
#define ATCSYSTEM_H_

#include <vector>
#include "Radar.h"
#include "Display.h"
#include "Aircraft.h"

class ATCSystem {
private:
    Radar mRadar;
    Display mDisplay;

public:

    // ATC constructor
    ATCSystem(Display iDisplay, Radar iRadar);

    // While loop to continously run the system
    void run();

    // Checks for aircraft violations
    void checkViolations();

    // Gives a log statement of the airspace
    void logState();
};

#endif /* ATCSYSTEM_H_ */