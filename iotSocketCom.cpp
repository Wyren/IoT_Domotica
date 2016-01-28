// ****************************************************************************
// *                               iotNRFCom.cpp                              *
// *                            Author: Michel Roke                           *
// ****************************************************************************

// ****************************************************************************
// *                        ******** Includes ********                        *
// ****************************************************************************
#include "iotSocketCom.h"
#include "iotComMan.h"
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern IotComMan deviceCom;
extern DeviceManagement devMan;

// ****************************************************************************
// *                                Initializer                               *
// ****************************************************************************
void IotSocketCom::iotSocketCom(){
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    rtrnVal = getaddrinfo(ADDR, PORT, &hints, &port);
    if ((sockfd = socket(port->ai_family, port->ai_socktype, port->ai_protocol)) == -1) {
        perror("client: socket");
    }

    if (connect(sockfd, port->ai_addr, port->ai_addrlen) == -1) {
        perror("client: connect");
        close(sockfd);
    }

    if (port == NULL) {
        fprintf(stderr, "client: failed to connect\n");
    }
    
}

// ****************************************************************************
// *                            Incomming messages                            *
// ****************************************************************************
void IotSocketCom::sockReceive(){
    
}

// ****************************************************************************
// *                             Sending messages                             *
// ****************************************************************************
void IotSocketCom::sockSend(){

}

// ****************************************************************************
// *                       Constructing pilight messages                      *
// ****************************************************************************
void IotSocketCom::sockMsg(){

}

// ****************************************************************************
// *                    ******** Command file read ********                   *
// ****************************************************************************
bool IotSocketCom::readCmdFile(){
    uint8_t paramCount = 0;
    bool succes = false;
    // Device ID, group ID, action ID, four action values;
    uint16_t param[7] = {0,0,0,0,0,0,0};

    // Open the communication file to read the command
    std::fstream cmdFile;
    cmdFile.open("/home/pi/nRFdomotica/communication", std::fstream::in);

    // Peek if the file is empty
    if(cmdFile.peek() != std::fstream::traits_type::eof()) {
        // Read the command values 
        for (paramCount = 0; paramCount < 7; ++paramCount) {
            cmdFile >> param[paramCount];
        }

        // Close the file in order to clean it
        cmdFile.close();

        // Assign the group ID
        devMan.group = (param[1] == 0) ? 1 : (param[1] < 6) ? (uint8_t)param[1] : 5;

        // Check the device existence
        if (deviceCom.checkDevice(param[0], devMan.group) && param[0] != 0) {
            deviceCom.fillCmd(param);
            succes = true;
        }

        // Clear the file
        cmdFile.open("/home/pi/iotDomotica/communication", std::fstream::out | std::fstream::trunc);
    }
    
    cmdFile.close();
    return succes;
}
