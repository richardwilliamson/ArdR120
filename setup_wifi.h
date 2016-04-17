#ifndef SETUP_WIFI_h
#define SETUP_WIFI_h

#include <WiFiManager.h>

IPAddress staticIP(0,0,0,0);

IPAddress consoleIP; //IP of console

WiFiManager wifiManager;

void setupWifiManager();
void updateValuesCallback(WiFiManager *manager);
void saveConfigCallback ();
void apSetupCallback (WiFiManager *manager);
void wifiLoopCallback(WiFiManager *manager);



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

void displaySetupNetworkType();
void interpretSetupNetworkType(Buttons key);
void saveSetupNetworkType();

void displaySetupIp();
void interpretSetupIp(Buttons key);
void saveSetupIp();

bool getIsDHCP();


void displaySetupServer();
void interpretSetupServer(Buttons key);



#endif

