#ifndef SETUP_WIFI_h
#define SETUP_WIFI_h



void interpretSetupWifi(Buttons key);
void displaySetupWifi();
void saveSetupWifi();

void interpretSetupWifiPassword(Buttons key);
void displaySetupWifiPassword();
void saveSetupWifiPassword();


void readWifiEEPROM();
void connectWifi();


char ssid[33];  //  your network name (SSID)
char pass[17];       // your network password


#endif
