#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <iostream>

class Aircraft {
private:
	int mEntryTime;
    int mId;
    float mX, mY, mZ;     // Position coordinates
    float mSpeedX, mSpeedY, mSpeedZ; // Speed coordinates

public:

    // Aircraft constructor
    Aircraft(int iEntryTime, int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ);

    // Getter for ID
    int getID() const { return mId; }

    // updates aircraft positions based on their speed
    void updatePosition();

    // Outputs a string to the radar
    void respondToRadar();
};

#endif /* AIRCRAFT_H_ */
