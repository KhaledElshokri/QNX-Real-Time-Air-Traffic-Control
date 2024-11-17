#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <mutex>
#include <chrono>

using namespace std;

#include "ATCSystem.h"
#include "Aircraft.h"
#include "MockStorage.h"
#include "Radar.h"
#include "ATCSystem.h"
#include "CommunicationSystem.h"
#include "Display.h"
#include "OperatorConsole.h"

// Global mutexes to protect critical sections
std::mutex coutMutex; 			// Technically a shared memory, so we must lock it when threads are writing to it
std::mutex predTimeMutex;
std::mutex ATCSystemRadarData;

// Timer to trigger the entry of aircraft
std::chrono::steady_clock::time_point programStartTime;

void startSystem(string inputOption){
	vector<Aircraft> initialAircraftList;
	MockStorage mockStorage;
	string data;

	// Initialize components
	Display display;
	CommunicationSystem commSystem;
	OperatorConsole opConsole(commSystem);


	if(inputOption == "Low"){
		data = mockStorage.lowTraffic;
	}else if (inputOption == "Medium"){
		data = mockStorage.mediumTraffic;
	}else if (inputOption == "High"){
		data = mockStorage.highTraffic;
	}else{
		data = mockStorage.congestedTraffic;
	}

	// Parse aircrafts
	stringstream dataStream(data);
	string line;
	while (getline(dataStream, line, ';')) {
		stringstream ss(line);
		int entryTime, id;
		float x, y, z, speedX, speedY, speedZ;
		char comma;

		if (ss >> entryTime >> comma >> id >> comma >> x >> comma >> y >> comma
			>> z >> comma >> speedX >> comma >> speedY >> comma >> speedZ) {
			Aircraft aircraft(entryTime, id, x, y, z, speedX, speedY, speedZ, commSystem);
			initialAircraftList.push_back(aircraft);

		}
	}

	cout << "Parsed " << initialAircraftList.size() << " aircraft entries for " << inputOption << " traffic." << endl;

	Radar radar(initialAircraftList);

	// Initialize plane threads
	pthread_t planeThreadArray[initialAircraftList.size()];
	for(size_t i = 0; i < initialAircraftList.size(); i++){
		//				id,                 attr_struct,
		//              v                   v      start routine           attribute pointer
		pthread_create(&planeThreadArray[i], NULL, &Aircraft::startThread, &initialAircraftList[i]);
		// I used to have a sleep here for 1 second, i removed it and everything still works. JUST INCASE
	}

	ATCSystem ATCSys(radar, display, commSystem);

	// Initialize and start main threads
	pthread_t ATCSystemThread;
	pthread_create(&ATCSystemThread, NULL, &ATCSystem::startThread, &ATCSys);

	pthread_t displayThread;
	pthread_create(&displayThread, NULL, &Display::startThread, &radar);

	pthread_t opConsoleThread;
	pthread_create(&opConsoleThread, NULL, &OperatorConsole::startThread, &opConsole);

	pthread_t radarThread;
	pthread_create(&radarThread, NULL, &Radar::startListenerThread, &radar);

	pthread_t commSystemThread;
	pthread_create(&commSystemThread, NULL, &CommunicationSystem::startThread, &commSystem);

	//Simulator will run indefinitely until program is manually stopped.

	for (size_t i = 0; i < initialAircraftList.size(); i++) {
	    pthread_join(planeThreadArray[i], nullptr);
	}

	pthread_join(ATCSystemThread, nullptr);
	pthread_join(displayThread, nullptr);
	pthread_join(opConsoleThread, nullptr);
	pthread_join(radarThread, nullptr);
	pthread_join(commSystemThread, nullptr);

}

int main() {
	programStartTime = std::chrono::steady_clock::now();

	// Will have to parse the text file here and file in the the list of
	// aircrafts then construct the aircraft class with this list
	// use pThread library to manage priorities

	string inputOption;

	cout << "\tWelcome to our ATC System" << endl;
	cout << "Choose your flight density to simulate." << endl;
	cout << "(Low, Medium, High, Congested)" << endl;

	do{
		cout << "Enter here: ";
		cin >> inputOption;
	}while(inputOption != "Low" && inputOption != "Medium" && inputOption != "High" && inputOption != "Congested");

	startSystem(inputOption);

	return 0;
}



