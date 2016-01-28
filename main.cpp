#include "SPI.h"
#include "initialize.h"
#include <Arduino.h>
#include <avr/io.h>

extern Initialize initialize;

int main(){
	bool debug = true;

	init();														// Arduino init

	if (debug){
		Serial.begin(115200);									// print debug messages
	}
	
	if (!initialize.initDone){
		initialize.init();										// initialize the nRF chip
	}

	while(1){
		initialize.communication.listenDevice();				// Start listening for data
	}
	return 0;
}
