void setupUSART()
{
  /*set usart in double speed mode*/
  UCSR0A = (1 << U2X0);

  /*DISABLE GLOBAL INTERRUPT*/
  cli();
  
  /*ENABLE INTERRUPT WHEN RECEIVE IS COMPLETE*/
  UCSR0B |= 1 << RXCIE0; 

  /*ENABLE GLOBAL INTERRUPT*/
  sei();

  /*ENABLE TRANSMIT AND RECEIVE PINS*/
  UCSR0B |= 1 << TXEN0 | 1 << RXEN0;

  /*set the data bit size to 8 bits */
  UCSR0B &= ~(1 << UCSZ02);
  UCSR0C |= 1 << UCSZ01 | 1 << UCSZ00;

  /* set the operation to Asynchronous Uart*/
  UCSR0C &= ~(1 << UMSEL01 | 1 << UMSEL00);

  /*set the parity bit to even*/
  UCSR0C &= ~(1 << UPM00);
  UCSR0C |= (1 << UPM01);

  /* set clock polaity to 0 to disable it*/
  UCSR0C &= ~(1 << UCPOL0);
  
  /*set stop bit to be 1 bits*/
  UCSR0C &= ~(1 << USBS0);
  
  /* set baud rate to 100k */
  UBRR0 = (byte)19;
}

void USARTsend(uint8_t message)
{
  /*if data register is empty*/
  while(!(UCSR0A & 0x20)){}
  
   /* load the message into the data register to begin transmission */
//   if(busCurrentState == busState::busFree)  
    PORTB |= 1 << DEPin;
    UDR0 = message;
  
}

/*INTERRUPT TRIGGERED WHEN MESSAGE IS AVAILABLE*/
ISR(USART_RX_vect)
{
  /*if the time between this packet and the last one is large, this is a new packet
    if it isn't continue the old packet
  */
  T = micros();
  uint8_t status = UCSR0A;
  uint8_t data   = UDR0;
  
 if(T - previousIteration >= interByteGap)  
  {
     dataFrameIndex = 0;
     dataCopied = false;
     dataReady = false;
     USARTReceiveError = false;
  }
  
  /*check for overflow, parity and stop bit error*/
     if (status & ((1<<FE0) | (1<<UPE0))) {
        USARTReceiveError = true;
    }
        /*move the incoming data into the array increment the array index*/
    if(dataFrameIndex < MAX_FRAME_SIZE)
    {
      dataFrameR[dataFrameIndex] = data;   
      dataFrameIndex++ ;  
    }

  
  /*we are expecting 9 bytes, once they are done we know the frame is complete*/
  if(dataFrameIndex == 9)
  {
      dataReady = true;

      /*when the frame is finished, enable the INT0 ISR trigger again*/
      EIMSK |= (1 << INT0);
  }
  previousIteration = T;
  previousTime = timer;
  busCurrentState = busState::busBusy;
}
