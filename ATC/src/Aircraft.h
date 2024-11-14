#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <iostream>

class Aircraft {
private:
	int mEntryTime;
    int mId;
    float mX, mY, mZ;     // Position coordinates
    float mSpeedX, mSpeedY, mSpeedZ; // Speed coordinates

private:
    // updates aircraft positions based on their speed
    	//must be static for the timer to take it
	static void updatePosition(union sigval sv);

public:

    // Aircraft constructor
    Aircraft(int iEntryTime, int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ);

    // Getter for ID
    int getID() const { return mId; }

    // Outputs a string to the radar
    void respondToRadar();

    //thread routine
    void* start();

    //routine started by the thread, actual activity defined in Plane::run()
    	//it returns a void* because its a generic pointer, and matches the type needed by pthread_start(...)
    static void* startThread(void* context);
};

#endif /* AIRCRAFT_H_ */
