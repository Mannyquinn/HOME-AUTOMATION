#define DEBUG
/* DEFINITIONS FOR SERIAL OUTPUT*/
#define BAUDRATE 115200
#define TX_PIN 9
#define RX_PIN 8
#define SUCCESS 0xFF
#define FAILURE 0x00
/*ENABLE DEBUGGING*/
#ifdef DEBUG
  #include<SoftwareSerial.h>
  SoftwareSerial mySerial(RX_PIN, TX_PIN);   
#endif

#define MAX_FRAME_SIZE 15
#define MAX_DATA 8
/*DEVICE PARAMETERS*/
byte myID = 0X01;
byte mySubnet = 0X02;

/*POSSIBLE DATA RECEIVED*/
enum controlFields
{
  sys = 0x01,                               //system
  scene = 0x02, //state requested
  digitalControl = 0x03,
  analogControl = 0x04,
  alarm = 0x05,
  ack = 0x06,
  broadcast = 0x07,
  sensor = 0x08
};

/*RESOLUTION FUNCTION*/
void resolveFunction(byte*),
     ResolveDigitalControl(byte&, byte&, byte*),
     resolveScene(),
     resolveAnalogControl(),
     resolveBroadcast(),
     resolveSensor();

/* USART FUNCTION DECLARATIONS */
byte sendPacket(byte, uint16_t,byte, byte, byte);
byte readDeviceState(void);

/*CONTAINERS FOR UART DATA*/                                                        
volatile int dataFrameIndex = 0;
volatile byte dataFrameR[MAX_FRAME_SIZE];                 //buffer for incoming data
volatile bool dataCopied = false;

/*timer details*/
volatile unsigned  long timer = 0;
#define interByteGap 200UL
#define busDetectionPeriod 200UL
volatile unsigned long previousTime = 0, previousIteration = 0; 

enum busState : uint8_t
{
  busFree = (uint8_t)0x01, busBusy = (uint8_t)0x02
};

volatile busState busCurrentState = busState::busFree, 
         busPreviousState = busState::busFree;

volatile bool dataReady = false;
uint8_t frameLen; 
volatile unsigned long T =0;
volatile bool USARTReceiveError = false;
bool pendingData = false;

unsigned long ct, pt;
bool turn = 0;

#define DEPin PB2
