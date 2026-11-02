byte sendPacket(byte controlField, uint16_t data = 0, byte receiverID = 0 ,byte receiverSubnet = 1, byte datasize = 2)                     
{ 
    byte dataFrameT[9];
    byte parity = 0XFF;                                                                                                  
    dataFrameT[0] = myID; 
    dataFrameT[1] = mySubnet; 
    dataFrameT[2] = receiverID; 
    dataFrameT[3] = receiverSubnet;
    dataFrameT[4] = controlField;
    dataFrameT[5] = datasize;
    dataFrameT[6] = data & 0xFF;                                                       //LSB of data represents channel
    dataFrameT[7] = data >> 8 & 0xFF;                                                 //MSB of data represents value
    dataFrameT[8] = parity;
    
    /*disable USART receive interrupt */
    UCSR0B &= ~(1 << RXCIE0);  
    
    /*transmit the data*/
    for(uint8_t i = 0; ((i < 9) && busCurrentState == busState::busFree); i++)
    { 
      USARTsend(dataFrameT[i]);
    }
    
    if(busCurrentState == busState::busBusy)
    {
      /*enable USART receiving interrupt */
      UCSR0B |= (1 << RXCIE0); 
  
      /*disable transmission and enable reception*/
      PORTB &= ~(1 << DEPin);
      return FAILURE;
    }
    
    /*waits for all things in the buffer to be sent first*/
    while(!(UCSR0A & (1 << TXC0))){}
    
    /*enable USART receiving interrupt */
    UCSR0B |= (1 << RXCIE0);   
    
    /*disable transmission and enable reception*/
    PORTB &= ~(1 << DEPin);
    
   
    return SUCCESS;
}



void resolveFunction(byte *dataFramePtr)
{
  static byte dataArray[8];                                //maximum amount of sendable data is 8 byte
  static byte senderID, senderSubnet, receiverID, receiverSubnet, controlField, dataSize, CRC;  
              senderID = dataFramePtr[0],
              senderSubnet = dataFramePtr[1],
              receiverID = dataFramePtr[2],
              receiverSubnet = dataFramePtr[3],
              controlField = dataFramePtr[4],
              dataSize = dataFramePtr[5],
              CRC = dataFramePtr[dataSize + 6];      //plus 6 to offset for the data
  if(dataSize > MAX_DATA) 
  {
    #ifdef DEBUG
    mySerial.print("CRDA: ");
    mySerial.println(dataSize);
    #endif
    return;
  }
  for(int i = 0; i < dataSize; i++)
  {
    dataArray[i] = dataFramePtr[6+i];
  }

  dataCopied = true;
 
    #ifdef DEBUG
    {
      mySerial.print("sID: ");
      mySerial.println((byte)senderID);
      mySerial.print("s SN: ");
      mySerial.println((byte)senderSubnet);
      mySerial.print("rID: ");
      mySerial.println((byte)receiverID);
      mySerial.print("rSN: ");
      mySerial.println((byte)receiverSubnet);
      mySerial.print("CF: ");
      mySerial.println((byte)controlField);
      mySerial.print("DS: ");
      mySerial.println((byte)dataSize);
      mySerial.flush();
      mySerial.print("FD: ");
      mySerial.println((byte)dataArray[0]);
      mySerial.print("SD: ");
      mySerial.println((byte)dataArray[1]);
      mySerial.print("pa: ");
      mySerial.println((byte)CRC);
    }
  #endif

  switch(controlField)                                                                                    //resolve each function code and call the appropriate function
  {
         
    case (byte)controlFields::scene:
         resolveScene();
        break;
     
    case (byte)controlFields::digitalControl:
         resolveDigitalControl(receiverID, receiverSubnet, dataArray);
        break;

    case (byte)controlFields::sensor:
         resolveSensor();
        break;
        
          default:
        break;
  }
} 
