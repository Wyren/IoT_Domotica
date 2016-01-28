// ****************************************************************************
// *                                iotNRFCom.h                               *
// *                            Author: Michel Roke                           *
// ****************************************************************************

#ifndef IOTNRFCOM_H
#define IOTNRFCOM_H

// ****************************************************************************
// *                        ******** Includes ********                        *
// ****************************************************************************
#include "iotInit.h"
#include <RF24/RF24.h>

class IotNRFCom {
private:
// ****************************************************************************
// *                        ******** Variables ********                       *
// ****************************************************************************
    uint8_t publicKey;                       		// Value to store the current public key

// ****************************************************************************
// *                        ******** Functions ********                       *
// ****************************************************************************
    bool encryption(uint8_t input[], bool encrypt); // Encrypts or decrypts the input depending on the boolean
    void raisePublicKey();                          // Changes the public key via a fibonacci calculation

public:
// ****************************************************************************
// *                        ******** Functions ********                       *
// ****************************************************************************
    void iotNRFCom();                               // Initializes the registers for the nRF
    bool sendCmd();                                 // Sends a message to the designated device
    bool receiveCmd();                              // Receives messages checks the sender
};

#endif  /* IOTNRFCOM_H */
