// ****************************************************************************
// *                                iotComMan.h                               *
// *                            Author: Michel Roke                           *
// ****************************************************************************
#ifndef IOTCOMMAN_H
#define IOTCOMMAN_H

// ****************************************************************************
// *                        ******** Includes ********                        *
// ****************************************************************************
#include "iotInit.h"

class IotComMan {
private:
// ****************************************************************************
// *                        ******** Variables ********                       *
// ****************************************************************************
    uint8_t lastCmd[6];                                 // Container for the last command compare with the acknowledge
    bool debugMode;                                     // Determines if users wants to debug

// ****************************************************************************
// *                        ******** Functions ********                       *
// ****************************************************************************
    void iotComMan();                                   // Contains all initializers
    void msgState();                                    // Processes the function outputs
    uint8_t errorProcess(uint8_t code);                 // Evaluates the errorCode and redirects accordingly
    uint8_t processMsg();                               // Processes commands for the Raspberry, gives 0 if output is unexpected creating a reset
    void sensorUpdate();                                // Checks which sensor needs to be updated and passes it new information on to pilight
public:
// ****************************************************************************
// *                        ******** Functions ********                       *
// ****************************************************************************
    void cmdDevice(char setting);                       // Contains the program main and handles start-up commands
    bool checkDevice(uint16_t devID, uint8_t groupNum); // Checks if the device is know, if not adds the device if there is space left
    void fillCmd(uint16_t actValue[]);                  // Put the command input in "command" to be send

};

#endif  /* IOTCOMMAN_H */
