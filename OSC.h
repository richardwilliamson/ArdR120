#ifndef OSC_h
#define OSC_h
void sendOSCMessage(OSCMessage &theMessage);
void routeOSCMessage(OSCMessage &theMessage);
void routeChannel(OSCMessage &msg, int addrOffSet); 
void routeCmd(OSCMessage &msg, int addrOffSet);

#endif
