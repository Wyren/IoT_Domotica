#include "initialize.h"

Initialize::Initialize()
:addresses( {"GTinY","LTinY", "KTinY", "BTinY", "STinY"} ){

}

/**
    - Initializes the nRF chip
    - Map the ports of the arduino
    - Set some values in the struct
*/
void Initialize::init(){
    int pipe, adr;

    communication.radio.begin();
    communication.radio.setPALevel(RF24_PA_MAX);                                                            
    communication.radio.setRetries(5,12);
    communication.radio.enableAckPayload();
    communication.radio.enableDynamicPayloads();
    for (pipe = 1, adr = 0; pipe < 6; adr++, pipe++){
        communication.radio.openReadingPipe(pipe, addresses[adr]);
    }
    communication.radio.startListening();

    DDRD |=  B01111000;                                         // set the output pins
    DDRD &= ~B10000111;
    PORTD |= B00000100;

    Serial.print("Device initialized with device ID: ");
    Serial.println(communication.radioNumber);

    communication.command.deviceState.pin[PWMM] = PWM_PIN;
    communication.command.deviceState.pin[RED] = RED_RGB_PIN;
    communication.command.deviceState.pin[BLUE] = BLUE_RGB_PIN;
    communication.command.deviceState.pin[GREEN] = GREEN_RGB_PIN;

    initDone = true;
}