// ****************************************************************************
// *                               iotComMan.cpp                              *
// *                            Author: Michel Roke                           *
// ****************************************************************************

// ****************************************************************************
// *                        ******** Includes ********                        *
// ****************************************************************************
#include "iotComMan.h"
#include "iotNRFCom.h"
#include "iotSocketCom.h"
#include "iotDebug.h"

using namespace IotNetwork;

DeviceManagement devMan;
IotNRFCom nRFCom;
IotDebug debug;
IotSocketCom socketCom;
extern RF24 radio;

// ****************************************************************************
// *                      ******** Command main ********                      *
// ****************************************************************************
void IotComMan::cmdDevice(char setting) {
    // Initialize the program
    IotComMan::iotComMan();

    // Check if debug mode is active
    debugMode = (setting == 'd') ? true : false;

    for (;;) {  // Keep the program running
        IotComMan::msgState();
    }
}

// ****************************************************************************
// *               ******** Initializing the program **********               *
// ****************************************************************************
void IotComMan::iotComMan() {
    nRFCom.iotNRFCom();
    socketCom.iotSocketCom();
    sleep(0.5);                         // Allow everything to settle in
}

// ****************************************************************************
// *             ******** Checking the device existence **********            *
// ****************************************************************************
bool IotComMan::checkDevice(uint16_t devID, uint8_t groupNum) {
    uint8_t count = 0;

    // Check the device list for existing devices
    for (count = 0; count < CMD_SIZE; ++count) {
        if (devID == devMan.deviceIDList[count]) {
            devMan.devArrayNum = count;
            return true;
        }

        // Empty spaces found, device does not exist yet so it's added
        if (devMan.deviceIDList[count] == 0) {
            devMan.deviceIDList[count] = devID;
            devMan.deviceGrpList[count] = groupNum; // Implemented for future development
            devMan.devArrayNum = count;
            return true;
        }
    }

    // No more space left for devices
    // Create a report function to pilight and ensure a dynamic space allocation
    return false;
}

// ****************************************************************************
// *                  ******** Filling the command **********                 *
// ****************************************************************************
void IotComMan::fillCmd(uint16_t actValue[]) {
    uint8_t arrNum;

    // Add the destination to the command
    devMan.command[0] = devMan.deviceIDList[devMan.devArrayNum] >> 8;
    devMan.command[1] = (uint8_t)devMan.deviceIDList[devMan.devArrayNum];

    // Add devArrayNum to lastCmd for the acknowledge check
    lastCmd[0] = devMan.devArrayNum;

    // Add the action to be done, fill lastCmd as well for later checks
    devMan.command[2] = lastCmd[1] = (uint8_t)actValue[2];

    // Add the action values 
    for (arrNum = 3; arrNum < 7; ++arrNum){
        devMan.command[arrNum] = lastCmd[(arrNum - 1)] = (uint8_t)actValue[arrNum];
    }
    // Add the sender id
    devMan.command[7] = radioNumber >> 8;
    devMan.command[8] = (uint8_t)radioNumber;
}

// ****************************************************************************
// *                      ******** Send states **********                     *
// ****************************************************************************
void IotComMan::msgState() {
    static uint8_t 
        currentState = 1,           // Registers the current state
        comState = 0,               // Registers communication state
        errorCode = 0;              // Registers the communication outcome

    if (debugMode && currentState != 1) {
        std::cout << "Current state: " << (short)currentState << ", error code:  " << (short)errorCode << std::endl;
    }

    switch(currentState) {
        case 1:   // Waiting state
        
            // Listen to the radio and wait for a command
            if (radio.available(&devMan.group)) {
                // Is an acknowledge expected, then leave comState untouched
                comState = (errorCode == 1) ? comState : 2;
                currentState = 3;
            }

            // Check the command file for commands from pilight
            if (socketCom.readCmdFile()) {
                comState = 1;
                currentState = 2;
            }

            // (Implementation of pilight socket event) Listen to the pilight socket for commands
            /**
             * Connect to the pilight socket;
             * currentState = IotComMan::readSocket();
             */

            if (debugMode) {
                currentState = debug.iotDebug() ? 2 : 1;
                comState = 1;
            }
            break;

        case 2:   // Sending state
            // Was a command sent from pilight or does an acknowledge need to be send
            errorCode = !nRFCom.sendCmd() ? 5 : (comState == 1) ? 1 : 4;

            // If errorCode is 5; No message sent
            // comState = (errorCode == 5) ? comState : 2;
            currentState = 4;
            break;

        case 3:   // Receiving state
            // Listen for the incoming message
            errorCode = !nRFCom.receiveCmd() ? 6 : (comState == 1) ? 2 : 3;

            // If errorCode is 6; Message for another device
            // comState = (errorCode == 6) ? comState : 3;
            currentState = 4;
            break;

        case 4:   // Determine state
            // Evaluates the program state and responds accordingly
            currentState = IotComMan::errorProcess(errorCode);

            if (debugMode) {  // Debug output to console
                debug.outputValue(devMan.command, CMD_SIZE);
                debug.outputValue(devMan.message, CMD_SIZE);
            }

            break;

        case 5:   // Process state
            // Determine via switch state what the command is
            currentState = IotComMan::processMsg();
            // Create an error handling process

            break;

        case 6:   // End of communication state
            // Send an update message to pilight
            // IotComMan::sendSocket();

            // Reset variables
            devMan.devArrayNum = 255;
            errorCode = 0;
            comState = 0;

            // Reset the state back to the beginning
            currentState = 1;
            break;

        default:    // In case of an unknown situation go to state 6 for a reset
            currentState = 6;
    }
}

// ****************************************************************************
// *                ******** Process the error code **********                *
// ****************************************************************************
uint8_t IotComMan::errorProcess(uint8_t code) {
    static uint8_t comAttempt = 0;  // Registers the amount of attempt of communication
                                    // Store the sender id for verification
    uint16_t senderIDCheck = (devMan.command[7] << 8) + devMan.command[8];
    uint8_t nextState = 0;          // Return value

    switch (code) {
        case 1:
            // Command sent, listen for a acknowledge
            // Go straight to state 1, keep errorCode and comState set
            comAttempt = 0; // Reset the attempt counter
            nextState = 1;
            break;

        case 2:
            // Did we send a message to this device
            if (senderIDCheck == devMan.deviceIDList[lastCmd[0]]) {
                // Go to state 5 to confirm acknowledge
                nextState = 5;
            } else {
                // New command gotten, go to process state
                // Not yet implemented
            }
            break;

        case 3:
            // Command received, check sender device id
            if (IotComMan::checkDevice(senderIDCheck, devMan.group)) {
                // Device is know, send acknowledge
                devMan.command[0] = senderIDCheck >> 8;
                devMan.command[1] = (uint8_t)senderIDCheck;
                for (uint8_t i = 6; i > 2; --i) {
                    devMan.command[i] = devMan.command[(i-1)];
                }
                devMan.command[2] = 128;
                devMan.command[7] = radioNumber >> 8;
                devMan.command[8] = (uint8_t)radioNumber;

                nextState = 2;
            }
            break;

        case 4:
            // Acknowledge sent, process command
            comAttempt = 0;
            nextState = 5;
            break;

        case 5:
            // Message not received by any device, send again and up attempt counter
            ++comAttempt;
            nextState = 2;

            // Maximum reached inform user via process state
            if (comAttempt > 3) {
                // Start listening could not be called in sendCmd
                radio.startListening();
                comAttempt = 0;
                // errorCode = 8;
                nextState = 5;
            }
            break;

        case 6:
            // Message was for another device or same message
            nextState = 6;
            break;

        default:    // In case of an unknown situation go to state 6 for reset
            nextState = 6;
    }

    return nextState;
}

// ****************************************************************************
// *                  ******** Update sensor state **********                 *
// ****************************************************************************
void IotComMan::sensorUpdate() {
    // First value from the command is the sensor id
    // The Sensor id should match the one in pilight to send an update via socket
    // Second value is the sensor state; digital input = boolean, analog input = 0 - 1024 (10 bit ADC)
    switch(devMan.command[3]) {
        case 12: // Doorbell
            // Not yet implemented
            // cout << "Doorbell" << endl;
            break;
        case 116: // Senseo
            // Not yet implemented
            // cout << "Senseo" << endl;
            break;
        default:    // Unknown sensor
            // Inform user about the situation
            // cout << "Unknown value" << endl;
        return;
    }

}

// ****************************************************************************
// *                ******** Process external input **********                *
// ****************************************************************************
uint8_t IotComMan::processMsg() {
    uint8_t 
        nextState = 0,
        count = 0,
        counter = 0;

    switch (devMan.command[2]) { // command[2] contains the action to preform
        // Create a function to process the doorbell, Senseo, etc. messages
        case 32: // Sensor update
            // Template function
            IotComMan::sensorUpdate();
            break;

        case 127: // Resend last message
            // Create an Acknowledge function
            break;

        case 128: // Check acknowledge           // Create separate function?
            // Start reading at 1 which stores action id
            for (counter = 1; counter < 6; ++counter) {
                nextState = (lastCmd[counter] != devMan.command[(counter + 2)]) ? 6 : 2;

                if (nextState == 2) {
                    // Data corrupted; inform user and resend lastCmd
                    devMan.command[0] = devMan.deviceIDList[lastCmd[0]] >> 8;
                    devMan.command[1] = (uint8_t)devMan.deviceIDList[lastCmd[0]];
                    for (count = 1; count < 6; ++count) {
                        devMan.command[(count + 1)] = lastCmd[count];
                    }
                    devMan.command[7] = radioNumber >> 8;
                    devMan.command[8] = (uint8_t)radioNumber;
                    continue; // 
                }
            }
            
            if(nextState == 6){
                // No flag set, acknowledge accepted clear lastCmd
                memset(lastCmd, 0, sizeof(lastCmd));
            }
            break;

        default:
            // No matching command;
            nextState = 0;
    }

    return nextState;
}
