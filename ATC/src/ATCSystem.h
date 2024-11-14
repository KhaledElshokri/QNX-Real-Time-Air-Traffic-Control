#ifndef ATCSYSTEM_H_
#define ATCSYSTEM_H_

#include <vector>
#include "Radar.h"
#include "Display.h"
#include "Aircraft.h"
#include "CommunicationSystem.h"

class ATCSystem {
private:
    Radar radar;
    Display display;
    CommunicationSystem commSystem;

public:
    ATCSystem(Radar iRadar, Display iDisplay, CommunicationSystem iCommSystem);

    void setRadar(Radar iRadar);

    // While loop to continously run the system
    void run();

    // Checks for aircraft violations
    void checkViolations();

    // Gives a log statement of the airspace
    void logState();

    static void checkAirspaceViolations(union sigval sv);

    //thread routine
	void* start();

	//routine started by the thread
	static void* startThread(void* context);
};

#endif /* ATCSYSTEM_H_ */
