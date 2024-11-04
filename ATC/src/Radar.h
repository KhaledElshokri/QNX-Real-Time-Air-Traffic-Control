#ifndef RADAR_H_
#define RADAR_H_


#include "Aircraft.h"
#include <vector>

class Radar {
private:
    std::vector<Aircraft> mAircraftList;

public:

    // updates the aircrafts positions in the list
    void updatePositions();

    // Controls aircrafts and triggers response from aircraft by id
    void requestPosition(int iId);
};


#endif /* RADAR_H_ */