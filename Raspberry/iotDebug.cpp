// ****************************************************************************
// *                              iotDebug.h                              *
// *                            Author: Michel Roke                           *
// ****************************************************************************

// ****************************************************************************
// *                        ******** Includes ********                        *
// ****************************************************************************
#include "iotDebug.h"
#include "iotComMan.h"

using namespace std;
extern IotComMan deviceCom;

// ****************************************************************************
// *                ******** Debug via command prompt ********                *
// ****************************************************************************
bool IotDebug::iotDebug(){
    // Device ID, group ID, action ID, four action values;
    uint16_t inputValue[7] = {0,0,0,0,0,0,0};
    string input;

    cout << endl << "Basic module IoT Domotics!" << endl;
    cout << "Press enter to continue! Ctrl + c to exit!" << endl << endl;
    cin.get();

    /*                ******** Assembling the command **********              */
    cout << "\033[2J\033[1;1H" << endl;         // Clear the screen
    cout << " ************ Choose device ***********" << endl;
    cout << "Please type the destination device id" << endl;
    getline(cin,input);
    inputValue[0] = atoi(input.c_str());

    // Choose the group the device is in and what it needs to do
    cout << endl << "Choose the device group: '1'-'5'" << endl;
    cout << "Choose an action: '1' for power_Lamp, '2' for dim_Lamp, '3' for RGB_lamp" << endl;
    cout << "Type the numbers sequentially, no spaces!" << endl;
    getline(cin,input);                     // Read the input from the console

    // Transform the string to integer, put it in the correct containers
    // Ensure correct values
    inputValue[1] = ((input[0] - 48) == 0) ? 1 : ((input[0] - 48) < 6) ? (input[0] - 48) : 5;
    inputValue[2] = ((input[1] - 48) == 0) ? 1 : ((input[1] - 48) < 4) ? (input[1] - 48) : 3;
    // Display the typed values
    cout << endl << "Group: " << inputValue[1] << ", Action: ";
    cout << inputValue[2] << ", Device ID: " << inputValue[0] << endl << endl;

    // Check the device existence
    if(inputValue[0] != 0 && deviceCom.checkDevice(inputValue[0], (uint8_t)inputValue[1])) {
        // Switch case action     
        switch (inputValue[2]) {

            case 1: // Simple action on turning a light on and off
                cout << "Type the device switch number: '1'-'4'" << endl;
                cout << "Type '0' to turn the light off or '1' to turn the light on" << endl;
                cout << "Type the numbers sequentially, no spaces!" << endl;
                getline(cin,input);         // Read the input from the console
                inputValue[3] = ((input[0] - 48) == 0) ? 1 : ((input[0] - 48) < 5) ? (input[0] - 48) : 4;
                inputValue[4] = ((input[1] - 48) == 0) ? 0 : 1;
                break;

            case 2: // Dimmer action to change the value between 0 and 255
                cout << "Type a value between (low) '0' and '255'(high) to dim light" << endl;
                getline(cin,input);         // Read the input from the console
                inputValue[3] = atoi(input.c_str());
                break;

            case 3: // Setting the value of the RGB lamp
                cout << "Type a value between '0'(low) and '255'(high) for Red lamp" << endl;
                getline(cin,input);         // Read the input from the console
                inputValue[3] = atoi(input.c_str());

                cout << "Type a value between '0'(low) and '255'(high) for Green lamp" << endl;
                getline(cin,input);         // Read the input from the console
                inputValue[4] = atoi(input.c_str());

                cout << "Type a value between '0'(low) and '255'(high) to Blue lamp" << endl;
                getline(cin,input);         // Read the input from the console
                inputValue[5] = atoi(input.c_str());

                // For testing display the console input
                cout << "You typed: Red: " << inputValue[3] << ", Green: ";
                cout << inputValue[4] << ", Blue: " << inputValue[5] << endl; 
                break;

            default:
                cout << "Incorrect function selected\nTry again!" << endl;
                return false;
        }
        // Collect the data and fill the command array
        deviceCom.fillCmd(inputValue);
        cout << "Press enter to continue! " << endl;
        cin.get();

        return true;

    } else {
        cout << "No device found!" << endl;
        cout << "Choose one of the displayed options" << endl;

        return false;
    }

    return false;
}

// ****************************************************************************
// *                     ******** Output value **********                     *
// ****************************************************************************
void IotDebug::outputValue(uint8_t input[], uint8_t arrSize) {
    uint8_t i = 0;
    
    cout << "Values: ";
    for (i = 0; i < arrSize; ++i) {
        cout << (short)input[i] << ", ";
    }
    cout << endl;
}
