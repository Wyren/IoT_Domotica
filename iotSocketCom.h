// ****************************************************************************
// *                              iotSocketCom.h                              *
// *                            Author: Michel Roke                           *
// ****************************************************************************
#ifndef IOTSOCKETCOM_H
#define IOTSOCKETCOM_H

#define PORT "5050"         // The port the client will be connecting to 
#define ADDR "127.0.0.1"    // The ip the client will be connecting to 
#define MAXDATASIZE 1024    // Max number of bytes we can get at once 

// ****************************************************************************
// *                        ******** Includes ********                        *
// ****************************************************************************
#include "iotInit.h"
#include <netdb.h>

class IotSocketCom {
private:
// ****************************************************************************
// *                                 Variables                                *
// ****************************************************************************
    int 
        sockfd,
        rtrnVal;
    struct addrinfo 
        hints,
        *port;
    char buff[MAXDATASIZE];

// ****************************************************************************
// *                        ******** Functions ********                       *
// ****************************************************************************
    void sockReceive();
    void sockSend();
public:
// ****************************************************************************
// *                        ******** Functions ********                       *
// ****************************************************************************
    bool readCmdFile();         // Reads the communication to which pilight can send commands
    void iotSocketCom();
    void sockMsg();
};

#endif /* IOTSOCKETCOM_H */
