#ifndef SETUPMANAGER_H
#define SETUPMANAGER_H

#include "ArdR120.h"

byte setupMode = 0;
#define SETUP_MODE_AWAIT_SELECTION 255
#define SETUP_MODE_NONE 0

#define SETUP_MODE_INFO 1

#define SETUP_MODE_USER 2
#define SETUP_MODE_CONSOLE_IP 3

#define SETUP_MODE_NETWORK_TYPE 4
#define SETUP_MODE_DEV_IP 5
#define SETUP_MODE_DEV_SUBNET 6
#define SETUP_MODE_DEV_GATEWAY 7

#define SETUP_MODE_WIFI_SSID 8
#define SETUP_MODE_WIFI_PASS 9

#define SETUP_MODE_ACCESSPOINT 10

#define SETUP_MODE_UPLOAD 11

#define SETUP_MODE_ENABLE_SERVER 12

#define SETUP_STORE_VERSION 0 //17 bytes

#define SETUP_STORE_USER 17 //1 byte
#define SETUP_STORE_IP 18 //4 bytes - 0 if DHCP
#define SETUP_STORE_GATEWAY 22 //4 bytes
#define SETUP_STORE_SUBNET 26 //4 bytes

#define SETUP_STORE_CONSOLE_IP 30 //4 bytes


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

void displaySetupInfo();
void interpretSetupInfo(Buttons key);

void interpretSetupUser(Buttons key);
void displaySetupUser();
void saveSetupUser();
byte readUserEEPROM();
void writeUserEEPROM(byte user);


#endif


