void resolveDigitalControl(byte& receiverID, byte& receiverSubnet, byte* data)                
{
  if(receiverID == myID && receiverSubnet == mySubnet)                                  //was the message addressed to me?
  {
    channel[ data[0] ] = data[1];                                                       //data[0] holdss the channel number. data[1] holds the new state
  }
}

void resolveSensor()
{

}

void resolveScene()
{
                                                                                                                                         
}
