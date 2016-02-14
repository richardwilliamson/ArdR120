#include "OSC.h"

void sendOSCMessage(OSCMessage &message)
{
  //send out via TCP
  BufferStore(buf); //declare a buffer to send to

      //MsgLength mLen;

  message.send(buf); //send to the buffer
  buf.sendOut(client); //now transmit it en masse

  message.empty(); // free space occupied by message

}

void checkForIncomingTCP()
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
      routeOSCMessage(rMsg);
      //delay(1);
    }

    rMsg.empty();

  } 
}

//call this each time we have any OSC that might affect the output
void screenNeedsUpdate(OSCMessage &msg, int addrOffSet)
{
    updateScreen = true;

}
void routeOSCMessage(OSCMessage &theMessage)
{
  //lcd.clear();
  if (!theMessage.hasError()) //if the OSC is OK then act on it
  {
    theMessage.route(String(F("/eos/out/active/chan")).c_str(), routeChannel);
    theMessage.route(String(F("/eos/out/active/chan")).c_str(), screenNeedsUpdate);

    theMessage.route(String(F("/eos/out/cmd")).c_str(), routeCmd);
    theMessage.route(String(F("/eos/out/cmd")).c_str(), screenNeedsUpdate);


  } else
  {
    Serial.println("error");
  }

}
