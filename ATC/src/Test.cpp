#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <mutex>

using namespace std;

#include "ATCSystem.h"
#include "Aircraft.h"
#include "MockStorage.h"

std::mutex coutMutex;

void parseAircraftData(const string& data, vector<Aircraft>& aircraftList) {
    stringstream dataStream(data);
    string line;

    while (getline(dataStream, line, ';')) {
        stringstream ss(line);
        int entryTime, id;
        float x, y, z, speedX, speedY, speedZ;
        char comma;

        if (ss >> entryTime >> comma >> id >> comma >> x >> comma >> y >> comma
            >> z >> comma >> speedX >> comma >> speedY >> comma >> speedZ) {
            Aircraft aircraft(entryTime, id, x, y, z, speedX, speedY, speedZ);
            aircraftList.push_back(aircraft);
        }
    }
}


void startSystem(string inputOption){
	vector<Aircraft> aircraftList;
	MockStorage mockStorage;
	string data;

	if(inputOption == "Low"){
		data = mockStorage.lowTraffic;
	}else if (inputOption == "Medium"){
		data = mockStorage.mediumTraffic;
	}else if (inputOption == "High"){
		data = mockStorage.highTraffic;
	}else{
		data = mockStorage.congestedTraffic;
	}

	parseAircraftData(data, aircraftList);
	cout << "Parsed " << aircraftList.size() << " aircraft entries for " << inputOption << " traffic." << endl;

	//initialize main threads


	//initialize plane threads
	pthread_t planeThreadArray[aircraftList.size()];
	for(size_t i = 0; i < aircraftList.size(); i++){
		//				id,                 attr_struct,
		//              v                   v      start routine     attribute pointer
		pthread_create(&planeThreadArray[i], NULL, &Aircraft::startThread, &aircraftList[i]);
		sleep(1); //sleep for 1 second
	}

	//sleep this thread for 120 seconds, letting the ATC simulation run.
	sleep(120);

	for (size_t i = 0; i < aircraftList.size(); i++) {
	    pthread_join(planeThreadArray[i], nullptr);
	}
}

int main() {

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



