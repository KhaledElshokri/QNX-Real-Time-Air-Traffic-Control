#include <mutex>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <sys/dispatch.h>
#include <vector>
#include <chrono>

#include "Aircraft.h"
#include "Display.h"

extern std::mutex coutMutex;
extern std::chrono::steady_clock::time_point programStartTime;

/* RESPONSIBILITIES
 *  - Listens for radar to tell it to Display::renderGrid() to the console
 *  - Listens for the ATCSystem to tell it to return Display::buildGrid(), which is a string to save to a file.
*/

typedef struct {
	std::vector<Aircraft> aircraftData;
	bool received;
} radar_msg;

typedef struct{
	int aircraft1ID;
	int aircraft2ID;
	bool received;
} violation_msg;

inline int getElapsedTime() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - programStartTime).count();
}


// Renders Aircraft positions from the list
void Display::renderGrid(std::vector<Aircraft> aircraftData)
{
//	std::vector<Aircraft>& aircraftData = *givenAircraftData; //deref parameter

	/*
	 * Actual size is 100,000 by 100,000 (X by Y)
	 * with 20 row and column size, each cell is 5,000 x 5,000
	 *
	 * The grids 0,0 is in the bottom left corner
	 *
	 * . is empty, char value quantity of aircraft in the space
	 * 				^	Because if this, if there are over 9 aircraft in the
	 * 					5000x5000 square, it will get incremented past the
	 * 					0 - 9 numerical symbols in ASCII. It doens't break, just
	 * 					becomes unreadable
	 */

	int Size = 100000; // 100000x100000
	int rowSize = 20, columnSize = 20;
	int cellSize = Size/rowSize;

	// Create grid and initialize to all .'s
	char grid[rowSize][columnSize];
	for (int i = 0; i < rowSize; ++i) {
	    for (int j = 0; j < columnSize; ++j) {
	        grid[i][j] = '.';
	    }
	}

	int xPosInGrid, yPosInGrid;

	// Put aircraf locations into grid
	for(int i = 0; i < aircraftData.size(); i++){
		if(aircraftData[i].getEntryTime() <= getElapsedTime()){
			xPosInGrid = (aircraftData[i].getXPos())/cellSize;
			yPosInGrid = (aircraftData[i].getYPos())/cellSize;

			if(grid[xPosInGrid][yPosInGrid] == '.'){
				grid[xPosInGrid][yPosInGrid] = '1';
			}else{
				grid[xPosInGrid][yPosInGrid] = (grid[xPosInGrid][yPosInGrid] + 1);
			}
		}
	}

	// Render grid to cout
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		for (int j = columnSize - 1; j >= 0; --j) {
			for (int i = 0; i < rowSize; ++i) {
				std::cout << grid[i][j] << ' ';
			}
			std::cout << std::endl;
		}
		std::cout << "\n\n\n" << std::endl;
	}

}

std::string Display::buildGrid(std::vector<Aircraft> aircraftData)
{
    int Size = 100000; // 100000x100000
    int rowSize = 20, columnSize = 20;
    int cellSize = Size / rowSize;

    // Initialize grid to all '.'s
    char grid[rowSize][columnSize];
    for (int i = 0; i < rowSize; ++i) {
        for (int j = 0; j < columnSize; ++j) {
            grid[i][j] = '.';
        }
    }

    int xPosInGrid, yPosInGrid;

    // Populate grid with aircraft locations
    for (int i = 0; i < aircraftData.size(); i++) {
    	if(aircraftData[i].getEntryTime() <= getElapsedTime()){
			xPosInGrid = (aircraftData[i].getXPos()) / cellSize;
			yPosInGrid = (aircraftData[i].getYPos()) / cellSize;

			if (grid[xPosInGrid][yPosInGrid] == '.') {
				grid[xPosInGrid][yPosInGrid] = '1';
			} else {
				grid[xPosInGrid][yPosInGrid] = (grid[xPosInGrid][yPosInGrid] + 1);
			}
    	}
    }

    // Build the grid as a string
    std::string gridString;
    for (int j = columnSize - 1; j >= 0; --j) {
        for (int i = 0; i < rowSize; ++i) {
            gridString += grid[i][j];
            gridString += ' ';
        }
        gridString += '\n';
    }

    gridString += "\n\n\n";

    return gridString;
}

void* Display::start(){
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "Display: Display thread started" << std::endl;
	}

	//create child thread to just listen for messages from radar
	pthread_t displayRadarListenerThread;
	pthread_create(&displayRadarListenerThread, NULL, &Display::startRadarListenerThread, this);

	pthread_t displayViolationListenerThread;
	pthread_create(&displayViolationListenerThread, NULL, &Display::startViolationListenerThread, this);

	return nullptr;
}

void* Display::startThread(void* context){
	return static_cast<Display*>(context)->start();
}

void* Display::startRadarListener(){
	{
		std::lock_guard<std::mutex> guard(coutMutex);
		std::cout << "Display: Display radar listener thread started" << std::endl;
	}

	//listen for data from radar
	std::string channelName = "radar_to_display";
	name_attach_t *attach = name_attach(NULL, channelName.c_str(), 0);
	if (attach == NULL ) {
		perror("name_attach");
	}

	int rcvid;
	radar_msg msg;

	//used to update display every 5 seconds
	int requestNumber = 0;
	while(true){
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if (rcvid == -1) {
			perror("MsgReceive");
		}

		radar_msg reply;
		reply.received = true;

		int status = MsgReply(rcvid, 0, &reply, sizeof(reply));

		//render the grid with the data from ATCSystems radar
		requestNumber++;
		if(requestNumber % 5 == 0){
			Display::renderGrid(msg.aircraftData);
		}
	}
}

void* Display::startRadarListenerThread(void* context){
	return static_cast<Display*>(context)->startRadarListener();
}

void* Display::startViolationListener(){
	std::string channelName = "atc_to_display_violations";
	name_attach_t *attach = name_attach(NULL, channelName.c_str(), 0);
	if (attach == NULL){
		perror("name_attach");
	}

	int rcvid;
	violation_msg msg;

	while(true){
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
		if(rcvid == -1) {
			perror("MsgReceive");
		}

		{
			std::lock_guard<std::mutex> guard(coutMutex);
			std::cout << "+---- Got violation from ATCSystem ----+" << std::endl;
			std::cout << "|Violation is between flight " << msg.aircraft1ID << " and flight " << msg.aircraft2ID << std::endl;
			std::cout << "|Enter a command in the operator console to instruct them to change course. " << std::endl;
			std::cout << "+--------------------------------------+" << std::endl;
		}

		violation_msg reply;
		reply.received = true;

		int status = MsgReply(rcvid, 0, &reply, sizeof(reply));
	}
}

void* Display::startViolationListenerThread(void* context){
	return static_cast<Display*>(context)->startViolationListener();
}
