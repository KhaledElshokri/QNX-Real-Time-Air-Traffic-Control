#include "Aircraft.h"
#include <mutex>

// Using message based communications

extern std::mutex coutMutex;

// Aircraft constructor
Aircraft::Aircraft(int iEntryTime , int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ) :
	mEntryTime(iEntryTime), mId(iId), mX(iX), mY(iY), mZ(iZ), mSpeedX(iSpeedX), mSpeedY(iSpeedY), mSpeedZ(iSpeedZ)
{
	// Creates each aircraft object from an input text file
}

// updates aircraft positions based on their speed
void Aircraft::updatePosition(union sigval sv)
{
	Aircraft* aircraft = static_cast<Aircraft*>(sv.sival_ptr);
	aircraft->mX += aircraft->mSpeedX;
	aircraft->mY += aircraft->mSpeedY;
	aircraft->mZ += aircraft->mSpeedZ;

	// we need to make a mutex lock on the cout, otherwise threads will
//	std::lock_guard<std::mutex> guard(coutMutex);
//	std::cout << "Plane mX, mY, mZ: " << aircraft->mX << ", " << aircraft->mY << ", " << aircraft->mZ << std::endl;

}

// Outputs a string to the radar
void Aircraft::respondToRadar()
{
	// returns its id, location and speed to the Radar
}

void* Aircraft::start()
{
	std::cout << "Thread started" << std::endl << std::flush;
	//PUT ACTUAL BEHAVIOUR OF PLANES

	/*needs to:
		- listen for message requests from radar
		- update its positions
	 */

	//TODO: Open communication channel for the plane, to receive requests from operator

	// Create timer to update plane location.
	timer_t plane_timer_id;
	struct sigevent sev;
	struct itimerspec its;

	sev.sigev_notify = SIGEV_THREAD; //Notify via thread
	sev.sigev_notify_function = Aircraft::updatePosition;
	sev.sigev_value.sival_ptr = this; // Pass this Aircraft instance
	sev.sigev_notify_attributes = nullptr; // Default thread attributes


	if(timer_create(CLOCK_REALTIME, &sev, &plane_timer_id) == -1){
		std::cerr << "Error creating timer for Aircraft: " << strerror(errno) << std::endl;
	}

	// Set time timer to expire after 1 second, and then repeat every 1 second.
	its.it_value.tv_sec = 1;  // Initial expiration after 1 seconds
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 1; // Repeat every 1 second
	its.it_interval.tv_nsec = 0;

	if(timer_settime(plane_timer_id, 0, &its, nullptr) == -1){
		std::cerr << "Error setting timer: " << strerror(errno) << std::endl;
	}


	return nullptr;
}

//routine started by the thread, actual activity defined in Plane::run()
//it returns a void* because its a generic pointer, and matches the type needed by pthread_start(...)
void* Aircraft::startThread(void* context)
{
	return static_cast<Aircraft*>(context)->start();
}
