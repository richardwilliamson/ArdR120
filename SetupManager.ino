#include "SetupManager.h"
#include "UploadManager.h"

bool updateScreenSetup = false;

char inputBuffer[17]; //max size of input buffer
byte cursorPosition;
byte currentBufferLength;

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
      case SETUP_MODE_USER:
        interpretSetupUser(key);
        break;
      case SETUP_MODE_DEV_IP:
      case SETUP_MODE_DEV_SUBNET:
      case SETUP_MODE_CONSOLE_IP:
        //send all to same
        interpretSetupIp(key);
        break;
      case SETUP_MODE_WIFI_SSID:
      case SETUP_MODE_WIFI_PASS:
        interpretSetupWifi(key);
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
      setupMode = SETUP_MODE_USER;
      updateScreenSetup = true;
      break;
    case BTN_2:
      setupMode = SETUP_MODE_DEV_IP;
      updateScreenSetup = true;
      break;
    case BTN_3:
      setupMode = SETUP_MODE_DEV_SUBNET;
      updateScreenSetup = true;
      break;
    case BTN_4:
      setupMode = SETUP_MODE_CONSOLE_IP;
      updateScreenSetup = true;
      break;
    case BTN_5:
      setupMode = SETUP_MODE_WIFI_SSID;
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
  switch (setupMode)
  {
    case SETUP_MODE_USER:
      displaySetupUser();
      break;
    case SETUP_MODE_DEV_IP:
    case SETUP_MODE_DEV_SUBNET:
    case SETUP_MODE_CONSOLE_IP:
      displaySetupIp();
      break;
    case SETUP_MODE_WIFI_SSID:
    case SETUP_MODE_WIFI_PASS:
      displaySetupWifi();
      break;
  }
}

void exitSetup()
{
  setupMode = SETUP_MODE_NONE;
  EosOscManager::getInstance()->setScreenNeedsUpdate(true);
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

  Serial.write(SND_LEFT_LINE); //send the cursor to the start

  for (byte i = 0; i < cursorPosition; i++)
  {
    Serial.write(SND_RIGHT_CHAR);
  }

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


  //if ok display OK & ret true
  EosOscManager::getInstance()->setUser(newUser);
  EosOscManager::getInstance()->resetConnection();

  //TODO SAVE TO PERSISTANT MEMORY

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
  }
  SND_NEW_LINE;

  if (strlen(inputBuffer) == 0) //if nothing in the buffer convert the current relevant IP to buffer
  {
    IPAddress ip;
    switch (setupMode)
    {
      case SETUP_MODE_DEV_IP:
        ip = inIp;
        break;
      case SETUP_MODE_DEV_SUBNET:
        ip = subnet;
        break;
      case SETUP_MODE_CONSOLE_IP:
        ip = outIp;
        break;
    }

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

  Serial.write(SND_LEFT_LINE); //send the cursor to the start

  for (byte i = 0; i < cursorPosition; i++)
  {
    Serial.write(SND_RIGHT_CHAR);
  }

}

void saveSetupIp()
{

  Serial.write(SND_CLEAR);
  //split the string into it's bits
  bool error = false;
  char * pch;
  pch = strtok (inputBuffer, ".");
  byte i = 0;
  byte res[4]; //ready to recieve -
  while (pch != NULL)
  {
    if (i < 4)
    {
      int number = atoi(pch);
      if (number < 254 && number < 0)
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
      case SETUP_MODE_DEV_IP:
        inIp = res;
        Serial.print("Device IP changed");
        break;
      case SETUP_MODE_DEV_SUBNET:
        subnet = res;
        Serial.print("Subnet changed");
        break;
      case SETUP_MODE_CONSOLE_IP:
        outIp = res;
        Serial.print("console IP changd");
        break;
    }

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

}
void saveSetupWifi()
{

}


