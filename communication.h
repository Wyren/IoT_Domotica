#ifndef COMMUNICATION_H
#define	COMMUNICATION_H
#include "RF24.h"
#include "command.h"

#define CMD_SIZE 10
#define RETRY_TIME 100

// ID LIST
#define DEVICE_CONTROLLER_ID 1040
#define SERVER_ID 26984
#define GROUP_NUMBER 1
#define PRIVATE_KEY 54874

class Communication{
private:	
	// Declaration
	bool 	ready = true,
			ack = false;
			
	uint8_t first = 55, 
			second = 89, 
			ackCounter,
			retry;			
	uint8_t	message[CMD_SIZE],
			messageCpy[CMD_SIZE],
			cmd[CMD_SIZE],
			cmdPrev[CMD_SIZE];

	uint16_t checksum;

	const uint16_t  groupNumber = GROUP_NUMBER;
	const uint16_t 	privateKey = PRIVATE_KEY;

	// Prototypes
	bool valid();
	uint8_t upChecksum();
	void processCommand();
	bool sendAcknowledge();
	bool encryption(uint8_t input[], bool encrypt);
	void processFlags();

public:
	// Declaration	
	RF24 radio;
	Command command;

	const uint16_t radioNumber = DEVICE_CONTROLLER_ID;
	const uint16_t serverNumber = SERVER_ID;

	uint8_t state = 0;
	bool ledStripActive = false;
	
	// Prototypes
	Communication();
	void listenDevice();
	void prepareMessage(uint8_t data[], bool intern);
};

#endif