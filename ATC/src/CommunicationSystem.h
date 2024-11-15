#ifndef SRC_COMMUNICATIONSYSTEM_H_
#define SRC_COMMUNICATIONSYSTEM_H_

#include <vector>


/* Responsible for:
	- Transmission of controller commands to the specified aircraft.
	- To send command "m" to aircraft "R" over the communication subsystem, the computer system emits the following command: send(R,m)
 */

class CommunicationSystem {
public:
	CommunicationSystem();
	virtual ~CommunicationSystem();

	// R is aircraft ID, m is message
	int send(int R, std::vector<std::string> m);

	void* start();

	static void* startThread(void* context);
};

#endif /* SRC_COMMUNICATIONSYSTEM_H_ */
