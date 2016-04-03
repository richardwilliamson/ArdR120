//+10v > XLR pin 2 > brown
//Ground & screen - wires to XLR male pin 1 > grey
//pins 3-5 : not used
//Pin 6: RX (data to base/handheld) - wires to XLR male pin 5 > pink
//Pin 7: TX (data from base/handheld) - wires to XLR male > GREEN

#define DEBUG

#include "ArdR120.h"

#include "SetupManager.h"
#include "setup_wifi.h"


//#include <EEPROM.h>

int status = WL_IDLE_STATUS;

void setup() {
  
  //consoleIP = IPAddress(192,168,1,9);
  //updateEEPROMConsoleIP();
//
//    staticIP = IPAddress(192,168,1,200);
//     IPAddress subnet(255,255,255,0);
//     IPAddress gateway(192,168,1,1);
//     updateEEPROMStaticIP(gateway, subnet);
  
  
  Serial.begin(9600);
  delay(100); //wait for the thing to go alive!

  Serial.write(SND_CLEAR);
  Serial.print("WELCOME");    

  //start up wifi - has to be before config on ESP (but I think after on normal arduino..)
  //WiFi.begin("richard", "appleapple"); 
  WiFi.begin(); 

  setupWifiManager();


  if (!getIsDHCP())
  {  
     DEBUG_PRINT("Static");
     DEBUG_DELAY;
     //only read this if we are static
     IPAddress subnet;
     IPAddress gateway;
     readEEPROMIP(gateway, subnet);
     DEBUG_PRINT("gateway/subnet");
     DEBUG_PRINT(gateway);
     DEBUG_PRINT(subnet);
     DEBUG_DELAY;
     WiFi.config(staticIP, gateway, subnet);
     DEBUG_PRINT(staticIP);
  } //otherwise we are DHCP so don't need to call config..
  else
  {
    DEBUG_PRINT("DHCP");
    DEBUG_DELAY;
  }

  readEEPROMConsoleIP();
  //TODO - if we don't have a console IP we should prompt for one..
  DEBUG_PRINT("reading user");
  yield();
  byte user = readUserEEPROM();
  EosOscManager::getInstance()->setUser(user); //should set which user from settings..
  oscCommand = EosOscCommand();

  DEBUG_PRINT("register stuff");
  yield();

  EosOscManager::getInstance()->registerHandler(&oscCommand);
  EosOscManager::getInstance()->registerHandler(EosOscChannel::getInstance());

  DEBUG_PRINT("connecting");
  DEBUG_PRINT(WiFi.localIP());
  
  bool connected = doConnect(true);

  if (!connected)
  {
    return; //don't do anything else (should now be in a setup mode)
  }

  EosOscManager::getInstance()->resetConnection();


  delay(1000); //display message for a second before then trying to update screen



}


boolean doConnect(bool verbose)
{
  //make sure connected to wifi

  connectWifi();

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.write(SND_CLEAR);
    Serial.print("wifi failed:");
    SND_NEW_LINE;
    Serial.print(WiFi.SSID());
    //Serial.print(pass);
    Serial.print("@ATT auto config, REMDIM man confi, or retry");
    while(1) //wait for input and either go to config or retry
    {
      yield();
        Buttons key = getKeyPress();
        if (key==BTN_ATT)
        { //go to config
            enterSetup(SETUP_MODE_ACCESSPOINT);
            return false;
        } else if (key==BTN_REM_DIM)
        { 
            enterSetup(SETUP_MODE_WIFI_SSID);
            return false;
        } else if (key!=BTN_NONE)
        { //just retry
            return false;
        }
        
    }
    return false;
  }
  if (verbose)
  {
    Serial.print("Connected to:");
    SND_NEW_LINE;
    Serial.print(WiFi.SSID());
  }
  yield();

  //now connect to console
  byte tries = 0;
  while (!client.connected())
  {
    delay(10);
    client.connect(consoleIP, 3032); //should we do any kind of timeout??
    tries++;
    if (tries > 2) 
      break;
    
  }

  if (!client.connected())
  {
    Serial.write(SND_CLEAR);
    Serial.print("Console failed:");
    SND_NEW_LINE;
    Serial.print(consoleIP);
    Serial.print("@ATT configure, any key retry");
    while(1) //wait for input and either go to config or retry
    {
      yield();
        Buttons key = getKeyPress();
        if (key==BTN_ATT)
        { //go to config
            enterSetup(SETUP_MODE_CONSOLE_IP);
            return false;
        } else if (key!=BTN_NONE)
        { //just retry
            return false;
        }
        
    }
    return false;
  }


  if (tries) //since tries is not 0 we must not have been connected but now are!
  {
      EosOscManager::getInstance()->setClient(client);
      EosOscManager::getInstance()->resetConnection(); //since we weren't connected and now are we should reset the connection to be safe
  }

  if (verbose)
  {
    Serial.print("Console connected:");
    SND_NEW_LINE;
    Serial.print(consoleIP);
  }

  return true;




}

Buttons getKeyPress()
{
  if (Serial.available())
  {
    //Serial.println("A");
    Buttons a = (Buttons)(Serial.read() - BUTTON_OFFSET);
    return a;

    Serial.flush(); //only get one command at a time
    //Serial.print(a);
    //yield();
  }

  return BTN_NONE;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (setupMode != SETUP_MODE_NONE)
  { //if in setup mode loop elsewhere
    if (client.connected())
    { //stop listening for data and empty cache
      //Serial.print("DISCONNECTING");
      client.flush();
      //client.stop();
    }
    doSetupLoop();
    yield();
    return;
  }

  if (!doConnect(false))
  {
    //return
    //TODO - should listen out for a button press to enter setup?
    return;
  }
//  boolean connected 
//  while (!doConnect(false))
//  {
//    while (!Serial.available())
//    {
//      yield(); //just give up until we get a button press
//    }
//    Serial.flush(); //empty the buffer so we ignore the input
//  }

  EosOscManager::getInstance()->checkForIncomingTCP();

  yield();

  //Serial.write(Serial.read());
  Buttons key = getKeyPress();

  if (key != BTN_NONE)
    interpretCmd(key);
  //check for a new keypress

  yield();


  //update screen if required
  if (EosOscManager::getInstance()->getScreenNeedsUpdate())
  {

    Serial.write(SND_CLEAR);

    EosOscChannel::getInstance()->writeCommandLine(Serial);

    SND_NEW_LINE;

    oscCommand.writeCommandLine(Serial);

    EosOscManager::getInstance()->setScreenNeedsUpdate(false);

  }
  //delay(10); //keeps ESP happy*/
}

long lastAtAtPress = 0; //used to log the last time AtAt was pressed
int atAtTimeout = 1000; //millis that we will wait for AtAt to be pressed

void interpretCmd(Buttons key)
{

  if (lastAtAtPress > 0)
    if (millis() - lastAtAtPress  < atAtTimeout)
    { //we are in AtAtMode
      interpretAtAtCmd(key);  //interpret it
      lastAtAtPress = 0;  //reset the counter
      return; //don't do anything else
    } else
    {
      lastAtAtPress = 0;

    }

  //key = (buttons)((int)key - BUTTON_OFFSET);
  switch (key)
  {
    case BTN_ATT:
      {
        lastAtAtPress = millis();
        //TODO - Display something to show atAtt pressed on screen
        Serial.print("SHIFT");

        return;
      }
    case BTN_1:
      {
        oscCommand.sendCommand("1");
        break;
      }
    case BTN_2:
      {
        oscCommand.sendCommand("2");
        break;
      }
    case BTN_3:
      {
        oscCommand.sendCommand("3");
        break;
      }
    case BTN_4:
      {
        oscCommand.sendCommand("4");
        break;
      }
    case BTN_5:
      {
        oscCommand.sendCommand("5");
        break;
      }
    case BTN_6:
      {
        oscCommand.sendCommand("6");
        break;
      }
    case BTN_7:
      {
        oscCommand.sendCommand("7");
        break;
      }
    case BTN_8:
      {
        oscCommand.sendCommand("8");
        break;
      }
    case BTN_9:
      {
        oscCommand.sendCommand("9");
        break;
      }
    case BTN_0:
      {
        oscCommand.sendCommand("0");
        break;
      }
    case BTN_AT:
      {
        oscCommand.sendCommand("@");
        break;
      }
    case BTN_ENTER:
      {
        oscCommand.sendKey("ENTER"); //send this as a key not a command
        break;
      }
    case BTN_CLR:
      {
        oscCommand.sendCommand("clear_cmd");
        break;
      }
    case BTN_THRU:
      {
        oscCommand.sendCommand("Thru");
        break;
      }
    case BTN_PLUS:
      {
        oscCommand.sendCommand("+");
        break;
      }
    case BTN_MINUS:
      {
        oscCommand.sendCommand("-");
        break;
      }
    case BTN_CUE:
      {
        oscCommand.sendCommand("Cue");
        break;
      }
    case BTN_GRP:
      oscCommand.sendCommand("Group");
      break;
    case BTN_GO:
      {
        oscCommand.sendKey("go");
        break;
      }
    case BTN_STOP_BACK:
      oscCommand.sendKey("stopback");
      break;
    case BTN_REM_DIM:
      oscCommand.sendCommand("Rem_Dim");
      break;
    case BTN_FULL:
      {
        oscCommand.sendCommand("Full");
        break;
      }
    case BTN_ON:
      {
        oscCommand.sendCommand("Level");
        break;
      }
    case BTN_REC:
      oscCommand.sendCommand("Record");
      break;
    case BTN_MACRO:
      oscCommand.sendKey("Macro");
      break;
    case BTN_NEXT:
      oscCommand.sendKey("Next");
      break;
    case BTN_LAST:
      oscCommand.sendKey("Last");
      break;
    case BTN_UP:
      oscCommand.sendKey("+%");
      break;
    case BTN_DOWN:
      oscCommand.sendKey("-%");
      break;
    default:
      {
        return; //do nothing with it
      }
  }
}

void interpretAtAtCmd(Buttons key)
{
  //these are the shift-commands
  switch (key)
  {
    case BTN_ATT:
      setupMode = SETUP_MODE_AWAIT_SELECTION;
      break;
    case BTN_CLR:
      oscCommand.sendNewCommand("");
      break;
    case BTN_GO:
      oscCommand.sendCommand("Go_To_Cue");
      break;
    case BTN_0:
      oscCommand.sendCommand("Sneak");
      break;
    case BTN_1:
      oscCommand.sendCommand("Preset");
      break;
    case BTN_2:
      oscCommand.sendCommand("Intensity_Palette");
      break;
    case BTN_3:
      oscCommand.sendCommand("Focus_Palette");
      break;
    case BTN_4:
      oscCommand.sendCommand("Color_Palette");
      break;
    case BTN_5:
      oscCommand.sendCommand("Beam_Palette");
      break;
    case BTN_6:
      oscCommand.sendKey("Live");
      break;
    case BTN_7:
      oscCommand.sendKey("Blind");
      break;
    case BTN_8:
      oscCommand.sendKey("open_park_blind");
      break;
    case BTN_9:
      oscCommand.sendKey("open_dmx_patch");
      break;
    case BTN_REC:
      oscCommand.sendCommand("Update");
      break;
    case BTN_AT:
      oscCommand.sendCommand("Address");
      break;
    case BTN_PLUS:
      oscCommand.sendCommand("Park");
      break;
    default:
      {
        return; //do nothing
      }
  }
}





