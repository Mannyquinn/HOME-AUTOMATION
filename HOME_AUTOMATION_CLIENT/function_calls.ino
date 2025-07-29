void startUp()
{
  hasBeenInitialized = EEPROM.read(INITIALIZED_ADDRESS);
  if(hasBeenInitialized != INITIALIZED){initialize();}
  myID        = EEPROM.read(ID_ADDRESS);
  myMAC       = EEPROM.read(MAC_ADDRESS);
  mySubnet    = EEPROM.read(SUBNET_ADDRESS);
  deviceState = EEPROM.read(STATE_ADDRESS);
  deviceType  = EEPROM.read(DEVICETYPE_ADDRESS);
}

byte sendPacket(byte functionCode, byte data = 0)
{                                                                                                       //clear serial buffer first
  Transceiver.enableTransmission();
  static byte dataframe[6];
  byte parity = 0xFF;
  dataframe[0] = 100;
  dataframe[1] = 5; // server subnet
  dataframe[2] = functionCode;
  dataframe[3] = 1;         //channel1                                                                              //master channel
  dataframe[4] = data;
  dataframe[5] = parity;
  Serial.write(dataframe, 6); //sends ID first
  Serial.flush();                                                                                       //waits for all things in the buffer to be sent first
  Transceiver.disableTransmissionEnableReception();
  return SUCCESS;
}

byte* readPacket()
{
  static byte dataframe[6];
  while(Serial.available() < 6);
  Serial.readBytes(dataframe, 6);
  return dataframe;
}

byte initialize()
{
  //the first time the device boots up, let it store these info in the EEPROM
  myID = (byte)101;
  myMAC = (byte)50 ;                                                                                         //read the dip inputs to get mac, hardcode it for now
  deviceState = (byte)HIGH;
  deviceType = (byte)device::controlPanel;
  mySubnet = (byte)5;

  //now save the above to predefined locations in EEPROM
  EEPROM.write(INITIALIZED_ADDRESS, INITIALIZED);
  EEPROM.write(MAC_ADDRESS, myMAC);
  EEPROM.write(ID_ADDRESS, myID);
  EEPROM.write(SUBNET_ADDRESS, mySubnet);
  EEPROM.write(DEVICETYPE_ADDRESS, deviceType);
  EEPROM.write(STATE_ADDRESS, deviceState);

}

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
         resolveDiscovery(ID,channel);
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
  if(passedChannel == 0)                                   //MAC is used for discovery not device ID, once the server realises that the function code is discovery, it treaths the first byte as MAC
  {
    //device discovered, write to discovered devices array,
  }
}

void resolveServe(byte& passedID, byte& passedSubnet)     //a request to serve information has been issued, server must send sensor informations or IO states
{
  if(passedID == myID && passedSubnet == mySubnet)        //don't forget to always make sure that we confirm the subnet mask
  {
    sendPacket(functionCode::serve, deviceState);         //send sensor readings or digital states of relay, each bit in the device state represents a port state
  }
}


void resolveDigitalControl(byte& passedID, byte& passedSubnet, byte& channel, byte& passedData) //a request to control a digital port has been issued, server must adhare to it and send a reponse to confim conformity
{
  if((passedID == myID) && (passedSubnet == mySubnet) &&(deviceType == device::relayModule || deviceType == device::actuator)) //only relays and actuators have digitally controlled IO not sensors, use subnet and ID to confirm which device is being addressed
  {
    //get channel to toggle from channel and state to toggle from data, data can only be 1 or 0
    //update channel state to passed data and return it
    sendPacket(functionCode::digitalControl, passedData); 
  }
}


void resolveAnalogControl(byte& passedID, byte& passedSubnet, byte& channel, byte& passedData)
{
   if((passedID == myID) && (passedSubnet == mySubnet) &&(deviceType == device::relayModule || deviceType == device::actuator)) //only dimmer relays and actuators can be analogcontrolled not sensors
  {
    //get channel data to know which port to adjust parameter and parameter value from data, it can be from 0 to 255;
    //update channel state
    //sendPacket(functionCode::digitalControl,channelState); 
  }
}


void resolveIDchange(byte& passedID, byte& passedSubnet, byte& passedData)
{
   if(passedID == myMAC)                                   //MAC is used for discovery not device ID, once the server realises that the function code is discovery, it treaths the first byte as MAC
  {
    myID = passedData;                                     //update the myID variable because, it's used in the sendPacket() function
    mySubnet = passedSubnet;                               //update the mySubnet variable because, it's used in the sendPacket() function
    EEPROM.update(ID_ADDRESS, myID);                       //update the EEEPROM for ID
    EEPROM.update(SUBNET_ADDRESS, mySubnet);               //update the EEEPROM for subnet
    
    sendPacket(functionCode::discovery, deviceType);      //for a discovery message response, server responds with its ID, subnet, discovery functioncode and the data is the devicetype the devicetype is the data response
  }
}


void readDeviceState()
{
  //code to read device sensor data 
  deviceState = 200; //simulate data
}
