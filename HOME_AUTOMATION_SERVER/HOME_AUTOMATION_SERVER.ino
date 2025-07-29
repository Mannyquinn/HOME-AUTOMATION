#include<EEPROM.h>
#define DEBUG

#ifdef DEBUG
#include<SoftwareSerial.h>
#endif

#define BAUDRATE 115200
#define TX_PIN 9
#define RX_PIN 8
#define RESET 111                                           //set the reset button pin
#define GREEN_STATUS_LED 112
#define RED_STATUS_LED 113                                 //should come on if it hasn't been checked on in 10mins, meaning communication problems

#define SUCCESS 0XFF
#define FAILURE 0XFE
#define INITIALIZED 100

/*EEPROM ADDRESSES*/
#define INITIALIZED_ADDRESS 1
#define MAC_ADDRESS 2
#define ID_ADDRESS 3
#define SUBNET_ADDRESS 4
#define DEVICETYPE_ADDRESS 5
#define STATE_ADDRESS 6

byte myID;
byte myMAC;
byte mySubnet;
byte deviceState;                                           //this can be renamed as sensor readings for sensor
byte deviceType;

bool hasBeenInitialized; //get from EEPROM

enum functionCode
{
  discovery = 0x01,
  serve = 0x02,
  digitalControl = 0x03,
  analogControl = 0x04,
  changeID = 0x05,
  erorReport = 0x06
};

enum device
{
  gateway =0x00,
  controlPanel = 0x01,
  relayModule = 0x02,
  sensor = 0x03,
  IRcontroller = 0x04,
  actuator = 0x05
};

/* FUNCTION DECLARATIONS */
void startup(void);
byte sendPacket(byte, byte = 0);
byte* readPacket(void);
byte initialize(void);
void resolveFunction();
void resolveDiscovery(byte&, byte& channel);
void resolveServe(byte&, byte&);
void resolveDigitalControl(byte&, byte&, byte&, byte&);
void resolveAnalogControl(byte&, byte&, byte&, byte&);
void resolveIDchange(byte&, byte&, byte&);
void readDeviceState(void);

#ifdef DEBUG
SoftwareSerial mySerial(RX_PIN, TX_PIN);                                // RX, TX  serial communnication for MODBUS over UART over RS485
#endif

struct MAX487
{
  int RE_PIN, DE_PIN;
  MAX487(int, int);
  void enableTransmission();
  void disableTransmissionEnableReception();
  void shutdownDevice();
};

MAX487 Transceiver(13, 12);
void setup() 
{
  #ifdef DEBUG
  mySerial.begin(BAUDRATE);
  #endif
  
  Serial.begin(BAUDRATE);
  Transceiver.disableTransmissionEnableReception();
  startUp();
}

void loop() 
{

  resolveFunction();                                                        //resolve the incoming serial commuinication
  
}
