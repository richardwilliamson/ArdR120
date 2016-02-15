#ifndef ArdR120_h
#define ArdR120_h
#include "OSCMessage.h";
union MsgLength   //this means I can acces the bytes and the long value for a 4 didgit int (long)
{
  unsigned long value;
  byte bytes[4];
};

//note buttons go top down on R120 (stage electrics is different)
//first button is actually 48
#define BUTTON_OFFSET 48

enum Buttons {
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

char channelInfo[30];
char commandInfo[30];

bool updateScreen = false;


void checkForIncomingTCP();

void interpretCmd(Buttons key);
void interpretAtAtCmd(Buttons key);

void sendCmd(char cmd[]);
void sendKey(char key[]);

#endif

