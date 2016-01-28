// ****************************************************************************
// *                               iotNRFCom.cpp                              *
// *                            Author: Michel Roke                           *
// ****************************************************************************

/**
 * The communication between the nRF modules and the Raspberry is handled here.
 *
 */

// ****************************************************************************
// *                        ******** Includes ********                        *
// ****************************************************************************
#include "iotNRFCom.h"

using namespace IotNetwork;
extern DeviceManagement devMan;

// ****************************************************************************
// *                 ******** Hardware configuration ********                 *
// ****************************************************************************
    // Configure the appropriate pins for your connections
    // Radio CE Pin, CSN Pin, SPI Speed
    // See http://www.airspayce.com/mikem/bcm2835/group__constants.html#ga63c029bd6500167152db4e57736d0939 and the related enumerations for pin information.

    // Set-up for GPIO 15 CE and GPIO 24 CSN with SPI Speed @ 8Mhz
    RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);


// ****************************************************************************
// *             ******** Initializing the radio module **********            *
// ****************************************************************************
void IotNRFCom::iotNRFCom() {
    radio.begin();                          // Initialize the nRF
    radio.setPALevel(RF24_PA_MAX);          // Set output power to maximum
    radio.setRetries(10,15);                // Set the amount of retries en the delay between each try.
    radio.enableAckPayload();               // Allow optional acknowledge payloads
    radio.enableDynamicPayloads();          // Dynamic payload means smaller messages
    radio.startListening();                 // Start listening for messages

    // Set the addresses to the data pipes leaving pipe 0 for sending.
    uint8_t pipe;
    for(pipe = 1; pipe < 6; ++pipe) {       // Use pipe -1 for the correct address
        radio.openReadingPipe(pipe, addresses[(pipe - 1)]);
    }
    publicKey = 144;                        // 55 + 89, starting values at raisePublicKey()
}

// ****************************************************************************
// *                   ******** Send the message **********                   *
// ****************************************************************************
bool IotNRFCom::sendCmd() {             
    // Stop listening
    radio.stopListening();

    // Create a new public key
    IotNRFCom::raisePublicKey();
    devMan.command[9] = publicKey;

    // Encrypt message using the public key
    IotNRFCom::encryption(devMan.command, true);

    // Set the transmission to the correct address
    radio.openWritingPipe(addresses[(devMan.group -1)]);

    // Radio gives a true if message is received
    if(radio.write(devMan.message, CMD_SIZE)) {
        // Start listening
        radio.startListening();
        return true;
    }
    // Nobody received the message
    return false;
}

// ****************************************************************************
// *                  ******** Listen for messages **********                 *
// ****************************************************************************
bool IotNRFCom::receiveCmd() {
    uint16_t deviceIDCheck = 0;

    // Read the message
    radio.read(devMan.message, CMD_SIZE);

    // Decrypt the message and see if it is a new message
    if (IotNRFCom::encryption(devMan.message, false)){
        // For whom is the message
        deviceIDCheck = (devMan.command[0] << 8) + devMan.command[1];

        // Message is not for this device
        if (radioNumber != deviceIDCheck) {
            // Send message without feedback
            IotNRFCom::sendCmd();
            // No new message
            return false;
        }
        // Message received
        return true;
    }
    // Received last sent message
    return false;
}

// ****************************************************************************
// *           ******** Encryption/decryption of messages **********          *
// ****************************************************************************
bool IotNRFCom::encryption(uint8_t input[], bool encrypt) {
    uint8_t
        count = 0,
        valid = 0,
        xOr = 0,
        result[CMD_SIZE];

    // Encrypting/Decrypting all but the public key
    for (count = 0; count < (CMD_SIZE - 1); ++count) {
        // Creating a small randomness for the encryption
        xOr = input[9] & (1 << count);

        // Add a second random factor by using the public key to XOR the message
        if (xOr == (1 << count)){
            result[count] = input[count] ^ input[9];
            result[count] = (result[count] ^ (privateKey >> count));
        } else {
            result[count] = input[count] ^ input[9];
            result[count] = (result[count] ^ (privateKey >> (count + 1)));
        }
    }

    // Add the public key to the results
    result[9] = input[9];

    // Check boolean to determine result destination
    if (encrypt) {
        memcpy(devMan.message, result, CMD_SIZE);
        return true;
    }
    if (!encrypt) {
        // Check if the message came from here
        for (count = 0; count < 9; ++count) {
            if (devMan.command[count] == result[count]) {
                ++valid;
            }
        }
        // If the message did not, process it
        if (valid != 9) {
            memcpy(devMan.command, result, CMD_SIZE);
            return true;
        }
    }

    // The message came from here
    return false;
}

// ****************************************************************************
// *                 ******** Change the public key **********                *
// ****************************************************************************
void IotNRFCom::raisePublicKey() {
    static uint8_t      // The variables are static to retain their value
        fibonacciOne = 55,
        fibonacciTwo = 89;

    // Use the Fibonacci method to generate a new key
    fibonacciOne = fibonacciTwo;
    fibonacciTwo = publicKey;
    publicKey = fibonacciOne + fibonacciTwo;
}
