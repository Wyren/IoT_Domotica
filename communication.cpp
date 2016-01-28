#include <string.h>
#include "communication.h"
#include "Initialize.h"

/*  ID list
  server = 26984 - (105, 104)
  Radio 1 = 1040 - (4, 16)
  Radio 2 = 35180 - (137, 108)
  Radio 3 = 63798 - (249, 54)
  Radio 4 = 129   - (0, 129)

  SENSEO LIST
  blue wire - LED
  orange wire - On button
  brown wire - one cup
  green wire - two cups
*/

Initialize initialize;

Communication::Communication()
:radio(9,10){

}

/**
    This state machine is looped infinitly and processes the incoming data

    State 0:    Waits till a message is available and checks if it is a valid message
    State 1:    Checks if the incoming message is meant for this device and
                if its an acknowledge or a command
    State 2:    send an acknowledge to the server
    State 3:    The command is executed here
    State 4:    Sending a message which retries a couple times if it fails
    State 5:    processing the acknowledge which we received from the server
*/

void Communication::listenDevice(){    
    uint8_t pipeNumber, attempts, tempCmd;
    uint16_t deviceID, senderID;

    command.checkFlags();
    processFlags();
    if(ledStripActive){
        command.ledStrip();
    }

    switch (state){
        case 0: // WaitForMessage
            radio.startListening();
            if (radio.available(&pipeNumber)){                                                  // wait untill data is available
                Serial.println("------------------------------------------------------------");
                Serial.println("Message received");
                radio.read(message, CMD_SIZE);                                                  // reading the incoming data
                encryption(message, false);                                                     // decrypt the incoming message
                if (valid()){   
                    state = 1;                                                               // checks the validity of the message
                }                                                      
            }
            break;
        case 1: // ScanMessage
            deviceID = ((cmd[0] << 8) + cmd[1]); 
            senderID = ((cmd[7] << 8) + cmd[8]);
            Serial.print("From device: ");
            Serial.println(senderID);   
            Serial.print("To Device: ");                                              
            Serial.print(deviceID);
            Serial.print(" - ");     
            if (deviceID == radioNumber){
                if(cmd[2] == 128){
                    Serial.println("Ack received");
                    state = 5;
                }else if (cmd[2] == 127){
                    Serial.println("Send message again");
                    prepareMessage(messageCpy, false);
                    state = 4;
                }else{
                    Serial.println("Lets process this message!");
                    state = 2;
                }
            }else if(senderID == radioNumber){
                Serial.println("Ignore, its my own message");
                state = 0;
            }else{
                Serial.println("Wrong device ID, forward message");
                _delay_ms(5);
                prepareMessage(cmd, false);
                state = 4;  
            }
            break;
        case 2: //sendAcknowledge
            if(sendAcknowledge()){
                Serial.println("Acknowledge is accepted");                
                state = 3;
            }
            break;
        case 3: //ExecuteCommand
            command.executeCommand(cmd);
            Serial.println("Done");
            state = 0;
            break;
        case 4: //SendMessage
            attempts = 10 - retry;
            if(radio.write(message, CMD_SIZE)){
                Serial.println("Send!");
                retry = 0;
                state = 0;
            }else{
                Serial.println("Retry");
                Serial.print(attempts);
                Serial.println(" - Attempts left");
                retry++;
                _delay_ms(RETRY_TIME);
            }
            if (retry >= 10){
                Serial.println("Sending message Failed!");
                retry = 0;
                state = 0;
                break;
            }
            break;   
        case 5: //ProcessAcknowledge
            for (int i = 2; i < CMD_SIZE - 5; i++){
                if (messageCpy[i] == message[i]){
                    ackCounter++;
                }
            }
            if (ackCounter <= CMD_SIZE - 5){
                Serial.println("Acknowledge is accepted");
                state = 0;
            }else{
                cmd[2] = 127;
                prepareMessage(messageCpy, false);
                Serial.println("Something went wrong with the acknowledge");
                state = 4;
            }
            break;
        default: // if for some reason a case is used which is not defined
            Serial.println("Something went wrong with the statemachine!");
            break;
  }
}

/**
    An incoming message is check to see if it is the same message as received before. If so the message is ignored
*/
bool Communication::valid(){
    uint8_t validCounter = 0;
    for (uint8_t i = 0; i < CMD_SIZE - 1; i++){
        if (cmdPrev[i] == cmd[i]){
          validCounter++;
        }
        cmdPrev[i] = cmd[i];
    }
    if (validCounter >= CMD_SIZE - 1){
        Serial.println("Same message ");
        return false;
    }else{
        Serial.print("Unique message ");
        return true;
    }
}

/**
    Prepares some things which have to happen before we can send a message, this included encrypting and for some messages, changes the sender and receiver adress
    
    @param data[] The data that should be encrypted in the message
    @param external This states if the messages will change from sender to receiver
*/
void Communication::prepareMessage(uint8_t data[], bool external){
    if(external){
        cmd[7] = (radioNumber >> 8);
        cmd[8] = (radioNumber & 0xFF);
        cmd[0] = (serverNumber >> 8);
        cmd[1] = (serverNumber & 0xFF);
    }
    radio.stopListening();
    radio.openWritingPipe(initialize.addresses[0]);
    encryption(data, true);
}

/**
    The command is shifted a bit in the middle to make place for the acknowledge sign, "cmd[2] = 128"
*/
bool Communication::sendAcknowledge(){
    uint8_t cmdCopy[CMD_SIZE];
    for (int i = 0; i< CMD_SIZE; i++){
        cmdCopy[i] = cmd[i];
    }
    for (int i = 6; i < 2; i--){
        cmd[i] = cmd[i - 1];    
    }
    cmd[2] = 128;
    prepareMessage(cmd, true);
    Serial.println("Acknowledging...");
    if (radio.write(message, CMD_SIZE)){
        for (int i = 0; i< CMD_SIZE; i++){
            cmd[i] = cmdCopy[i];
        }
        return true;
    }
    return false;
}

/**
    The checksum is upped after the message is encrypted. This is to prevent the message to look the same
*/
uint8_t Communication::upChecksum(){
    checksum = first + second;
    first = second;
    second = checksum;

    return checksum;
}

/**
    Encrypts or decrypts a incoming/outgoing message
    
    @param input[] The input that should be encrypted or decrypted
    @param encrypt This boolean indicates if the message should be encrypted or decrypted
*/
bool Communication::encryption(uint8_t input[], bool encrypt){
    uint8_t xOr = 0, 
          result[CMD_SIZE];
    if (encrypt){
        upChecksum();
        cmd[9] = checksum;
        input[9] = checksum;
    }
    Serial.print("Input: ");
    for (int i = 0; i < CMD_SIZE; i++){
      Serial.print(input[i]);
      Serial.print(", ");
    }
    Serial.println(" ");
    for (uint8_t count = 0; count < (CMD_SIZE - 1); ++count){
        xOr = input[9] & (1 << count);
        if (xOr == (1 << count)){
            result[count] = input[count] ^ input[9];
            result[count] = (result[count] ^ (privateKey >> count));
        } else {
            result[count] = input[count] ^ input[9];
            result[count] = (result[count] ^ (privateKey >> (count + 1)));
        }
    }
    result[9] = input[9];
    Serial.print("Output: ");
    for (int i = 0; i < CMD_SIZE; i++){
      Serial.print(result[i]);
      Serial.print(", ");
    }
    Serial.println(" ");
    if (encrypt){
        Serial.println("Encrypting");
        memcpy(message, result, CMD_SIZE);
        return true;
    }else{
        Serial.println("Decypting");
        memcpy(cmd, result, CMD_SIZE);
        return true;
    }
}

/**
    Whenever a flag is changing, this function will respond and send a message to the server
*/
void Communication::processFlags(){
    if (command.flagReadySenseo){
        cmd[2] = 32;
        cmd[3] = 'S';
        prepareMessage(cmd, true);
        for (int i = 0; i < CMD_SIZE; i++){
            messageCpy[i] = cmd[i];
        }
        if (command.flagReadySenseo && !command.flagWaitSenseo){
            command.flagReadySenseo = false;
            Serial.println("Senseo is warmed up");
            state = 4;
        }
    }

    if (command.flagDoorBell){
        cmd[2] = 32;
        cmd[3] = 'D';
        prepareMessage(cmd, true);
        for (int i = 0; i < CMD_SIZE; i++){
            messageCpy[i] = cmd[i];
        }
        Serial.println("Someone at the door");
        command.flagDoorBell = false;
        state = 4;
      }
}
