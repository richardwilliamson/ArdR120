#ifndef ArdR120_h
#define ArdR120_h
#include <OSCMessage.h>
#include <EosOscManager.h>
#include <EosOscCommand.h>
#include <EosOscChannel.h>
//#include <WiFiManager.h>

#include "BufferStore.h"

#include <ESP8266WiFi.h>

//display characters

#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println (x)
 #define DEBUG_DELAY delay(1000)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_DELAY
#endif


#define SND_HOME 1   //puts cursor back at start
#define SND_BACKSPACE 8
#define SND_SPACE 9 
#define SND_RIGHT_CHAR 9 //space and right char same?? not space!
#define SND_DOWN_LINE 10
#define SND_LEFT_LINE 13
#define SND_CLEAR 12

#define SND_NEW_LINE Serial.write(SND_DOWN_LINE); Serial.write(SND_LEFT_LINE)

//24 makes next chars flash
//25 makes previous chars flash
//26 stops all flashing
EosOscCommand oscCommand;

WiFiClient client;

//IPAddress inIp(192, 168, 1, 200); //these can all be saved automagically apparently
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 255, 0);


//note buttons go top down on R120 (stage electrics is different)
//first button is actually 48

#define BTN_RESET 0
#define BUTTON_OFFSET 47

enum Buttons {
  BTN_NONE,
  BTN_F1,
  BTN_REC_SUB,
  BTN_GO,
  BTN_UNDO,
  BTN_DIM,
  BTN_FX,
  BTN_SUB,
  BTN_GRP,
  BTN_CUE,
  BTN_F2,
  BTN_REC,
  BTN_CUT,
  BTN_ATT,
  BTN_LAST,
  BTN_7,
  BTN_4,
  BTN_1,
  BTN_CLR,
  BTN_F3,
  BTN_REC_TIME,
  BTN_STOP_BACK,
  BTN_FULL,
  BTN_NEXT,
  BTN_8,
  BTN_5,
  BTN_2,
  BTN_0,
  BTN_F4,
  BTN_TIME,
  BTN_REM_DIM,
  BTN_ON,
  BTN_OFF,
  BTN_9,
  BTN_6,
  BTN_3,
  BTN_DOT,
  BTN_F5,
  BTN_MACRO,
  BTN_UP,
  BTN_DOWN,
  BTN_THRU,
  BTN_MINUS,
  BTN_PLUS,
  BTN_AT,
  BTN_ENTER
};


boolean doConnect(bool verbose);
void checkForIncomingTCP();

void interpretCmd(Buttons key);
void interpretAtAtCmd(Buttons key);
void interpretSetupCmd(Buttons key);

void sendCmd(char cmd[]);
void sendKey(char key[]);

Buttons getKeyPress();

#endif



