
#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <iostream>

class Aircraft {
private:
    int id;
    float x, y, z;     // Position coordinates
    float speedX, speedY, speedZ; // Speed coordinates

public:

    // Aircraft constructor
    Aircraft(int id, float x, float y, float z, float speedX, float speedY, float speedZ);

    // Getter for ID
    int getID() const { return id; }

    // updates aircraft positions based on their speed
    void updatePosition();

    // Outputs a string to the radar
    void respondToRadar();
};

#endif /* AIRCRAFT_H_ */
