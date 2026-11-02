void setup() 
{ 
  #ifdef DEBUG
  mySerial.begin(BAUDRATE);
  #endif
  
  setupUSART();
  INT0interruptSetup();
  enableINT0();
  DDRB |= 1 << DEPin;
  PORTB &= ~(1 << DEPin);

  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() 
{ 
  /* make sure you dont use delay, rather use non blocking methods*/  
  noInterrupts();
  timer = micros();
  unsigned long diff = timer  - previousTime;
  interrupts();
  /* detects that the receiver isn't getting any more information therefore we have gotten the last byte*/
 if(diff > busDetectionPeriod)                         
  {      
      noInterrupts();
      PORTB |=( 1 << PB3);
      busCurrentState = busState::busFree;
      EIMSK |= (1 << INT0);
      previousTime = timer;
      PORTB &=~( 1 << PB3);
      interrupts();
         
  }

  /*if there is avalable data just gotten from the bus, let us read*/
  if(dataReady && !dataCopied && !USARTReceiveError) 
  {   
    frameLen = dataFrameIndex;   
    if(frameLen == 9)
    {
      resolveFunction(dataFrameR);                          
    }
    else
    {
     #ifdef DEBUG
       // mySerial.print("FRIG: ");                   //frame ignored
       // mySerial.println(frameLen);
      #endif
    } 
  }

  ct = micros();
  if(ct - pt >= (unsigned long)200000)
  {
    if(turn == 0)
    {
      if(sendPacket(0x03, 0x0100, 0x03, 0x04, 0x02) == SUCCESS)
        turn = !turn;
    }
      
    else if(turn == 1)
    {
      if(sendPacket(0x03, 0x0000, 0x03, 0x04, 0x02) == SUCCESS)
        turn = !turn;
    }
    
    pt = ct;
  }
}
