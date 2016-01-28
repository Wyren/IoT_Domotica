#ifndef INITIALIZE_H
#define	INITIALIZE_H
#include "communication.h"

#define BVV(bit, val) ((val)?_BV(bit):0)

class Initialize{
public:
	// Declaration
	Communication communication;

	bool initDone = false;
	uint8_t addresses[5][6];

	// Prototypes
	void init();
	Initialize();
};

#endif