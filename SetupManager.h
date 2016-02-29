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


void doSetupLoop();
void displaySetup();
void exitSetup();

void interpretSetupUser(Buttons key);
void displaySetupUser();
void saveSetupUser();

bool doCursorChange(Buttons key); //returns true if it's done something, false if not
bool getNumberFromKey(Buttons key, bool allowDot, bool allowClear); //updates the buffer and returns true. Returns false if didn't update
void checkCursor(); //checks the cursor is within range

void pad(char * str, byte len);

void interpretSetupIp(Buttons key);
void displaySetupIp();
void saveSetupIp();

void interpretSetupWifi(Buttons key);
void displaySetupWifi();
void saveSetupWifi();



#endif

