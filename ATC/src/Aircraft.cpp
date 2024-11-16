#include "Aircraft.h"
#include <mutex>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sys/dispatch.h>
#include "CommunicationSystem.h"

/*
 * Each aircraft has a thread, which runs start();
 */

extern std::mutex coutMutex;

typedef struct {
	int flightId;
	float X, Y, Z;
	float mSpeedX, mSpeedY, mSpeedZ;
	CommunicationSystem commSystem;
} aircraft_msg;

typedef struct {
	bool received;
	int aircraftID;
	float xSpeed;
	float ySpeed;
	float zSpeed;
} changespeed_cmd;

// Aircraft constructor
Aircraft::Aircraft(int iEntryTime , int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ, CommunicationSystem iCommSystem) :
	mEntryTime(iEntryTime), mId(iId), mX(iX), mY(iY), mZ(iZ), mSpeedX(iSpeedX), mSpeedY(iSpeedY), mSpeedZ(iSpeedZ), commSystem(iCommSystem)
{
	// Creates each aircraft object from an input text file
}

Aircraft::Aircraft(int iId, float iX, float iY, float iZ, float iSpeedX, float iSpeedY, float iSpeedZ, CommunicationSystem iCommSystem) :
	mId(iId), mX(iX), mY(iY), mZ(iZ), mSpeedX(iSpeedX), mSpeedY(iSpeedY), mSpeedZ(iSpeedZ), commSystem(iCommSystem)
{
	//constructor for radar to use
}
void Aircraft::coutDebug(){
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "Debug: Info from Aircraft ID: " << mId << std::endl;
		std::cout << "X: " << mX << std::endl;
		std::cout << "Y: " << mY << std::endl;
		std::cout << "Z: " << mZ << std::endl;
		std::cout << "X Speed: " << mSpeedX << std::endl;
		std::cout << "Y Speed: " << mSpeedY << std::endl;
		std::cout << "Z Speed: " << mSpeedZ << std::endl;
	}
}


// updates aircraft positions based on their speed
void Aircraft::updatePosition(union sigval sv)
{
	Aircraft* aircraft = static_cast<Aircraft*>(sv.sival_ptr);

	aircraft->mX += aircraft->mSpeedX;
	aircraft->mY += aircraft->mSpeedY;
	aircraft->mZ += aircraft->mSpeedZ;

	// we need to make a mutex lock on the cout, otherwise race condition
//	std::lock_guard<std::mutex> guard(coutMutex);
//	std::cout << "Plane mX, mY, mZ: " << aircraft->mX << ", " << aircraft->mY << ", " << aircraft->mZ << std::endl;

}

void Aircraft::respondToRadar(union sigval sv){
	Aircraft* aircraft = static_cast<Aircraft*>(sv.sival_ptr);

	std::string channelName = "aircraft_" + std::to_string(aircraft->getId());
	name_attach_t *attach = name_attach(NULL, channelName.c_str(), 0);
	if (attach == NULL) {
		perror("name_attach");
	}

	int rcvid;
	aircraft_msg msg;

	while(true){
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (rcvid == -1) {
			perror("MsgReceive");
		}

		aircraft_msg reply;
		reply.flightId = aircraft->getId();

		int status = MsgReply(rcvid, 0, &reply, sizeof(reply));
	}
}

// Main thread for aircraft
// 		- creates and starts timer to update position
//		- listens for radar requests
void* Aircraft::start()
{
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "Aircraft: Aircraft Thread started" << std::endl << std::flush;
	}

	// Create timer to update plane location.
	timer_t plane_timer_id;
	timer_t plane_msg_timer_id;
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
		std::cerr << "Error setting timer for Aircraft: " << strerror(errno) << std::endl;
	}

	//start thread to listen from communicaiton system
	pthread_t commSystemListenerThread;
	pthread_create(&commSystemListenerThread, NULL, &Aircraft::startCommListenerThread, this);


	// listen for radar requests
	std::string channelName = "aircraft_" + std::to_string(this->getId());
	name_attach_t *attach = name_attach(NULL, channelName.c_str(), 0);
	if (attach == NULL) {
		perror("name_attach");
	}

	int rcvid;
	aircraft_msg msg;

		//might want to move this to its own thread, instead of blocking
		//the "creator" thread at the end to receive messages
	while(true){
		// waits here until a message is received
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (rcvid == -1) {
			perror("MsgReceive");
		}

		aircraft_msg reply;
		reply.flightId = this->getId();
		reply.X = this->getXPos();
		reply.Y = this->getYPos();
		reply.Z = this->getZPos();
		reply.mSpeedX = this->getXSpeed();
		reply.mSpeedY = this->getYSpeed();
		reply.mSpeedZ = this->getZSpeed();
		reply.commSystem = this->getCommSystem();

		int status = MsgReply(rcvid, 0, &reply, sizeof(reply));
	}

	return nullptr;
}

void* Aircraft::startThread(void* context)
{
	return static_cast<Aircraft*>(context)->start();
}

// Thread which listens for "changespeed ID x y z" command
void* Aircraft::startCommListener(){

	//listen for message
	std::string channelName = "aircraft_commsys_" + std::to_string(this->getId());
	name_attach_t *attach = name_attach(NULL, channelName.c_str(), 0);
	if (attach == NULL) {
		perror("name_attach");
	}

	int rcvid;
	changespeed_cmd msg;

	while(true){
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if(rcvid == -1){
			perror("MsgReceive");
		}


		{
			std::lock_guard<std::mutex> guard(coutMutex);
			std::cout << "Aircraft: Received request to change speed. " << std::endl;
		}

		this->setSpeed(msg.xSpeed, msg.ySpeed, msg.zSpeed);

		MsgReply(rcvid, EOK, NULL, 0);
	}

}

void* Aircraft::startCommListenerThread(void* context){
	return static_cast<Aircraft*>(context)->startCommListener();
}
