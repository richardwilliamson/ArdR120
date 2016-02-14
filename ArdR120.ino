//+10v > XLR pin 2 > brown
//Ground & screen - wires to XLR male pin 1 > grey
//pins 3-5 : not used
//Pin 6: RX (data to base/handheld) - wires to XLR male pin 5 > pink
//Pin 7: TX (data from base/handheld) - wires to XLR male > GREEN

//top left key 48, bottom right 92 (top>bottom, left to right)
//1=64

#include "ArdR120.h"
#include "OSC.h"

#include <OSCMessage.h>
#include <ESP8266WiFi.h>
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
    sendCmd(a);
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

void sendCmd(Buttons key)
{
    //key = (buttons)((int)key - BUTTON_OFFSET);
    char *cmd;
    
    switch (key)
    {
      case BTN_1:
      {
        cmd = "1";
        break;
      }
      case BTN_2:
      {
        cmd = "2";
        break;
      }
      case BTN_3:
      {
        cmd = "3";
        break;
      }
      case BTN_4:
      {
        cmd= "4";
        break;
      }
      case BTN_5:
      {
         cmd= "5";
         break;
      }
      case BTN_6:
      {
         cmd= "6";
         break;
      }
      case BTN_7:
      {
         cmd= "7";
         break;
      }
      case BTN_8:
      {
         cmd= "8";
         break;
      }
      case BTN_9:
      {
         cmd= "9";
         break;
      }
      case BTN_0:
      {
         cmd= "0";
         break;
      }
      case BTN_AT:
      {
        cmd = "@";
        break;
      }
      case BTN_ENTER:
      {
        cmd = "#";
        break;
      }
      case BTN_CLR:
      {
        cmd = "clear_cmd";
        break;
      }
      case BTN_THRU:
      {
        cmd = "Thru";
        break;
      }
      case BTN_PLUS:
      {
        cmd = "+";
        break;
      }
      case BTN_MINUS:
      {
        cmd = "-";
        break;
      }
      case BTN_CUE:
      {
        cmd = "Cue";
        break;
      }
      case BTN_GRP:
      {
        cmd = "Group";
        break;
      }
      case BTN_GO:
      {
        cmd = "Go_To_Cue";
        break;
      }
      case BTN_REM_DIM:
      {
        cmd = "Rem_Dim";
        break;
      }
      case BTN_FULL:
      {
        cmd = "Full";
        break;
      }
      case BTN_ON:
      {
        cmd = "On";
        break;
      }
      default:
      {
        return; //do nothing with it
      }
    }

    OSCMessage msg("/eos/cmd");
    msg.add(cmd);

    sendOSCMessage(msg);

}


