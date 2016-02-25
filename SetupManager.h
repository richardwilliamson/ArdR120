#ifndef SETUPMANAGER_H
#define SETUPMANAGER_H

#include "ArdR120.h"

byte setupMode = 0;
#define SETUP_MODE_AWAIT_SELECTION 255
#define SETUP_MODE_NONE 0
#define SETUP_MODE_USER 1

#define SETUP_MODE_UPLOAD 9


void doSetupLoop();
void displaySetup();
void exitSetup();

void interpretSetupUser(Buttons key);
void displaySetupUser();
void saveUserSetup();

#endif

