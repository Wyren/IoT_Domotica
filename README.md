# IoT_Domotica - Raspberry program

Intern project for further development

Install guide:
* To use the nRF, the libriary of TMRh20 will be used.
* Type the following commands in the console to install:
 * sudo apt-get install git
 * sudo git clone https://github.com/tmrh20/RF24.git RF24
 * cd RF24
 * sudo make install
* To install the nRF program type in the following commands:
 * sudo git clone -b Raspberry_nRF https://github.com/Wyren/IoT_Domotica.git nRFdomotica
 * cd nRFdomotica
 * sudo make
* To run the program type in:
 * sudo ./iotMain
