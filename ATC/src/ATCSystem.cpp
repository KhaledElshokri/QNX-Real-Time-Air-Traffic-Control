#include <mutex>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sys/dispatch.h>
#include "ATCSystem.h"

/* RESPONSIBILITIES
 * 	- Runs ATCSystem::monitorAirspace() on a 1 second timer.
 * 		- Gets all aircraft from the Radar::runRadar() method, sends the info to the display to show to the console.
 * 		- Computes if there will be violations in the future. If there is, will send a message to the display to display violations
 * 	- Runs ATCSystem::logState() on a 30 second timer.
 * 		- Logs the current aircraft grid to the VM's internal file system as a TXT file.
 *  - Starts a child thread which listens for a command to change prediction time. Changes if nessecary.
*/

extern std::mutex coutMutex;
extern std::mutex predTimeMutex;
extern std::mutex ATCSystemRadarData;

typedef struct {
	std::vector<Aircraft> aircraftData;
	bool received;
} radar_msg;

typedef struct{
	int aircraft1ID;
	int aircraft2ID;
	bool received;
} violation_msg;

typedef struct {
	bool received;
	int predTime;
} changepredtime_cmd;


ATCSystem::ATCSystem(Radar iRadar, Display iDisplay, CommunicationSystem iCommSystem):
		radar(iRadar), display(iDisplay), commSystem(iCommSystem)
{

}

void deleteLogFile() {
    const char* filePath = "/data/home/qnxuser/displaylog.txt";
    unlink(filePath);
}

// Checks for aircraft violations
void ATCSystem::checkViolations(std::vector<Aircraft>* radarOutput)
{
	// The constraint to check is if 2 aircrafts are less than 1000 feet apart in hight or
	// 3000 feet apart in width
	// This should be called every n seconds, depending on user input
	// If there is a violation that could happen within the next 3 minutes we should display
	// the alarm in Display.cpp

	std::vector<Aircraft>& radarFindings = *radarOutput; //deref the pointer

	{ // Critical Section
		std::lock_guard<std::mutex> guard(ATCSystemRadarData);
		radarData = radarFindings;
	}

	int VERTICAL_CONSTRAINT = 1000;
	int HORIZONTAL_CONSTRAINT = 3000;

	//check each aircraft against each other aircraft
	for (size_t i = 0; i < radarFindings.size(); i++) {
		for (size_t j = i + 1; j < radarFindings.size(); j++) {
			Aircraft aircraft1Projection = radarFindings[i];
			Aircraft aircraft2Projection = radarFindings[j];

			int predTime;
			{
				std::lock_guard<std::mutex> guard(predTimeMutex);
				predTime = predictionTimeSeconds;
			}

			// predict N seconds ahead of time for each aircraft
			float aircraft1ProjX = (radarFindings[i].getXPos() + (radarFindings[i].getXSpeed() * predTime));
			float aircraft1ProjY = (radarFindings[i].getYPos() + (radarFindings[i].getYSpeed() * predTime));
			float aircraft1ProjZ = (radarFindings[i].getZPos() + (radarFindings[i].getZSpeed() * predTime));
			aircraft1Projection.setPos(aircraft1ProjX, aircraft1ProjY, aircraft1ProjZ);

			float aircraft2ProjX = (radarFindings[j].getXPos() + (radarFindings[j].getXSpeed() * predTime));
			float aircraft2ProjY = (radarFindings[j].getYPos() + (radarFindings[j].getYSpeed() * predTime));
			float aircraft2ProjZ = (radarFindings[j].getZPos() + (radarFindings[j].getZSpeed() * predTime));
			aircraft2Projection.setPos(aircraft2ProjX, aircraft2ProjY, aircraft2ProjZ);

			//calculate airspace needed around aircraft 1
			int x1Min = aircraft1Projection.getXPos() - HORIZONTAL_CONSTRAINT;
			int x1Max = aircraft1Projection.getXPos() + HORIZONTAL_CONSTRAINT;
			int y1Min = aircraft1Projection.getYPos() - HORIZONTAL_CONSTRAINT;
			int y1Max = aircraft1Projection.getYPos() + HORIZONTAL_CONSTRAINT;
			int z1Min = aircraft1Projection.getZPos() - VERTICAL_CONSTRAINT;
			int z1Max = aircraft1Projection.getZPos() + VERTICAL_CONSTRAINT;

			//calculate airspace needed around aircraft 2
			int x2Min = aircraft2Projection.getXPos() - HORIZONTAL_CONSTRAINT;
			int x2Max = aircraft2Projection.getXPos() + HORIZONTAL_CONSTRAINT;
			int y2Min = aircraft2Projection.getYPos() - HORIZONTAL_CONSTRAINT;
			int y2Max = aircraft2Projection.getYPos() + HORIZONTAL_CONSTRAINT;
			int z2Min = aircraft2Projection.getZPos() - VERTICAL_CONSTRAINT;
			int z2Max = aircraft2Projection.getZPos() + VERTICAL_CONSTRAINT;

			//if two boxes overlap
			if ((x1Max >= x2Min && x2Max >= x1Min)
					&& (y1Max >= y2Min && y2Max >= y1Min)
					&& (z1Max >= z2Min && z2Max >= z1Min)) {
				//VIOLATION DETECTED

				//Send violation info the Display
				std::string channelName = "atc_to_display_violations";
				int coid = name_open(channelName.c_str(), 0);
				if(coid == -1){
					perror("name_open");
				}

				violation_msg msg;
				msg.received = false;
				msg.aircraft1ID = radarFindings[i].getID();
				msg.aircraft2ID = radarFindings[j].getID();
				violation_msg reply;
				reply.received = false;
				int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
				if(status == -1){
					perror("MsgSend");
				}

				//if no reply
				if(reply.received == false){
					perror("No reply from display");
				}
			}

		}
	}
}

// Gives a log statement of the airspace
void ATCSystem::logState(union sigval sv)
{
    ATCSystem* ATCSys = static_cast<ATCSystem*>(sv.sival_ptr);

    {
        std::lock_guard<std::mutex> guard(coutMutex);
        std::cout << "ATCSystem: Logging state" << std::endl;
    }

    // Open the log file in append mode
    int fd = open("/data/home/qnxuser/displaylog.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR);
    if (fd == -1) {
        perror("ATCSystem: Cannot open log file");
        return; // Exit the function if the file cannot be opened
    }

    // Create a copy of the radar data to avoid modifying shared state
    std::vector<Aircraft> radarDataCopy;
    {
        std::lock_guard<std::mutex> guard(ATCSystemRadarData); // Assuming ATCSystemRadarData is a mutex for radarData
        radarDataCopy = ATCSys->radarData;
    }

    // Generate the display string
    std::string displayString = ATCSys->display.buildGrid(radarDataCopy);

    // Write the string to the file
    ssize_t bytesWritten = write(fd, displayString.c_str(), displayString.size());
    if (bytesWritten == -1) {
        perror("ATCSystem: Error writing to log file");
    } else {
        {
            std::lock_guard<std::mutex> guard(coutMutex);
            std::cout << "ATCSystem: Successfully logged state to file." << std::endl;
        }
    }

    // Close the file
    if (close(fd) == -1) {
        perror("ATCSystem: Error closing log file");
    }
}


void ATCSystem::monitorAirspace(union sigval sv){
	ATCSystem* ATCSys = static_cast<ATCSystem*>(sv.sival_ptr);

	// Get info of all flights from the radar
	std::vector<Aircraft> radarFindings = ATCSys->radar.runRadar();

	// Check for airspace violations
	ATCSys->checkViolations(&radarFindings);

	// Send radar data to the display
	std::string channelName = "radar_to_display";
	int coid = name_open(channelName.c_str(), 0);
	if (coid == -1) {
		perror("name_open");
	}

	radar_msg msg;
	msg.received = false;
	msg.aircraftData = radarFindings;
	radar_msg reply;
	msg.received = false;
	reply.aircraftData = radarFindings;
	int status = MsgSend(coid, &msg, sizeof(msg), &reply, sizeof(reply));
	if(status == -1){
		perror("MsgSend");
	}

	if(reply.received == false){
		perror("No reply from Display");
	}

}


void* ATCSystem::start() {
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "ATC System started" << std::endl;
	}

	// Delete past log file
	deleteLogFile();

	// Start thread for listening for prediction time change
	pthread_t ATCSysListenerThread;
	pthread_create(&ATCSysListenerThread, NULL, &ATCSystem::startListenerThread, this);

	// Start timer for collision checking
	timer_t collisionCheck_timer_id;
	struct sigevent sevC;
	struct itimerspec itsC;

	sevC.sigev_notify = SIGEV_THREAD;
	sevC.sigev_notify_function = ATCSystem::monitorAirspace;
	sevC.sigev_value.sival_ptr = this; // Pass this ATCSystem instance
	sevC.sigev_notify_attributes = nullptr;

	if(timer_create(CLOCK_REALTIME, &sevC, &collisionCheck_timer_id) == -1){
		std::cerr << "Error creating timer for ATCSystem Collision Check: " << strerror(errno) << std::endl;
	}

	//repeats every second
	itsC.it_value.tv_sec = 1;
	itsC.it_value.tv_nsec = 0;
	itsC.it_interval.tv_sec = 1;
	itsC.it_interval.tv_nsec = 0;

	if(timer_settime(collisionCheck_timer_id, 0, &itsC, nullptr) == -1){
		std::cerr << "Error setting timer for ATCSystem Collision Check: " << strerror(errno) << std::endl;
	}

	// TODO start timer for file logging every 30 seconds
	timer_t log30_timer_id;
	struct sigevent sevL;
	struct itimerspec itsL;

	sevL.sigev_notify = SIGEV_THREAD;
	sevL.sigev_notify_function = ATCSystem::logState;
	sevL.sigev_value.sival_ptr = this;
	sevL.sigev_notify_attributes = nullptr;

	if(timer_create(CLOCK_REALTIME, &sevL, &log30_timer_id) == -1){
		std::cerr << "Error creating timer for ATCSystem 30 second display log: " << strerror(errno) << std::endl;
	}

	itsL.it_value.tv_sec = 30;
	itsL.it_value.tv_nsec = 0;
	itsL.it_interval.tv_sec = 30;
	itsL.it_interval.tv_nsec = 0;

	if(timer_settime(log30_timer_id, 0, &itsL, nullptr) == -1){
		std::cerr << "Error setting timer for ATC 30 second display log: " << strerror(errno) << std::endl;
	}

	return nullptr;

}

// Listen for request to change prediction time
void* ATCSystem::startListener(){
	std::string channelName = "commsys_to_atcsystem";
	name_attach_t *attach = name_attach(NULL, channelName.c_str(), 0);
	if(attach == NULL){
		perror("name_attach");
	}

	int rcvid;
	changepredtime_cmd msg;

	while(true){
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if(rcvid == -1){
			perror("MsgReceive");
		}

		{
			std::lock_guard<std::mutex> guard(coutMutex);
			std::cout << "ATCSystem: Received request to change prediction time. " << std::endl;
		}

		{
			std::lock_guard<std::mutex> guard(predTimeMutex);
			this->setPredTime(msg.predTime);
		}

		MsgReply(rcvid, EOK, NULL, 0);
	}

}

void* ATCSystem::startThread(void* context){
	return static_cast<ATCSystem*>(context)->start();
}

void* ATCSystem::startListenerThread(void* context){
	return static_cast<ATCSystem*>(context)->startListener();
}
