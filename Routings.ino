
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

void routeCmd(OSCMessage &msg, int addrOffset) {
  //Serial.println("route command");
  int length = msg.getDataLength(0);

  char cmdLine[length];
  msg.getString(0, cmdLine, length);

  String str = String(cmdLine);
  if (length>29)
    str = str.substring(length-29);
    
  str.toCharArray(commandInfo, 29);
  
}

