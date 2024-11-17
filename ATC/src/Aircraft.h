#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <iostream>
#include "CommunicationSystem.h"

class Aircraft {
private:
	int mEntryTime;
    int mId;
    float mX, mY, mZ;     // Position coordinates
    float mSpeedX, mSpeedY, mSpeedZ; // Speed coordinates

    CommunicationSystem commSystem;

    // timer trigger method
	static void updatePosition(union sigval sv);
	static void respondToRadar(union sigval sv);

public:
	int getId() const { return mId; }
	int getID() const { return mId; }
	float getXPos() const { return mX; }
	float getYPos() const { return mY; }
	float getZPos() const { return mZ; }
	float getXSpeed() const { return mSpeedX; }
	float getYSpeed() const { return mSpeedY; }
	float getZSpeed() const { return mSpeedZ; }
	int getEntryTime() const { return mEntryTime; }
	CommunicationSystem getCommSystem() const { return commSystem; }

	void setXPos(float iX) { mX = iX; }
	void setYPos(float iY) { mY = iY; }
	void setZPos(float iZ) { mZ = iZ; }
	void setPos(float iX, float iY, float iZ) { mX = iX; mY = iY; mZ = iZ; }
	void setXSpeed(float iSpeedX) { mSpeedX = iSpeedX; }
	void setYSpeed(float iSpeedY) { mSpeedY = iSpeedY; }
	void setZSpeed(float iSpeedZ) { mSpeedZ = iSpeedZ; }
	void setSpeed(float iSpeedX, float iSpeedY, float iSpeedZ) {
		mSpeedX = iSpeedX; mSpeedY = iSpeedY; mSpeedZ = iSpeedZ; }

	void coutDebug();

    // Aircraft constructor
    Aircraft(int iEntryTime, int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ, CommunicationSystem iCommSystem);
    // Getter for ID

    // Outputs a string to the radar
    void respondToRadar();

    //thread routine
    void* start();
    void* startCommListener();

    //routine started by the thread, actual activity defined in Plane::start();
    	//it returns a void* because its a generic pointer, and matches the type needed by pthread_start(...)
    static void* startThread(void* context);
    static void* startCommListenerThread(void* context);

    /*
     * Note: We need to have both a static startThread and then a start() method. This is because
     * pthread_create expects a static method to start the thread. We then use that static method
     * to start a non-static method, removing its restrictions.
     */
};

#endif /* AIRCRAFT_H_ */
