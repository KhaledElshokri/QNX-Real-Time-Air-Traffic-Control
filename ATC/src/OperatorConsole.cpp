#include "OperatorConsole.h"
#include <mutex>
#include <iostream>
#include <vector>
#include <atomic>
#include <sstream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include "Aircraft.h"

/* RESPONSIBILITIES
	- Lets the controller send commands to the aircraft
	- Command can request the following:
		- Change speed
	- Operator can request for information about a aircraft to show in the radar display. It should show:
		- Flight level
		- Speed
		- Position
	To do this, it must sned the request to the computer system, and which then sends the info to the Data Display
 */

extern std::mutex coutMutex;

OperatorConsole::OperatorConsole(CommunicationSystem iCommSystem) : commSystem(iCommSystem) {

}

OperatorConsole::~OperatorConsole() {}

void* OperatorConsole::start(){
	{
		{
			std::lock_guard<std::mutex> guard(coutMutex);
			std::cout << "OpConsole: OpConsole thread started. " << std::endl;
		}

		/* might need a thread to listen for response
		 * first i want to try just sending a message
		 * dealing with the response in succession instead
		 * of having the send and reply separate threads
		*/

		//all cin management VVV

		std::atomic_bool cinStop;
		cinStop = false;
		int fd = creat("/data/home/qnxuser/commandlog.txt", S_IRUSR | S_IWUSR | S_IXUSR);
		if(fd == -1){
			perror("OpConsole: Cannot create file");
		}

		std::string cmd;
		while(cinStop == false){
			std::getline(std::cin, cmd);
			std::vector<std::string> components; //strings seperated by white space

			//turn cmd into components seperated by white space
			std::stringstream ss(cmd);
			std::string component;
			while (std::getline(ss, component, ' ')) {
				components.push_back(component);
			}

			if(components.size() == 0 ){
				continue; //restart while loop, empty command
			}

			//log command
			if(fd != -1){
				//allocate heap
				char *buffer = new char[cmd.length() + 1];

				//put cmd into buffer
				strncpy(buffer, cmd.c_str(), cmd.length() +1);

				//write to commandlog.txt
				write(fd, buffer, cmd.length() + 1);
				write(fd, "/n", 1);

				//free heap
				delete[] buffer;
			}

			{
				std::lock_guard<std::mutex> guard(coutMutex);
				std::cout << "OpConsole: Command sent: " << cmd << std::endl;
			}

			// note: we must listen for the response on another thread
			int sendStatus = commSystem.send(0, components);
			if(sendStatus == -1){
				perror("OpConsole: Command send error");
			}
		}
	}

}

void* OperatorConsole::startThread(void* context){
	return static_cast<OperatorConsole*>(context)->start();
}
