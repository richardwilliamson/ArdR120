#ifndef SETUPMANAGER_H
#define SETUPMANAGER_H

#include "ArdR120.h"

byte setupMode = 0;
#define SETUP_MODE_AWAIT_SELECTION 255
#define SETUP_MODE_NONE 0
#define SETUP_MODE_USER 1
#define SETUP_MODE_DEV_IP 2
#define SETUP_MODE_DEV_SUBNET 3
#define SETUP_MODE_CONSOLE_IP 4
#define SETUP_MODE_WIFI_SSID 5
#define SETUP_MODE_WIFI_PASS 6


#define SETUP_MODE_UPLOAD 9


#define SETUP_STORE_VERSION 0 //17 bytes

#define SETUP_STORE_USER 17 //1 byte
#define SETUP_STORE_IP 18 //4 bytes - if 0.0.0.0 then use dhcp
#define SETUP_STORE_SUBNET 22 //4 bytes
#define SETUP_STORE_CONSOLE_IP 26 //4 bytes

#define SETUP_STORE_SSID 30 //33 bytes
#define SETUP_STORE_PASS 63 //17 bytes



void enterSetup(byte mode);
void doSetupLoop();
void displaySetup();
void exitSetup();

bool doCursorChange(Buttons key); //returns true if it's done something, false if not
bool getNumberFromKey(Buttons key, bool allowDot, bool allowClear); //updates the buffer and returns true. Returns false if didn't update
void checkCursor(); //checks the cursor is within range
void positionCursor();
bool getASCIIFromKey(Buttons key); 
void getLetterFromKey(byte number, Buttons key);


void pad(char * str, byte len);

void interpretSetupIp(Buttons key);
void displaySetupIp();
void saveSetupIp();
void readIpEEPROM();

void interpretSetupUser(Buttons key);
void displaySetupUser();
void saveSetupUser();
void readUserEEPROM();




#endif

