#include "SetupManager.h"
#include "ArdR120.h"
#include "UploadManager.h"

bool updateScreenSetup = false;

char inputBuffer[16]; //max size of input buffer
byte cursorPosition;

void doSetupLoop()
{

  if (setupMode!=SETUP_MODE_AWAIT_SELECTION)   
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
  if (key!=BTN_NONE)
     interpretSetupCmd(key);

  yield();

  client.flush();
}


void interpretSetupCmd(Buttons key)
{


  if (setupMode!=SETUP_MODE_AWAIT_SELECTION)
  {
      switch (setupMode)
      {
          case SETUP_MODE_USER:
            interpretSetupUser(key);
            break;

      }
      return;
  }

  //so we have pressed @ATT twice..
  Serial.print("SETUP - REMDIM TO RETURN");
  //no setup mode yet
  
  if (strlen(inputBuffer)>0)
    strcpy(inputBuffer, ""); //empty out the input buffer
  cursorPosition = 0;
  
  switch (key)
  {
      case BTN_1:
        setupMode = SETUP_MODE_USER;
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
    Serial.print("SETUP - RemDim to exit");
    switch (setupMode)
    {
       case SETUP_MODE_USER:
          displaySetupUser();
    }
}

void exitSetup()
{
    setupMode = SETUP_MODE_NONE;
    oscManager.setScreenNeedsUpdate(true);
}

void displaySetupUser()
{
    Serial.print("User");
    SND_NEW_LINE;

    if (strlen(inputBuffer)==0) //if nothing in the buffer convert the current user to a buffer
    {
      sprintf(inputBuffer, "%i", oscManager.getUser());
      if (strlen(inputBuffer)<3)
      {//extend buffer by adding leading 0?)
         char tmp[4];
         strcpy(tmp, "");

         byte zeros = 3-strlen(inputBuffer);
         for (byte i=0; i<zeros; i++)
            strcat(tmp, "0");

          strcat(tmp, inputBuffer);
          strcpy(inputBuffer, tmp);
      }
    }      
      
    Serial.print(inputBuffer);

    Serial.write(SND_LEFT_LINE); //send the cursor to the start

    for (byte i=0; i<cursorPosition; i++)
    {
      Serial.write(SND_RIGHT_CHAR);
    }
  
}

void interpretSetupUser(Buttons key)
{
    //interpret what to do in the user setup
    //if REMDIM then exit (not save)
    //if ENTER then save and exit
    //if CLR then clear the current cursor position
    //IF UP then increment cursor
    //IF DOWN then decrement cursor

    switch (key)
    {
        case BTN_ENTER:
            //DO SAVE
            //***********
            saveUserSetup();
            return;
            break;
        case BTN_CLR:
            inputBuffer[cursorPosition] = ' ';
            //TODO SHOULD SHIFT LEFT IF NOT MAX POSITION
            break;
        case BTN_UP:
            cursorPosition ++;
            break;
        case BTN_DOWN:
            cursorPosition --;
            break;
        case BTN_1:
            inputBuffer[cursorPosition] = '1';
            cursorPosition ++;
            break;
        case BTN_2:
            inputBuffer[cursorPosition] = '2';
            cursorPosition ++;
            break;
        case BTN_3:
            inputBuffer[cursorPosition] = '3';
            cursorPosition ++;
            break;
        case BTN_4:
            inputBuffer[cursorPosition] = '4';
            cursorPosition ++;
            break;
        case BTN_5:
            inputBuffer[cursorPosition] = '5';
            cursorPosition ++;
            break;
        case BTN_6:
            inputBuffer[cursorPosition] = '6';
            cursorPosition ++;
            break;
        case BTN_7:
            inputBuffer[cursorPosition] = '7';
            cursorPosition ++;
            break;
        case BTN_8:
            inputBuffer[cursorPosition] = '8';
            cursorPosition ++;
            break;
        case BTN_9:
            inputBuffer[cursorPosition] = '9';
            cursorPosition ++;
            break;
        case BTN_0:
            inputBuffer[cursorPosition] = '0';
            cursorPosition ++;
            break;
    }
    
    if (cursorPosition > 2) cursorPosition = 0;
    if (cursorPosition ==255) cursorPosition = 2;
    
    updateScreenSetup = true;
}

void saveUserSetup()
{
    //do the save

    int newUser;

    newUser = atoi(inputBuffer);
    
    //if Error display error & ret false
    if (newUser > 254 )
    {
        Serial.write(SND_CLEAR);
        Serial.print("User Number must be between 0 and 254");
        delay(1500); //wait half a second
        updateScreenSetup = true;
        return;
      
    }

    if (newUser == 0)
      newUser = 255; //if user has selected 0 then set to 255 (which is currentuser for machine)
    
    
    //if ok display OK & ret true
    oscManager.setUser(newUser);
    oscManager.resetConnection();
    
    //TODO SAVE TO PERSISTANT MEMORY

    Serial.write(SND_CLEAR);
    if (newUser!=255)
    {
         Serial.print("User Number changed to ");
         Serial.print(newUser);
    } else
         Serial.print("User changed to console user");
         
    delay(1500);

    exitSetup();
}

