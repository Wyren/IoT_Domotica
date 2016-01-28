#ifndef COMMAND_H
#define	COMMAND_H
#include <stdint.h>

#define RED   0
#define GREEN 1
#define BLUE  2
#define PWMM  3

// DEVICE LIST
#define LAMP 1
#define SENSEO 2
#define CUP_1 3
#define CUP_2 4
#define LAVA 5
#define WATER_BOILER 6
// #define NEW_DEVICE x

// PIN ASSIGNMENT
#define RED_RGB_PIN 3
#define GREEN_RGB_PIN 6
#define BLUE_RGB_PIN 5
#define LAVA_LAMP_PIN 9
#define BOILER_PIN 10
#define PWM_PIN 3
#define TEMP_SENSOR 83
#define S0 8
#define S1 9
#define S2 12
#define S3 11
#define OUT 10

// TIMER
#define SENSEO_COUNT 600

/*
  SENSEO PIINS
  blue wire - LED
  orange wire - On button
  brown wire - one cup
  green wire - two cups
*/

typedef struct {
	bool power[2];
	uint8_t pin[4];
	uint8_t RGB[4];
}state;

class Command{
private:
	// Declaration
	uint16_t counter;
	long timer;

	uint8_t ledCounter,
			ledStripState;

	uint8_t PWM[3];

	bool timerSet = false,
		 nunchuckInputButton = false,
		 debounce = false;

	// Prototypes
	void dimDevice(uint8_t delayTime, uint8_t value);
	void fadeLamp(uint8_t delayTime);
	uint16_t pwmColour(uint8_t colour, uint8_t newValue, int counter, uint16_t secCount, uint8_t delayTime);
	bool readSenseo();
	void powerSenseo();
	void color();
	void getRGB(uint8_t cmd[]);
	void readNunchuck();

public: 
	// Declaration
	state deviceState;
	
	bool flagWaitSenseo,
		 flagReadySenseo,
		 flagDoorBell,
		 flagLED;

	// Prototypes
	void powerDevice(uint8_t value, uint8_t deviceNr);
	void executeCommand(uint8_t cmd[]);
	void checkFlags();
	void ledStrip();
};

#endif