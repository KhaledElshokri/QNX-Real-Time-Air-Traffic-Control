#include <mutex>
#include <iostream>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sys/dispatch.h>

#include "Aircraft.h"
#include "CommunicationSystem.h"

/* RESPONSIBILITIES
 *	- OperatorConsole triggers the CommunicationSystem::send(R, m) method, which
 *		sends the appropriate command by IPC message to the appropriate component
 *	- Listens for responses from the radar for showaircrafts, and prints them to console.
 *
 * COMMANDS
 * 		1. Show all info about all aircraft
 * 			CMD: showaircrafts
 * 				gives info about all aircraft to the console
 * 		2. Change speed x y z
 * 			CMD: changespeed {id} {x} {y} {z}
 * 				changes the speed of an aircraft
 * 		3. Change prediction time
 * 				CMD: changepred {timeInSeconds}
 */

typedef struct {
	bool received;
	std::vector<Aircraft> aircraftData;
} showaircrafts_cmd;

typedef struct {
	bool received;
	int aircraftID;
	float xSpeed;
	float ySpeed;
	float zSpeed;
} changespeed_cmd;

typedef struct {
	bool received;
	int predTime;
} changepredtime_cmd;

extern std::mutex coutMutex;

const std::string SHOW_AIRCRAFT_CMD = "showaircrafts";
const std::string CHANGE_SPEED_CMD = "changespeed";
const std::string CHANGE_PRED_TIME_CMD = "changepred";

CommunicationSystem::CommunicationSystem() {

}

CommunicationSystem::~CommunicationSystem() {
}

// Project defined method which we need to use. God it causes a huge headache for very little gain.
int CommunicationSystem::send(int R, std::vector<std::string> m)
{
	// None of these actually use the reply.
	// If there is a reply, it will be sent to the CommSys's <cmd_name>Listener thread
	if (m[0] == SHOW_AIRCRAFT_CMD) {
		// Send command
		std::string channelName = "commsys_to_radar";
		int coid = name_open(channelName.c_str(), 0);
		if(coid == -1){
			perror("name_open: commsys_to_radar:");
		}

		showaircrafts_cmd msg;
		msg.received = false;
		int status = MsgSend(coid, &msg, sizeof(msg), NULL, 0);
		if(status == -1) {
			perror("MsgSend");
		}

		return 0;
	} else if (m[0] == CHANGE_SPEED_CMD) {
		// Send command
		std::string channelName = "aircraft_commsys_" + m[1];
		int coid = name_open(channelName.c_str(), 0);
		if(coid == -1){
			perror("name_open: aircraft_commsys: Aircraft ID may not exist. ");
		}

		changespeed_cmd msg;
		msg.received = false;
		msg.aircraftID = std::stoi(m[1]);
		msg.xSpeed = std::stof(m[2]);
		msg.ySpeed = std::stof(m[3]);
		msg.zSpeed = std::stof(m[4]);
		int status = MsgSend(coid, &msg, sizeof(msg), NULL, 0);
		if(status == -1) {
			perror("MsgSend: aircraft_commsys:");
		}

		return 0;
	} else if (m[0] == CHANGE_PRED_TIME_CMD) {
		std::string channelName = "commsys_to_atcsystem";
		int coid = name_open(channelName.c_str(), 0);
		if(coid == -1){
			perror("name_open: commsys_to_atcsystem: ");
		}

		changepredtime_cmd msg;
		msg.received = false;
		msg.predTime = std::stoi(m[1]);
		int status = MsgSend(coid, &msg, sizeof(msg), NULL, 0);
		if(status == -1) {
			perror("MsgSend: commsys_to_atcsystem");
		}

		return 0;
	};

	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "CommSys: Unknown command. " << std::endl;
	}


	return 0;
}

void* CommunicationSystem::start(){
	// Start listener for radar
	std::string channelName = "radar_to_commsys";
	name_attach_t *attach = name_attach(NULL, channelName.c_str(), 0);
	if (attach == NULL) {
		perror("name_attach");
	}

	int rcvid;
	showaircrafts_cmd msg;
	while(true){
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if(rcvid == -1){
			perror("MsgReceive");
		}

		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "+-------------+ showaircrafts Result +-------------+" << std::endl;
		for(size_t i = 0; i < msg.aircraftData.size(); i++){
			std::cout << "| Aircraft ID: " << msg.aircraftData[i].getID() << std::endl;
			std::cout << "| \tX, Y, Z Speed (ft/s): "
					<< msg.aircraftData[i].getXSpeed() << ", "
					<< msg.aircraftData[i].getYSpeed() << ", "
					<< msg.aircraftData[i].getZSpeed() << std::endl;
			std::cout << "| \tX, Y, Z Position (ft): "
					<< msg.aircraftData[i].getXPos() << ", "
					<< msg.aircraftData[i].getYPos() << ", "
					<< msg.aircraftData[i].getZPos() << std::endl;
		}
		std::cout << "+-------------+  showaircrafts End   +-------------+" << std::endl;

		MsgReply(rcvid, EOK, NULL, 0);
	}



}

void* CommunicationSystem::startThread(void* context){
	return static_cast<CommunicationSystem*>(context)->start();
}

