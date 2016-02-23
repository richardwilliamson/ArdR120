#include "EosOscManager.h"

EosOscManager::EosOscManager() 
{
    //setUser(255);
}

EosOscManager::EosOscManager(byte theUser, WiFiClient &theClient) 
{
    this->setUser(theUser);
    client = theClient;
    this->resetConnection();
}

void EosOscManager::setUser(byte theUser)
{
    user=theUser;
    if (setHandler)
      handler->userChanged();
}

byte EosOscManager::getUser()
{
    return user; 
}



//route the incoming OSC
void EosOscManager::routeOSC(OSCMessage &theMessage)
{
  //lcd.clear();
  if (!theMessage.hasError()) //if the OSC is OK then act on it
  {
    //route any that include the user
    //Serial.print(handler);
    if (setHandler)
    { 
       handler->routeOsc(theMessage);
    } 
    
    theMessage.route(String(F("/eos/out/active/chan")).c_str(), routeChannel);
    theMessage.route(String(F("/eos/out/active/chan")).c_str(), this->routeScreenNeedsUpdate);

    //theMessage.route(String(F("/eos/out/cmd")).c_str(), routeCmd);
    theMessage.route(String(F("/eos/out/cmd")).c_str(), this->routeScreenNeedsUpdate);


  } else
  {
    Serial.println("error");
  }

}


void EosOscManager::sendOSCMessage(OSCMessage &message)
{
  //send out via TCP
  BufferStore(buf); //declare a buffer to send to

      //MsgLength mLen;

  message.send(buf); //send to the buffer
  buf.sendOut(client); //now transmit it en masse

  message.empty(); // free space occupied by message

}




bool EosOscManager::getScreenNeedsUpdate()
{
  return screenNeedsUpdate;
}

void EosOscManager::setScreenNeedsUpdate(bool doesIt)
{
  screenNeedsUpdate = doesIt;
}



void EosOscManager::checkForIncomingTCP()
{
  
  int rdS = 0;

  // read in the OSC From the client if we have it
  if ((rdS = client.available()) > 0)
  {
    //Serial.println("Got stuff");
    MsgLength len;  //a struct for the length
    len.value = 0; //preset it to 0

    byte f4 = 4; //first 4 bytes are the size, then we have the actual message..
    //get the size
    while (f4--)
    {
      len.bytes[f4] = client.read();
      rdS--;
    }

    String str = "";
    OSCMessage(rMsg);

    while (len.value--)
      //str +=(char)client.read();
      rMsg.fill(client.read());

    // Serial.println(wheelC);


    if (!rMsg.hasError()) 
    { //is valid OSC
      //send out via SLIP
      yield();
      this->routeOSC(rMsg);
      //delay(1);
    }

    rMsg.empty();

  } //*/
}

void EosOscManager::resetConnection()
{
    if (!client.connected())
      return; //don't try and connect if we aren't connected!
    //first reset the connection
    OSCMessage msg("/eos/reset");
    sendOSCMessage(msg);
    msg.empty();

    //now register which user we are
    if (user!=255)
    {
      //return; //don't need to register if we don't have a user
      char cmd[14];

      strcpy(cmd, "/eos/user/");
      char userNum[3];
      sprintf(userNum, "%i", user);
      strcat(cmd, userNum);
      OSCMessage msg2(cmd);
      this->sendOSCMessage(msg2); 
    }
    yield();
    

    //TODO should we transmit the current user so we remember what we are listening for?
}

void EosOscManager::registerHandler(EosOscHandler *theHandler)
//void EosOscManager::registerHandler(EosOscCommand * theHandler)
{
  handler = theHandler;
  setHandler = true;
}

//ROUTINGS???

void routeChannel(OSCMessage &msg, int addrOffSet) {
  /// recieves eos/out/active/chan
  // the info is in the description
  //which is a String

  //so get the description
  byte descLength = msg.getDataLength(0);
  char descC[descLength];
 // Serial.println("route channel");
  msg.getString(0, channelInfo, 29);
  //Serial.println(descC);

}

//call this each time we have any OSC that might affect the output
void EosOscManager::routeScreenNeedsUpdate(OSCMessage &msg, int addrOffSet)
{
  //todo need to implement a singleton object for oscManager
    oscManager.setScreenNeedsUpdate(true);

}







