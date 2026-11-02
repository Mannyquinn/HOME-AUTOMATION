__attribute__((always_inline)) inline void INT0interruptSetup()
{
  /* THIS ENABLES THE INTERRUPT ON FALLING EDGE*/
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);
    EICRA &= ~(1 << ISC00);
    EICRA |= 1 << ISC01;
   
}

__attribute__((always_inline)) inline void enableINT0()
{
  EIMSK |= 1 << INT0;
}

__attribute__((always_inline)) inline void disableINT0()
{
  EIMSK &= ~(1 << INT0);
}

ISR(INT0_vect)
{
      /*This will trigger on the first low state of the bus
    if it is the start bit of the usart the usart will receive information
    if it is an error, after a defined time, the interrupt will be enabled again 
    */
     PORTB |= 1 << PB4;
    /*make sure the Data Enable pin is disabled and the Receive pin is enabled so we can receive */
     PORTB &=( 1 << DEPin); 

    /*disable the interrupt so it doesn't trigger on the bits of the USART incoming byte*/
     EIMSK &= ~(1 << INT0);
     busCurrentState = busState::busBusy;
     
     /* now reset the time, so that the bus detection period can start counting */
     previousTime = micros();
     PORTB &= ~(1 << PB4);
}     
