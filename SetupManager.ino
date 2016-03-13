#include "SetupManager.h"
#include "UploadManager.h"
#include "setup_wifi.h"

#include <EEPROM.h>

bool updateScreenSetup = false;

char inputBuffer[17]; //max size of input buffer
byte cursorPosition;
byte currentBufferLength;

void enterSetup(byte mode)
{
  setupMode = mode;
  updateScreenSetup = true;
}
void doSetupLoop()
{

  if (setupMode != SETUP_MODE_AWAIT_SELECTION)
  {
    if (updateScreenSetup)
      displaySetup();
  }

  Buttons key = getKeyPress();
  if (key == BTN_REM_DIM)
  {
    exitSetup();
    return;
  }
  if (key != BTN_NONE)
    interpretSetupCmd(key);

  yield();

  client.flush();
}


void interpretSetupCmd(Buttons key)
{
  if (setupMode != SETUP_MODE_AWAIT_SELECTION)
  {
    switch (setupMode)
    {
      case SETUP_MODE_INFO:
        interpretSetupInfo(key);
        break;
      case SETUP_MODE_USER:
        interpretSetupUser(key);
        break;
      case SETUP_MODE_NETWORK_TYPE:
        interpretSetupNetworkType(key);
        break;
      case SETUP_MODE_DEV_IP:
      case SETUP_MODE_DEV_SUBNET:
      case SETUP_MODE_DEV_GATEWAY:
      case SETUP_MODE_CONSOLE_IP:
        //send all to same
        interpretSetupIp(key);
        break;
      case SETUP_MODE_WIFI_SSID:
        interpretSetupWifi(key);
        break;
      case SETUP_MODE_WIFI_PASS:
        interpretSetupWifiPassword(key);
        break;
    }
    return;
  }

  //so we have pressed @ATT twice..
  Serial.print("SETUP-RDIM=EXIT");
  //no setup mode yet

  if (strlen(inputBuffer) > 0)
  {
    currentBufferLength = 0;
    strcpy(inputBuffer, ""); //empty out the input buffer

  }
  cursorPosition = 0;

  switch (key)
  {
    case BTN_1:
      setupMode = SETUP_MODE_INFO;
      updateScreenSetup = true;
      break;
    case BTN_2:   
      setupMode = SETUP_MODE_USER;
      updateScreenSetup = true;
      break;
    case BTN_3:
      setupMode = SETUP_MODE_CONSOLE_IP;
      updateScreenSetup = true;
      break;
    case BTN_4:
      setupMode = SETUP_MODE_NETWORK_TYPE;
      updateScreenSetup = true;
      break;
    case BTN_5:
      setupMode = SETUP_MODE_WIFI_SSID;
      updateScreenSetup = true;
      break;
    case BTN_6:
      setupMode = SETUP_MODE_ACCESSPOINT;
      updateScreenSetup = true;
      break;
    case BTN_9:
      setupMode = SETUP_MODE_UPLOAD; //not really needed but..
      enterUploadMode();
      break; //this puts us in an endless loop so we will never come back!
    case BTN_CLR:
    case BTN_REM_DIM:
      exitSetup();
      break;
  }

}
void displaySetup()
{
  updateScreenSetup = false;
  //clear screen

  Serial.write(SND_CLEAR);

  //line 1: SETUP -
  Serial.print("SETUP-RDIM=EXIT");
  SND_NEW_LINE;
  switch (setupMode)
  {
    case SETUP_MODE_INFO:
      displaySetupInfo();
      break;
    case SETUP_MODE_USER:
      displaySetupUser();
      break;
    case SETUP_MODE_NETWORK_TYPE:
      displaySetupNetworkType();
      break;
    case SETUP_MODE_DEV_IP:
    case SETUP_MODE_DEV_SUBNET:
    case SETUP_MODE_CONSOLE_IP:
    case SETUP_MODE_DEV_GATEWAY:
      displaySetupIp();
      break;
    case SETUP_MODE_WIFI_SSID:
      displaySetupWifi();
      break;
    case SETUP_MODE_WIFI_PASS:
      displaySetupWifiPassword();
      break;
    case SETUP_MODE_ACCESSPOINT:
      enableAccessPoint();
      break;
      
  }
}

void exitSetup()
{
  setupMode = SETUP_MODE_NONE;
  EosOscManager::getInstance()->setScreenNeedsUpdate(true);
}

void displaySetupInfo()
{
  currentBufferLength = 3;
  switch (cursorPosition)
  {
     case 0:
        //Wifi network and whether connected
        Serial.print("SSID: ");
        Serial.print(WiFi.SSID());
        SND_NEW_LINE;
        if (WiFi.status()==WL_CONNECTED)
           Serial.print("connected");
        else
           Serial.print("not connected");
        break;
     case 1:
        //console IP and user
        Serial.print("user: "); //6 chars
        Serial.print(EosOscManager::getInstance()->getUser()); //<=3 = <=9
        SND_NEW_LINE;
        Serial.print("C:");
        Serial.print(consoleIP);
        break;
     case 2:
        //device IP and network type 
        if (getIsDHCP())
          Serial.print("DHCP ");
        else 
          Serial.print("Static IP:");
        SND_NEW_LINE;

        Serial.print(WiFi.localIP());
        break;
  }
  
}

void interpretSetupInfo(Buttons key)
{
   if (doCursorChange(key))
      updateScreenSetup = true;
}

void displaySetupUser()
{
  Serial.print("User");
  SND_NEW_LINE;

  if (strlen(inputBuffer) == 0) //if nothing in the buffer convert the current user to a buffer
  {
    sprintf(inputBuffer, "%i", EosOscManager::getInstance()->getUser());
    pad (inputBuffer, 3);
  }

  Serial.print(inputBuffer);

  positionCursor();

}

void interpretSetupUser(Buttons key)
{
  //interpret what to do in the user setup
  //if REMDIM then exit (not save) (HANDLED BEFORE THIS - THIS IS ALL WAY THRU SETUP
  //if ENTER then save and exit
  //if CLR then clear the current cursor position
  //IF UP then increment cursor
  //IF DOWN then decrement cursor

  currentBufferLength = 3; //(0>254)

  if (doCursorChange(key))
  {
    updateScreenSetup = true;
    return;
  }
  if (getNumberFromKey(key, true, false)) //allow clear, don't allow dot
  { updateScreenSetup = true;
    return;
  }
  //must have something else!

  switch (key)
  {
    case BTN_ENTER:
      //DO SAVE
      //***********
      saveSetupUser();
      return;
      break;

  }

  //have had nothing!!

}

void saveSetupUser()
{
  //do the save

  int newUser;

  newUser = atoi(inputBuffer);

  //if Error display error & ret false
  if (newUser > 254 )
  {
    Serial.write(SND_CLEAR);
    Serial.print("User Number must be between 0 and 254");
    delay(1500); //wait 1.5sec
    updateScreenSetup = true;
    return;

  }

  if (newUser == 0)
    newUser = 255; //if user has selected 0 then set to 255 (which is currentuser for machine)


  if (newUser!=EosOscManager::getInstance()->getUser())
  {  //only update and write if we haven't changed it
      writeUserEEPROM(newUser);
        //if ok display OK & ret true
      EosOscManager::getInstance()->setUser(newUser);

  }

  EosOscManager::getInstance()->resetConnection();

  Serial.write(SND_CLEAR);
  
  if (newUser != 255)
  {
    Serial.print("User Number changed to ");
    Serial.print(newUser);

  } else
    Serial.print("User changed to console user");


  delay(1500);

  exitSetup();
}


void writeUserEEPROM(byte user)
{
  EEPROM.begin(512);
  EEPROM.put(SETUP_STORE_USER, user);
  EEPROM.commit();
  EEPROM.end();
}

byte readUserEEPROM()
{
  byte aUser =0;
  EEPROM.begin(512);
  EEPROM.get(SETUP_STORE_USER, aUser);
  EEPROM.commit();
  EEPROM.end();

  return aUser;
  
}
  


void pad(char * str, byte len)
{
  if (strlen(str) < len)
  { //extend buffer by adding leading 0?)
    char tmp[len + 1];
    strcpy(tmp, "");

    byte zeros = len - strlen(str);
    for (byte i = 0; i < zeros; i++)
      strcat(tmp, "0");

    strcat(tmp, str);
    strcpy(str, tmp);
  }
}

void checkCursor()
{
  if (cursorPosition == 255) cursorPosition = currentBufferLength - 1;
  if (cursorPosition > currentBufferLength - 1) cursorPosition = 0;
}

void positionCursor()
{

  Serial.write(SND_LEFT_LINE); //send the cursor to the start

  for (byte i = 0; i < cursorPosition; i++)
  {
    Serial.write(SND_RIGHT_CHAR);
  }

}
bool doCursorChange(Buttons key)
{
  switch (key)
  {
    case BTN_UP:
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_DOWN:
      cursorPosition --;
      checkCursor();
      return true;
      break;
  }

  return false;
}

bool getNumberFromKey(Buttons key, bool allowClear, bool allowDot)
{
  switch (key)
  {
    case BTN_CLR:
      if (allowClear)
      {
        inputBuffer[cursorPosition] = ' ';
        return true;
      } else
        return false;
      //TODO SHOULD SHIFT LEFT IF NOT MAX POSITION
      break;
    case BTN_1:
      inputBuffer[cursorPosition] = '1';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_2:
      inputBuffer[cursorPosition] = '2';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_3:
      inputBuffer[cursorPosition] = '3';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_4:
      inputBuffer[cursorPosition] = '4';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_5:
      inputBuffer[cursorPosition] = '5';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_6:
      inputBuffer[cursorPosition] = '6';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_7:
      inputBuffer[cursorPosition] = '7';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_8:
      inputBuffer[cursorPosition] = '8';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_9:
      inputBuffer[cursorPosition] = '9';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_0:
      inputBuffer[cursorPosition] = '0';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_DOT:
      if (allowDot)
      {
        inputBuffer[cursorPosition] = '.';
        cursorPosition ++;
        checkCursor();
        return true;
      } else
        return false;
      break;
  }

  return false; //no valid key sent
}

Buttons lastKey = BTN_NONE;
byte btnCount = 0;
bool getASCIIFromKey(Buttons key)
{
  switch (key)
  {
    case BTN_CLR:
      inputBuffer[cursorPosition] = ' ';
      lastKey = key;
      btnCount = 0;
      return true;
      break;
    case BTN_1:
      getLetterFromKey(1, key);
      return true;
      break;
    case BTN_2:
      getLetterFromKey(2, key);
      return true;
      break;
    case BTN_3:
      getLetterFromKey(3, key);
      return true;
      break;
    case BTN_4:
      getLetterFromKey(4, key);
      return true;
      break;
    case BTN_5:
      getLetterFromKey(5, key);
      return true;
      break;
    case BTN_6:
      getLetterFromKey(6, key);
      return true;
      break;
    case BTN_7:
      getLetterFromKey(7, key);
      return true;
      break;
    case BTN_8:
      getLetterFromKey(8, key);
      return true;
      break;
    case BTN_9:
      getLetterFromKey(9, key);
      return true;
      break;
    case BTN_0:
      inputBuffer[cursorPosition] = '0';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
    case BTN_DOT:

      inputBuffer[cursorPosition] = '.';
      cursorPosition ++;
      checkCursor();
      return true;
      break;
  }

  return false;
}

void getLetterFromKey(byte number, Buttons key) //number should be the number we want (so char 48+num), for now assume all have 3 letters
{
  if (lastKey != key && lastKey != BTN_UP && lastKey != BTN_DOWN && lastKey != BTN_CLR && lastKey != BTN_NONE)
  {
    cursorPosition++;
    checkCursor();
  }

  if (lastKey == key)
  {
    //is a subsequent press - A, B, C, a, b, c
    //A = 65, a = 97
    byte offset = (3 * (number - 1));
    if (offset > 0) offset = offset - (1 * (number - 1));

    if ((number < 9 && btnCount < 3) || btnCount < 2) //if 9 we only have two letters
      inputBuffer[cursorPosition] = 48 + 17 + number - 1 + btnCount + offset; //(to get in right place in ascii table, 48 = '0', 17 is diff from '0' > 'A'
    else if ((number < 9 && btnCount < 6) || btnCount < 4) //if 9 we only have two letters
      inputBuffer[cursorPosition] = 48 + 17 + 32 + number - 1 + (btnCount - 3) + offset;
    else
      btnCount = 255; //so when we ++ we go to 0!
    btnCount ++;
  } else
    btnCount = 0;
  if (btnCount == 0)
  {
    inputBuffer[cursorPosition] = 48 + number;
    btnCount = 0;
  }

  checkCursor();
}



