#include "setup_wifi.h"
#include <EEPROM.h>
#include <WiFiManager.h>


IPAddress subnetIP; //used for temp holding of info

void connectWifi()
{
  byte tries = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(10);
    tries++;
    if (tries > 5) {
      break;
    }
  }

  //now look for whether we have an IP address
  tries = 0;
  while (WiFi.localIP()==IPAddress(0,0,0,0))
  {
      delay(10);
      tries ++;
      if (tries>10)
        break;
  }
  
}
bool haveReadIP = false;

bool getIsDHCP()
{
  //should check if we have read in IP addresses
  //if not read them in
  //if we have return the result
  DEBUG_PRINT("start to read");
  if (!haveReadIP)
  {
      EEPROM.begin(512);
      readIP(staticIP, SETUP_STORE_IP);
      EEPROM.end();
      DEBUG_PRINT("static is:");
      DEBUG_PRINT(staticIP);
      DEBUG_DELAY;
      haveReadIP=true;
  }
  DEBUG_PRINT("DONE DAT");
  return (staticIP[0]==0);
  
}
//***************************************************************************
//*********** ON DEVICE WIFI/IP CONFIG **************************************
//***************************************************************************

void displaySetupWifi()
{
  Serial.print("Select Wifi Network");
  SND_NEW_LINE;

  if (currentBufferLength == 0)
  {
    //assume not got anything yet..
    currentBufferLength = WiFi.scanNetworks();
    cursorPosition = 0;
  }

  //display the selected network name

  Serial.print(WiFi.SSID(cursorPosition));


}
void interpretSetupWifi(Buttons key)
{

  if (doCursorChange(key)) //move the cursor
  {
    updateScreenSetup = true;
    return;
  }

  if (key == BTN_ENTER)
  {
    saveSetupWifi();
    //save
  }

}
char ssidBuffer[33];
void saveSetupWifi()
{
  //put the new SSID into our global ssid variable, don't save to EEPROM yet though as this allows us to roll back if we need to
  WiFi.SSID(cursorPosition).toCharArray(ssidBuffer, 32);
  
  //TODO - NEED TO ACTUALLY SAVE THE THING!!!
  Serial.write(SND_CLEAR);
  Serial.print("Wifi Changed to:");
  SND_NEW_LINE;
  Serial.print(ssidBuffer);
  SND_NEW_LINE;
  
  if (WiFi.encryptionType(cursorPosition) != ENC_TYPE_NONE)
  {
    Serial.print("Password required");
    setupMode = SETUP_MODE_WIFI_PASS;
    //TODO NEED TO SAVE THE SSID SOMEWHERE WHILE WE GET THE PASSWORD, BUT NOT OVERWRITE OLD SSID - MAYBE PUT ON FLASH TO BE SAFE?
    cursorPosition = 0;
    currentBufferLength = 0; //set to zero so the next stage knows it needs changing..
    updateScreenSetup = true;
  } else
  {
    Serial.print("open network");
    WiFi.begin(WiFi.SSID(cursorPosition).c_str());
    delay(1500);
    exitSetup();
  }

  delay(1500);
}
void displaySetupWifiPassword()
{
  Serial.print("Enter password for");
  SND_NEW_LINE;
  Serial.print("-- show SSID -- "); //truncate to 16 chars though!
  SND_NEW_LINE;

  //should we save past wifi network info somehow??
  if (currentBufferLength == 0)
  {
    //set up input buffer
    currentBufferLength = 17;
    cursorPosition = 0;
    lastKey = BTN_NONE;
    strcpy(inputBuffer, "                "); //empty buffer
  }
  Serial.print(inputBuffer);
  positionCursor();


}
void interpretSetupWifiPassword(Buttons key)
{
  if (getASCIIFromKey(key))
  {
    updateScreenSetup = true;
    lastKey = key;

    return;
  }

  if (doCursorChange(key)) //move the cursor
  {
    updateScreenSetup = true;
    lastKey = key;
    return;
  }

  lastKey = key;

  if (key == BTN_ENTER)
  {
    saveSetupWifiPassword();
    //save
  }

}
void saveSetupWifiPassword()
{
  Serial.write(SND_CLEAR);
//  Serial.print(inputBuffer);

  //strip spaces off the end
  byte i = currentBufferLength;
  for (i; i>0; i--)
  {
     if (inputBuffer[i]!=' ' && inputBuffer[i]!=0)
        break;
  }

  char pass[17];
  
  strncpy(pass, inputBuffer, i+1); //just copy the length of up to last space so we don't save all the spaces
  pass[i+1]='\0';
  
  WiFi.begin(ssidBuffer, pass); 
  connectWifi();

  //TODO need to restore settings somehow??

  if (WiFi.status()!=WL_CONNECTED)
  {
      Serial.print("Wifi connection failed");
      updateScreenSetup = true;
      return;
  }

  Serial.print("saving");
  
  //now save to EEPROM

  //connect to the network and console (verbose);
  doConnect(true);

  exitSetup();

}


void displaySetupNetworkType()
{
  //prompt for type DHCP or static IP
  if (currentBufferLength!=2)
  { //set to 2 now we have initialised - this means we scroll the two options..
    currentBufferLength = 2;
    cursorPosition = (getIsDHCP())?0:1; //if cursor is 0 we are DHCP, otherwise static
    if (getIsDHCP())
      DEBUG_PRINT("DHCP");
    #ifdef DEBUG
      delay(1000);
    #endif
  }

  Serial.print("Network type:");
  SND_NEW_LINE;
  if (cursorPosition==0)
    Serial.print("DHCP");
  else
    Serial.print("Static");
  
}


void interpretSetupNetworkType(Buttons key)
{
  //just allow us to scroll up/down through static and DHCP
  if (doCursorChange(key))
  {
    updateScreenSetup = true;
    return;
  }
  
  //must have something else!
  switch (key)
  {
    case BTN_ENTER:
      //DO SAVE
      //***********
      saveSetupNetworkType();
      return;
      break;

  }

  //have had nothing!!
}

void saveSetupNetworkType()
{
  //if we have selected DHCP then we set the static IP to 0.0.0.0 and save, then restart networking
  //if not we set our setup mode to static IP, that will then follow through to subnet and gateway

  if (cursorPosition == 0)
  {
     //DHCP
     updateEEPROMDhcpIP();
     wifi_station_dhcpc_start(); //switches over to DHCP..
     Serial.write(SND_CLEAR);
     Serial.print("Network set to DHCP");
     delay(1500);
     exitSetup();
     
  } else
  {
     setupMode = SETUP_MODE_DEV_IP;
     cursorPosition = 0;
     currentBufferLength = 0;
     updateScreenSetup = true;
  }
}


//*************************************************************
//***** WEB BASED WIFI SETUP STUFF
//*************************************************************


WiFiManagerParameter userParm("user", "user or 0", "", 4);
WiFiManagerParameter consoleParm("console", "console IP", "", 17);

void setupWifiManager()
{
   
   #ifndef debug
      wifiManager.setDebugOutput(false);
   #endif
   
   wifiManager.setConnectTimeout(5); //the time to wait when trying to re-connect Wifi before giving up
   
   wifiManager.setForceStaticIPconfig(true); //whether or not we are static we want to display the boxes for it
   wifiManager.setDisplayExistingCreds(true); //pre-populates the boxes with the current network info..
   wifiManager.setDisplayUploadOption(true); //allows the user to upload new firmware over web

   wifiManager.addParameter(&userParm);
   wifiManager.addParameter(&consoleParm);

   wifiManager.setDisplaySettingsPageCallback(updateValuesCallback);
   wifiManager.setSaveConfigCallback(saveConfigCallback); //will set the doSave flag to true so we know to save changed
   wifiManager.setLoopCallback(wifiLoopCallback);

  
}

void wifiLoopCallback(WiFiManager *manager)
{
     Buttons key = getKeyPress();

     if (key==BTN_REM_DIM)
        wifiManager.setTimeout(1); //close the portal

}
void apSetupCallback (WiFiManager *manager)
{
    if (!getIsDHCP())  //currently have a static IP address..
        wifiManager.setSTAStaticIPConfig(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask());
    else
        wifiManager.setSTAStaticIPConfig();
}

//this is called before the settings page is displayed so we can update everything that needs updating..
void updateValuesCallback(WiFiManager *manager)
{


    String userString = String(EosOscManager::getInstance()->getUser());
    userParm.setDefaultValue(userString.c_str());
    consoleParm.setDefaultValue(consoleIP.toString().c_str());

    //hmm, this is called twice, which isn't perfect. 
    if (!getIsDHCP())  //currently have a static IP address..
        wifiManager.setSTAStaticIPConfig(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask());
    else
        wifiManager.setSTAStaticIPConfig();
}

//this simply gets called by WiFiManager, and indicated that settings need to be stored..
void saveConfigCallback () {
   //save the relevant information
       //change the relevant bits
       //note that SSID and Password are stored by the device so we don't store those

       byte newUser = atoi(userParm.getValue()); //should we be doing some kind of error checking??
       if (newUser!=EosOscManager::getInstance()->getUser())
       {  //only update and write if we haven't changed it
          writeUserEEPROM(newUser);
            //if ok display OK & ret true
          EosOscManager::getInstance()->setUser(newUser);
    
       }
       
       IPAddress newOutIP;
       newOutIP.fromString(consoleParm.getValue()); //error checking??
       consoleIP = newOutIP;
       updateEEPROMConsoleIP();

       boolean isStatic = wifiManager.getSTAIsStaticIP();
       if (isStatic)
       {
            //set static flag true
            staticIP = WiFi.localIP();
            IPAddress newGateway = WiFi.gatewayIP();
            IPAddress newSubnet = WiFi.subnetMask();
            updateEEPROMStaticIP(newGateway, newSubnet);
       } else
       {
            //set static flag false
            updateEEPROMDhcpIP();
       }
       
}

//start up as an access point
void enableAccessPoint()
{
   String ssidS = + "ArdR120-"+String(ESP.getChipId());
   const char* ssid = ssidS.c_str();
   const char* pass = "12345678";
   Serial.write(SND_CLEAR);
   Serial.print("Access point started");
   SND_NEW_LINE;
   Serial.print("SSID: ");
   Serial.print(ssid);
   SND_NEW_LINE;
   Serial.print("PASS: ");
   Serial.print(pass);
   //TODO - the above will be too long for the lines, should we shrink somehow?
   
   wifiManager.setTimeout(0); //in case this was set to something else before..

   wifiManager.startConfigPortal(ssid, pass); //this starts the portal then blocks until done - when done it tries to connect and returns true if connected

   if (WiFi.status()==WL_CONNECTED){
      Serial.write(SND_CLEAR);
      Serial.print("Connected to:");
      SND_NEW_LINE;
      Serial.print("SSID: "+WiFi.SSID());
      SND_NEW_LINE;
      if (wifiManager.getSTAIsStaticIP())
        Serial.print("S"); //prepend the IP with either S for static
      else
        Serial.print("D"); //or D for dynamic
        
      Serial.print(WiFi.localIP());
    
      delay(1000);
      exitSetup();

   } else
   { //failed to connect - what should we do - general question!
      Serial.write(SND_CLEAR);
      Serial.print("Failed to connect to network");
      SND_NEW_LINE;
      Serial.print("SSID: "+WiFi.SSID());
      SND_NEW_LINE;
      Serial.print(WiFi.localIP());
      //todo do something??
      delay(1000);
      exitSetup();
      
   }
   
}

void updateIP(IPAddress &ip, byte storeAddr)
{
  for (byte i=0; i<4; i++)
    EEPROM.put(storeAddr+i, ip[i]);
}
void readIP(IPAddress &ip, byte storeAddr)
{
  for (byte i=0; i<4; i++)
    EEPROM.get(storeAddr+i, ip[i]);
}

void readEEPROMIP(IPAddress &gateway, IPAddress &subnet)
{
    if (!getIsDHCP()) //only read info if we are not DHCP..
    {
        EEPROM.begin(512);
        readIP(gateway, SETUP_STORE_GATEWAY);
        readIP(subnet, SETUP_STORE_SUBNET); 
        EEPROM.end();  
    }
}

void readEEPROMConsoleIP()
{
  EEPROM.begin(512);
  readIP(consoleIP, SETUP_STORE_CONSOLE_IP);
  EEPROM.end();
}
void updateEEPROMStaticIP(IPAddress gateway, IPAddress subnet)
{
   EEPROM.begin(512);
   //TODO - error checking..
   updateIP(staticIP, SETUP_STORE_IP);
   updateIP(gateway, SETUP_STORE_GATEWAY);
   updateIP(subnet, SETUP_STORE_SUBNET);
   EEPROM.commit();
   EEPROM.end();
}

void updateEEPROMDhcpIP()
{
  //just set the main IP to 0;
  staticIP = IPAddress(0,0,0,0);
  EEPROM.begin(512);
  updateIP(staticIP, SETUP_STORE_IP);
  EEPROM.commit();
  EEPROM.end();
  
}

void updateEEPROMConsoleIP()
{
  EEPROM.begin(512);
  updateIP(consoleIP, SETUP_STORE_CONSOLE_IP);
  EEPROM.commit();
  EEPROM.end();
}

void interpretSetupIp(Buttons key)
{
  //interpret what to do in the IP setup
  //this could be the device IP, the device gateway, or the console IP
  //if device IP then initially need to prompt as to whether the user wants to use DHCP This should happen before we get here though?
  //otherwise..
  //if REMDIM then exit (not save) (HANDLED BEFORE THIS - THIS IS ALL WAY THRU SETUP
  //if ENTER then save and exit
  //if CLR then clear the current cursor position
  //IF UP then increment cursor
  //IF DOWN then decrement cursor

  currentBufferLength = 15; //(192.168.254.254 = 15 - lucky it just fits on a line )

  if (doCursorChange(key)) //move the cursor
  {
    updateScreenSetup = true;
    return;
  }
  if (getNumberFromKey(key, true, true)) //allow clear, allow dot
  { updateScreenSetup = true;
    return;
  }
  //must have something we aren't expecting!

  switch (key)
  {
    case BTN_ENTER:
      //DO SAVE
      //***********
      saveSetupIp();
      return;
      break;

  }

}
void displaySetupIp()
{
  switch (setupMode)
  {
    case SETUP_MODE_DEV_IP:
      Serial.print("Device IP:");
      break;
    case SETUP_MODE_DEV_SUBNET:
      Serial.print("Device Subnet:");
      break;
    case SETUP_MODE_CONSOLE_IP:
      Serial.print("Console IP");
      break;
    case SETUP_MODE_DEV_GATEWAY:
      Serial.print("Device gateway:");
      break;
  }
  SND_NEW_LINE;

  if (currentBufferLength == 0 || strlen(inputBuffer)==0) //if nothing in the buffer convert the current relevant IP to buffer
  {
    currentBufferLength = 15;
    inputBuffer[0] = '\0';
    IPAddress ip;
    switch (setupMode)
    {
        case SETUP_MODE_CONSOLE_IP:
          ip = consoleIP;
          break;
        case SETUP_MODE_DEV_IP:
          ip = WiFi.localIP();
          break;
        case SETUP_MODE_DEV_SUBNET:
          ip = WiFi.subnetMask();
          break;
        case SETUP_MODE_DEV_GATEWAY:
          ip = WiFi.gatewayIP();
          break;
    }
    //could use the toString() method of IPAddress but then wouldn't get the padding..
    for (byte i = 0; i < 4; i++)
    {
      char buf[4]; //XXX plus /0
      sprintf(buf, "%i", ip[i]);
      pad(buf, 3);
      strcat(inputBuffer, buf);
      if (i < 3)
        strcat(inputBuffer, "."); //but not the last one!
    }

  }

  Serial.print(inputBuffer);
  positionCursor();
}

void saveSetupIp()
{
  //TODO use the IPAddress class to do the work for us - but how does error checking work??
  Serial.write(SND_CLEAR);
  //split the string into it's bits
  bool error = false;
  char * pch;
  char tempBuffer[sizeof(inputBuffer)];
  strcpy(tempBuffer, inputBuffer);
  pch = strtok (tempBuffer, ".");
  byte i = 0;
  byte res[4]; //ready to recieve -
  while (pch != NULL)
  {
    if (i < 4)
    {
      int number = atoi(pch);

      if ((setupMode==SETUP_MODE_DEV_SUBNET) || (number < 254 && number > 0))
      {
        //ok for now
        if (i == 0 && number == 0)
        { //first bit can't be 0
          error = true;
          break;
        }
        res[i] = number;
      } else
      {
        error = true;
        break;
      }
    } else
    { //got more than four segments
      error = true;
      break;
    }
    //get the next slice
    pch = strtok (NULL, " ,.-");
    i++;
  }


  if (i != 4)
    error = true;

  if (!error)
  {
    //good to save
    IPAddress ip = (IPAddress)res;

    switch (setupMode)
    {
        case SETUP_MODE_CONSOLE_IP:
          consoleIP = res;
          updateEEPROMConsoleIP();
          Serial.print("console IP changd");
          SND_NEW_LINE;
          Serial.print(consoleIP);
          break;
        case SETUP_MODE_DEV_IP:
          staticIP = res;
          setupMode = SETUP_MODE_DEV_SUBNET; //so subnet next
          currentBufferLength = 0;
          cursorPosition = 0;
          updateScreenSetup = true;
          return;
          break;
        case SETUP_MODE_DEV_SUBNET:
          subnetIP = res; //temp store it..
          setupMode = SETUP_MODE_DEV_GATEWAY;
          currentBufferLength = 0;
          cursorPosition = 0;
          updateScreenSetup=true;
          return;
          break;
        case SETUP_MODE_DEV_GATEWAY:
          updateEEPROMStaticIP(res, subnetIP);
          WiFi.config(staticIP, res, subnetIP);
          Serial.print("static IP set");
          SND_NEW_LINE;
          Serial.print(staticIP);
          SND_NEW_LINE;
          Serial.print(subnetIP);
          break;
    }
    //hmmm


    //TODO restart networking and save changes to flash
    delay(1500);
    exitSetup();

  } else
  {
    Serial.print("Error changing IP");
    delay(1500);
    updateScreenSetup = true;
  }

}


