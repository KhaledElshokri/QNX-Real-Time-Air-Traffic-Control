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
    std::vector<Aircraft> radarData;

    //how far forward we predict collisions
    int predictionTimeSeconds = 180;

public:
    ATCSystem(Radar iRadar, Display iDisplay, CommunicationSystem iCommSystem);

    void setPredTime(int iPredictionTimeSeconds) { predictionTimeSeconds = iPredictionTimeSeconds; }

    void setRadar(Radar iRadar);

    // While loop to continously run the system
    void run();

    // Checks for aircraft violations
    void checkViolations(std::vector<Aircraft>* radarFindings);

    // Gives a log statement of the airspace
    static void logState(union sigval sv);

    static void monitorAirspace(union sigval sv);

    //thread routine
	void* start();
	void* startListener();

	//routine started by the thread
	static void* startThread(void* context);
	static void* startListenerThread(void* context);
};

#endif /* ATCSYSTEM_H_ */
