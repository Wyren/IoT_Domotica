// ****************************************************************************
// *                                  init.h                                  *
// *                            Author: Michel Roke                           *
// ****************************************************************************

// ****************************************************************************
// *                       ******** To-do list ********                       *
// ****************************************************************************
/**
 * Socket class needs to be implemented; read, send, connect, check connection, auto-reconnect, error handling
 * Incoming command function to process device commands
 * Resolve incoming command before acknowledge; msgState->case 4->case 2
 * Create a error handling process for failed commands
 * Create a dynamic array for maximum 255 devices and add array management for memory clean-up
 * Create a maximum devices reached report for user information
 * Make the program multi threaded to increase communication uptime
 * Make private key 24/32 bit and update encryption
 */

#ifndef IOTINIT_H
#define IOTINIT_H

// ****************************************************************************
// *                        ******** Includes ********                        *
// ****************************************************************************
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>

/** 
 * Command build-up: 10 Bytes total
 *      Destination id 2B, 
 *      Action 1B,
 *      Action values 4B,
 *      Sender id 2B,
 *      Public key 1B;
 */
#define CMD_SIZE 10

/**
 * Action list:
 *      1: Multi relay; switch relay X (1: relay id (X), 2: 1/0 (On/Off))
 *      2: Update Dimmer (If dimmer was off it is turned on)
 *      3: Update RGB lamp (Sends 3 bytes, can be used for functions which needs the same amount)
 *      32: Sensor alert; Alerts the Raspberry about a change in sensor value
 *      127: Request last message
 *      128: Sends Acknowledgement (1: Action, 2-4: Values)
 */

// ****************************************************************************
// *                       ******** Name spaces ********                      *
// ****************************************************************************
namespace IotNetwork {

// ****************************************************************************
// *                   ******** User Configuration ********                   *
// ****************************************************************************
    // Assign a unique identifier for this node
    const uint16_t radioNumber = 26984;

    // Radio pipe addresses for communication. 
    // These can be changed to personal preferences; Max 5 characters
    //                              General  Living   Kitchen  Bedroom  Sensors
    const uint8_t addresses[5][6] = {"GTinY","LTinY", "KTinY", "BTinY", "STinY"};

    // The key can be altered, but NEEDS to be the same for the entire network!
    const uint16_t privateKey = 54874;
}

typedef struct {
    // Array with registered devices, ten maximum at the moment \\ Change to vector for flexibility?
    uint16_t deviceIDList[CMD_SIZE];

    uint8_t
        deviceGrpList[CMD_SIZE],    // An array with the group id of the corresponding device
        command[CMD_SIZE],          // Decrypted message to read
        message[CMD_SIZE],          // Encrypted command to send
        // temp values to speed up the process
        devArrayNum,                // Value to store the last contacted device
        group;                      // Value to store the last connected device group
            
} DeviceManagement;

#endif  /* IOTINIT_H */
