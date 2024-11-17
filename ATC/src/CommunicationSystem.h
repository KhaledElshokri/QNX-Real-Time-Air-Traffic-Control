#ifndef SRC_COMMUNICATIONSYSTEM_H_
#define SRC_COMMUNICATIONSYSTEM_H_

#include <vector>

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
