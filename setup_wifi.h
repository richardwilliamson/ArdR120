#ifndef SETUP_WIFI_h
#define SETUP_WIFI_h

IPAddress staticIP(0,0,0,0);

IPAddress consoleIP; //IP of console

void interpretSetupWifi(Buttons key);
void displaySetupWifi();
void saveSetupWifi();

void interpretSetupWifiPassword(Buttons key);
void displaySetupWifiPassword();
void saveSetupWifiPassword();


void connectWifi();

void enableAccessPoint();


void updateIP(IPAddress &ip, byte storeAddr);
void readIP(IPAddress &ip, byte storeAddr);

void updateEEPROMStaticIP(IPAddress gateway, IPAddress subnet);
void updateEEPROMDhcpIP();
void updateEEPROMConsoleIP();

void readEEPROMIP(IPAddress &gateway, IPAddress &subnet);
void readEEPROMConsoleIP();



void interpretSetupIp(Buttons key);
void displaySetupIp();
void saveSetupIp();

bool getIsDHCP();





#endif

