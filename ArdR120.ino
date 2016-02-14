//+10v > XLR pin 2 > brown
//Ground & screen - wires to XLR male pin 1 > grey
//pins 3-5 : not used
//Pin 6: RX (data to base/handheld) - wires to XLR male pin 5 > pink
//Pin 7: TX (data from base/handheld) - wires to XLR male > GREEN

//top left key 48, bottom right 92 (top>bottom, left to right)
//1=64



#include <OSCMessage.h>
#include <ESP8266WiFi.h>

#include "ArdR120.h"
#include "OSC.h"
#include "BufferStore.h"

//display characters

#define SND_HOME 1   //puts cursor back at start
#define SND_BACKSPACE 8
#define SND_SPACE 9
#define SND_DOWN_LINE 10
#define SND_LEFT_LINE 13
#define SND_CLEAR 12

#define SND_NEW_LINE Serial.write(SND_DOWN_LINE); Serial.write(SND_LEFT_LINE)

//24 makes next chars flash
//25 makes previous chars flash
//26 stops all flashing


int status = WL_IDLE_STATUS;
const char* ssid = "richard";  //  your network name (SSID)
//const char* ssid = "BTBusinessHub-125";  //  your network name (SSID)

const char* pass = "appleapple";       // your network password
//const char* pass = "9315634916";       // your network password

IPAddress outIp(192, 168, 1, 9); //IP of console

IPAddress inIp(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiClient client;

boolean doConnect(bool verbose)
{
   //make sure connected to wifi
   
    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      tries++;
      if (tries > 30) {
        break;
      }
    }

   if (WiFi.status() != WL_CONNECTED)
   {
     Serial.write(SND_CLEAR);
     Serial.print("Failed to connect to wifi network");
     SND_NEW_LINE;
     Serial.print(ssid);
     return false;
   }
   if (verbose)
   {
      Serial.print("Connected to network");
      SND_NEW_LINE;
      Serial.print(ssid);
   }
   yield();

   //now connect to console
   tries = 0;
   while (!client.connected())
   {
      delay(100);
      client.connect(outIp, 3032); //should we do any kind of timeout??
      tries++;
      if (tries>30) {
        break;
      }
   }

   if (!client.connected())
   {
      Serial.write(SND_CLEAR);
      Serial.print("Failed to connect to console");
      SND_NEW_LINE;
      Serial.print(outIp);
      return false;
   }

   if (verbose)
   {
      Serial.print("Connected to console");
      SND_NEW_LINE;
      Serial.print(outIp);
   }

  

   
}
void setup() {

  WiFi.config(inIp, gateway, subnet); //sets up details of itself
  WiFi.begin(ssid, pass);

  //  WiFi.begin(ssid);

  
  // put your setup code here, to run once:
  Serial.begin(9600);

//boot into the thing
  delay(100); //wait for the thing to go alive!
  
  Serial.write(SND_CLEAR);
  Serial.print("WELCOME");

  while (!doConnect(true))
  {
      while (!Serial.available())
      { 
         yield(); //just give up until we get a button press
      }
      Serial.flush(); //empty the buffer so we ignore the input
  }
      
  delay(1000); //display message for a second before then trying to update screen
  


}

void loop() {
 // put your main code here, to run repeatedly:
  doConnect(false);
 
  checkForIncomingTCP();
  
  yield(); //alow background to run..

  //Serial.write(Serial.read());

 //check for a new keypress
  while(Serial.available())
  {
    //Serial.println("A");
    Buttons a = (Buttons)(Serial.read()-BUTTON_OFFSET);
    interpretCmd(a);
    //Serial.print(a);
    //yield();
  }
  yield();


//update screen if required
  if (updateScreen)
  {
    Serial.write(SND_CLEAR);
    
    Serial.print(channelInfo);
    
    SND_NEW_LINE; 

    Serial.write(commandInfo);

    updateScreen = false;
    
  }
  //delay(10); //keeps ESP happy
}

long lastAtAtPress = 0; //used to log the last time AtAt was pressed
int atAtTimeout = 1000; //millis that we will wait for AtAt to be pressed

void interpretCmd(Buttons key)
{

    if (lastAtAtPress>0)
      if(millis() - lastAtAtPress  < atAtTimeout)
      { //we are in AtAtMode
          interpretAtAtCmd(key);  //interpret it
          lastAtAtPress = 0;  //reset the counter
          return; //don't do anything else
      } else
          lastAtAtPress = 0;
    
    //key = (buttons)((int)key - BUTTON_OFFSET);    
    switch (key)
    {
      case BTN_ATT:
      {
        lastAtAtPress = millis();
        //TODO - Display something to show atAtt pressed on screen
        
        return;
      }
      case BTN_1:
      {
        sendCmd("1");
        break;
      }
      case BTN_2:
      {
        sendCmd("2");
        break;
      }
      case BTN_3:
      {
        sendCmd("3");
        break;
      }
      case BTN_4:
      {
        sendCmd("4");
        break;
      }
      case BTN_5:
      {
         sendCmd("5");
         break;
      }
      case BTN_6:
      {
         sendCmd("6");
         break;
      }
      case BTN_7:
      {
         sendCmd("7");
         break;
      }
      case BTN_8:
      {
         sendCmd("8");
         break;
      }
      case BTN_9:
      {
         sendCmd("9");
         break;
      }
      case BTN_0:
      {
         sendCmd("0");
         break;
      }
      case BTN_AT:
      {
        sendCmd("@");
        break;
      }
      case BTN_ENTER:
      {
        sendKey("ENTER"); //send this as a key not a command
        break;
      }
      case BTN_CLR:
      {
        sendCmd("clear_cmd");
        break;
      }
      case BTN_THRU:
      {
        sendCmd("Thru");
        break;
      }
      case BTN_PLUS:
      {
        sendCmd("+");
        break;
      }
      case BTN_MINUS:
      {
        sendCmd("-");
        break;
      }
      case BTN_CUE:
      {
        sendCmd("Cue");
        break;
      }
      case BTN_GRP:
      {
        sendCmd("Group");
        break;
      }
      case BTN_GO:
      {
        sendKey("go");
        break;
      }
      case BTN_STOP_BACK:
        sendKey("stopback");
        break;
      case BTN_REM_DIM:
        sendCmd("Rem_Dim");
        break;
      case BTN_FULL:
      {
        sendCmd("Full");
        break;
      }
      case BTN_ON:
      {
        sendCmd("Level");
        break;
      }
      case BTN_REC:
         sendCmd("Record");
         break;
      case BTN_MACRO:
         sendKey("Macro");
         break;
      case BTN_NEXT:
         sendKey("Next");
         break;
      case BTN_LAST:
         sendKey("Last");
         break;
      case BTN_UP:
         sendKey("+%");
         break;
      case BTN_DOWN:
         sendKey("-%");
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
    switch(key)
    {
       case BTN_CLR:
       {  
          OSCMessage msg("/eos/newcmd");
          sendOSCMessage(msg);
          break;
       }
       case BTN_GO:
          sendCmd("Go_To_Cue");
          break;
       case BTN_0:
          sendCmd("Sneak");
          break;
       case BTN_1:
          sendCmd("Preset");
          break;
       case BTN_2:
          sendCmd("Intensity_Palette");
          break;
       case BTN_3:
          sendCmd("Focus_Palette");
          break;
       case BTN_4:
          sendCmd("Color_Palette");
          break;
       case BTN_5:
          sendCmd("Beam_Palette");
          break;
       case BTN_REC:
          sendCmd("Update");
          break;
       case BTN_AT:
          sendCmd("Address");
          break;
       case BTN_PLUS:
          sendCmd("Park");
          break;
       default:
       {
          return; //do nothing
       }
    }
}

//TODO add in option to /eos/user/XX/...
byte user = 255;

void getUserString(String &str)
{
    str += "/eos/";
    if (user<255) //255 means use desk's current user
    {
       str +="/user/";
       str +=user;
       str +="/";
    }
       
}

void sendCmd(char cmd[])
{
    String cmdString = "";
    getUserString(cmdString);
    cmdString +="cmd";

    char cstr[cmdString.length()+1];
    cmdString.toCharArray(cstr, cmdString.length()+1);
    
    OSCMessage msg(cstr);
    msg.add(cmd);

    sendOSCMessage(msg);
}

void sendKey(char key[])
{
    String str = "";
    getUserString(str);
    str += "key/";
    str += xx§key;
    char cstr[str.length()+1];

    str.toCharArray(cstr, str.length()+1);
    OSCMessage msg(cstr);
    sendOSCMessage(msg);
}


