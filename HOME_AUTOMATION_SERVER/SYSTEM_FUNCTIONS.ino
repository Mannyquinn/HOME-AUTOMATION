byte initialize()
{
  //the first time the device boots up, let it store these info in the EEPROM
  myID = (byte)10;
  myMAC = (byte)100 ;                                                                                         //read the dip inputs to get mac, hardcode it for now
  deviceState = (byte)HIGH;
  deviceType = (byte)device::relayModule;
  mySubnet = (byte)5;

  //now save the above to predefined locations in EEPROM
  EEPROM.write(INITIALIZED_ADDRESS, INITIALIZED);
  EEPROM.write(MAC_ADDRESS, myMAC);
  EEPROM.write(ID_ADDRESS, myID);
  EEPROM.write(SUBNET_ADDRESS, mySubnet);
  EEPROM.write(DEVICETYPE_ADDRESS, deviceType);
  EEPROM.write(STATE_ADDRESS, deviceState);

}


void startUp()
{
  hasBeenInitialized     = EEPROM.read(INITIALIZED_ADDRESS);
  if(hasBeenInitialized != INITIALIZED){initialize();}
  myID                   = EEPROM.read(ID_ADDRESS);
  myMAC                  = EEPROM.read(MAC_ADDRESS);
  mySubnet               = EEPROM.read(SUBNET_ADDRESS);
  deviceState            = EEPROM.read(STATE_ADDRESS);
  deviceType             = EEPROM.read(DEVICETYPE_ADDRESS);
}

byte sendPacket(byte functionCode, byte data = 0)
{                                                                                                       //clear serial buffer first
  Transceiver.enableTransmission();
  static byte dataframe[6];
  byte parity = 0XFF;
  dataframe[0] = myID;
  dataframe[1] = mySubnet;
  dataframe[2] = functionCode;
  dataframe[3] = 0;                                                                                     //master channel
  dataframe[4] = data;
  dataframe[5] = parity;
  Serial.write(dataframe, 6);                                                                           //sends ID first
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

void readDeviceState()
{
  //code to read device sensor data 
  deviceState = 200; //simulate data
}
