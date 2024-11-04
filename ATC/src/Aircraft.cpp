#include "Aircraft.h"

// Aircraft constructor
Aircraft::Aircraft(int iEntryTime , int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ) :
	mEntryTime(iEntryTime), mId(iId), mX(iX), mY(iY), mZ(iZ), mSpeedX(iSpeedX), mSpeedY(iSpeedY), mSpeedZ(iSpeedZ)
{
	// Creates each aircraft object from an input text file
}

// updates aircraft positions based on their speed
void Aircraft::updatePosition()
{
	// should be called every 1 second to update location and every aircraft is a periodic task
    mX += mSpeedX;
	mY += mSpeedY;
	mZ += mSpeedZ;
}

// Outputs a string to the radar
void respondToRadar()
{
	// returns its id, location and speed to the Radar
}
