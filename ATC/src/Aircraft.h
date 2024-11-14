#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <iostream>

class Aircraft {
private:
	int mEntryTime;
    int mId;
    float mX, mY, mZ;     // Position coordinates
    float mSpeedX, mSpeedY, mSpeedZ; // Speed coordinates

    // timer trigger method
	static void updatePosition(union sigval sv);
	static void respondToRadar(union sigval sv);

public:
	int getId() const { return mId; }
	int getID() const { return mId; }
	float getXPos() const {return mX;};
	float getYPos() const {return mY;};
	float getZPos() const {return mZ;};
	float getXSpeed() const {return mSpeedX;};
	float getYSpeed() const {return mSpeedY;};
	float getZSpeed() const {return mSpeedZ;};
	void coutDebug();

    // Aircraft constructor
    Aircraft(int iEntryTime, int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ);
    Aircraft(int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ);
    // Getter for ID

    // Outputs a string to the radar
    void respondToRadar();

    //thread routine
    void* start();

    //routine started by the thread, actual activity defined in Plane::start();
    	//it returns a void* because its a generic pointer, and matches the type needed by pthread_start(...)
    static void* startThread(void* context);
};

#endif /* AIRCRAFT_H_ */
