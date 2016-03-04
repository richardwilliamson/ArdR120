#include "setup_wifi.h"
#include <EEPROM.h>


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
void saveSetupWifi()
{
  //put the new SSID into our global ssid variable, don't save to EEPROM yet though as this allows us to roll back if we need to
  WiFi.SSID(cursorPosition).toCharArray(ssid, 32);
  
  //TODO - NEED TO ACTUALLY SAVE THE THING!!!
  Serial.write(SND_CLEAR);
  Serial.print("Wifi Changed to:");
  SND_NEW_LINE;
  Serial.print(ssid);
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

    //needto save as no password somehow?
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
//  Serial.print("GOT ");
//  Serial.print(i);
//  Serial.print("it's");
  //TODO - NEED TO ACTUALLY SAVE THE THING!!!
  //strip the 
  
  strncpy(pass, inputBuffer, i+1); //just copy the length of up to last space so we don't save all the spaces
  pass[i+1]='\0';
  
  //test the connection - if we don't succeed then restore the old settings

  connectWifi();

  if (WiFi.status()!=WL_CONNECTED)
  {
      Serial.print("Wifi connection failed");
      readWifiEEPROM(); //this resets our old settings 
      delay(1000);
      updateScreenSetup = true;
      return;
  }

  Serial.print("saving");
  
  //now save to EEPROM
  EEPROM.begin(512); // ssid is 33, pass is 17
  EEPROM.put(SETUP_STORE_SSID, ssid);
  EEPROM.put(SETUP_STORE_PASS, pass);
  EEPROM.commit();
  EEPROM.end();
  Serial.print("saved");
  delay(1000);

  //connect to the network and console (verbose);
  doConnect(true);

  exitSetup();

}

void readWifiEEPROM()
{
  EEPROM.begin(512);
  EEPROM.get(SETUP_STORE_SSID, ssid);
  EEPROM.get(SETUP_STORE_PASS, pass);
  EEPROM.commit();
  EEPROM.end();
}

void connectWifi()
{
  //WiFi.disconnect();
  WiFi.begin(ssid, pass);
  byte tries = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(10);
    tries++;
    if (tries > 5) {
      break;
    }
  }
}

