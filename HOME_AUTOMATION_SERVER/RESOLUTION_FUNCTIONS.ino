void resolveFunction()
{
  byte* dataframeptr = readPacket();                                                                        //gets the incoming data frame and splits it into the components
  
  byte ID           = dataframeptr[0];
  byte subnet       = dataframeptr[1];
  byte functionCode = dataframeptr[2];
  byte channel      = dataframeptr[3];
  byte data         = dataframeptr[4];
  byte parity       = dataframeptr[5];
  
  #ifdef DEBUG
    {
    mySerial.print("ID: ");
    mySerial.println(ID);
    mySerial.print("subnet: ");
    mySerial.println(subnet);
    mySerial.print("function code: ");
    mySerial.println(functionCode);
    mySerial.print("channel: ");
    mySerial.println(channel);
    mySerial.print("data: ");
    mySerial.println(data);
    mySerial.print("parity: ");
    mySerial.println(parity);
    mySerial.println();
    }
  #endif
  
  switch(functionCode)                                                                                    //resolve each function code and call the appropriate function
  {
    case (byte)functionCode::discovery:
         resolveDiscovery(ID, channel);
         break;
         
    case (byte)functionCode::serve:
         resolveServe(ID, subnet);
        break;
     
    case (byte)functionCode::digitalControl:
          resolveDigitalControl(ID, subnet, channel, data);
         break;
         
    case (byte)functionCode::analogControl:
          resolveAnalogControl(ID, subnet, channel, data);
       break;
       
    case (byte)functionCode::changeID:
          resolveIDchange(ID, subnet, data);
         break;

      default:
      break;
  }
}


void resolveDiscovery(byte& passedID, byte& passedChannel)
{
  if(passedID == myMAC && passedChannel != 0)                                                                                         //MAC is used for discovery not device ID, once the server realises that the function code is discovery, it treaths the first byte as MAC
  {
    sendPacket(functionCode::discovery, deviceType);                                                                                  //for a discovery message response, server responds with its ID in ID, subnet in SUBNET, discovery functioncode in FUNCTIONCODE and the data is the devicetype the devicetype in DATA response
  }
}

void resolveServe(byte& passedID, byte& passedSubnet)                                                                                 //a request to serve information has been issued, server must send sensor informations or IO states
{
  if(passedID == myID && passedSubnet == mySubnet)                                                                                   //don't forget to always make sure that we confirm the subnet mask
  {
    sendPacket(functionCode::serve, deviceState);                                                                                     //send sensor readings or digital states of relay, each bit in the device state represents a port state
  }
}


void resolveDigitalControl(byte& passedID, byte& passedSubnet, byte& channel, byte& passedData)                                       //a request to control a digital port has been issued, server must adhare to it and send a reponse to confim conformity
{
  if((passedID == myID) && (passedSubnet == mySubnet) &&(deviceType == device::relayModule || deviceType == device::actuator))        //only relays and actuators have digitally controlled IO not sensors, use subnet and ID to confirm which device is being addressed
  {
    //get channel to toggle from channel and state to toggle from data, data can only be 1 or 0
    //update channel state to passed data and return it
    sendPacket(functionCode::digitalControl, passedData); 
  }
}


void resolveAnalogControl(byte& passedID, byte& passedSubnet, byte& channel, byte& passedData)
{
   if((passedID == myID) && (passedSubnet == mySubnet) &&(deviceType == device::relayModule || deviceType == device::actuator))        //only dimmer relays and actuators can be analogcontrolled not sensors
  {
    //get channel data to know which port to adjust parameter and parameter value from data, it can be from 0 to 255;
    //update channel state
    //sendPacket(functionCode::digitalControl,channelState); 
  }
}


void resolveIDchange(byte& passedID, byte& passedSubnet, byte& passedData)
{
   if(passedID == myMAC)                                                                                                                 //MAC is used for ID change, once the server realises that the function code is discovery, it treats the first byte as MAC
  {                                                                                                                                      //The master sends the new subnet under subnet,and the new device ID is stored in Data
    myID = passedData;                                                                                                                   //update the myID variable because, it's used in the sendPacket() function
    mySubnet = passedSubnet;                                                                                                             //update the mySubnet variable because, it's used in the sendPacket() function
    EEPROM.update(ID_ADDRESS, myID);                                                                                                     //update the EEEPROM for ID
    EEPROM.update(SUBNET_ADDRESS, mySubnet);                                                                                             //update the EEEPROM for subnet
    
    sendPacket(functionCode::changeID);                                                                                                  //for a change ID message response, server responds with no data but only the function code of changeID.
  }

                                                                                                                                         //maybe blink the led to signify change
}
