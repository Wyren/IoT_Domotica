// ****************************************************************************
// *                                 main.cpp                                 *
// *                            Author: Michel Roke                           *
// ****************************************************************************

#include "iotInit.h"
#include "iotComMan.h"

using namespace std;

IotComMan deviceCom;
// ****************************************************************************
// *           ******************* Main function *******************          *
// ****************************************************************************
int main(int argc, char* argv[]){

    char param = (argc > 1) ? argv[1][1] : 'n';

    deviceCom.cmdDevice(param);

    return 1;
} //main
