#include "Aircraft.h"

// Aircraft constructor
Aircraft::Aircraft(int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ) :
	mId(iId), mX(iX), mY(iY), mZ(iZ), mSpeedX(iSpeedX), mSpeedY(iSpeedY), mSpeedZ(iSpeedZ)
{

}

// updates aircraft positions based on their speed
void Aircraft::updatePosition()
{
    mX += mSpeedX;
	mY += mSpeedY;
	mZ += mSpeedZ;
}

// Outputs a string to the radar
void respondToRadar()
{

}
