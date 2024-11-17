#ifndef SRC_OPERATORCONSOLE_H_
#define SRC_OPERATORCONSOLE_H_

#include "CommunicationSystem.h"

class OperatorConsole {

private:
	CommunicationSystem commSystem;

public:
	OperatorConsole(CommunicationSystem iCommSystem);
	virtual ~OperatorConsole();

	void* start();

	static void* startThread(void* context);
};

#endif /* SRC_OPERATORCONSOLE_H_ */
