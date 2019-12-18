/*******************************************************************************************************
  LoRaTracker Programs for Arduino - Copyright of the author Stuart Robinson - 17/12/19

  http://www.LoRaTracker.uk

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - The program receives a LoRa packet without using a processor buffer, the LoRa devices
  internal buffer is read direct for the received sensor data. 
  
  The sensor used in the matching '17_Sensor_Transmiter' program is a BME280 and the pressure, humidity,
  and temperature are being and received. There is also a 16bit value of battery mV and and a 8 bit status
  value at the end of the packet.

  When the program starts, the LoRa device is setup to set the DIO0 pin high when a packet is received, the
  Atmel processor is then put to sleep and will wake up when a packet is received. When a packet is received,
  its printed and assuming the packet is validated, the sensor results are printed to the serial monitor
  and screen. Between readings the sensor transmitter is put to sleep in units of 8 seconds using the Atmel
  processor internal watchdog.

  For the sensor data to be accepted as valid the folowing need to match;

  The 16bit CRC on the received sensor data must match the CRC value transmitted with the packet.
  The packet must start with a byte that matches the packet type sent, 'Sensor1'
  The RXdestination byte in the packet must match this node ID of this receiver node, defined by 'This_Node'

  In total thats 16 + 8 + 8  = 32bits of checking, so a 1:4294967296 chance (approx) that an invalid
  packet is acted on and erroneous values displayed.

  The pin definitions, LoRa frequency and LoRa modem settings are in the Settings.h file.

  Easy Mikrobus Pro Mini Sleep current, plain TX only = 89uA
  Bares bones Arduino sleep current, plain TX only = 6.5uA

  Serial monitor baud rate is set at 9600.
*******************************************************************************************************/

#include <SPI.h>
#include "SX127XLT.h"
#include "Settings.h"
#include <Program_Definitions.h>

#include <U8x8lib.h>                                        //get library here >  https://github.com/olikraus/u8g2 
//U8X8_SSD1306_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);    //use this line for standard 0.96" SSD1306
U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);       //use this line for 1.3" OLED often sold as 1.3" SSD1306

SX127XLT LT;

uint32_t RXpacketCount;          //count of all packets received
uint32_t ValidPackets;           //count of packets received with valid data
uint32_t RXpacketErrors;         //count of all packets with errors received
uint16_t errors;
uint8_t validcount;
bool packetisgood;

uint8_t RXPacketL;               //length of received packet
int8_t  PacketRSSI;              //RSSI of received packet
int8_t  PacketSNR;               //signal to noise ratio of received packet

uint8_t RXPacketType;
uint8_t RXDestination;
uint8_t RXSource;
float temperature;               //the BME280 temperature value
float pressure;                  //the BME280 pressure value
uint16_t humidity;               //the BME280 humididty value
uint16_t voltage;                //the battery voltage value
uint8_t statusbyte;              //a status byte, not currently used
uint16_t CRCvalue;               //the CRC value of the packet data


void loop()
{
  RXPacketL = LT.receiveSXBuffer(0, 0, WAIT_RX);   //returns 0 if packet error of some sort, no timeout set

  digitalWrite(LED1, HIGH);      //something has happened

  PacketRSSI = LT.readPacketRSSI();
  PacketSNR = LT.readPacketSNR();

  if (RXPacketL == 0)
  {
    packet_is_Error();
  }
  else
  {
    packet_Received_OK();                            //its a valid packet LoRa wise, but it might not be a packet we want
  }

  digitalWrite(LED1, LOW);
  Serial.println();
}


void packet_Received_OK()
{
  //a LoRa packet has been received, which has passed the internal LoRa checks, including CRC, but it could be from
  //an unknown source, so we need to check that its actually a sensor packet we are expecting, and has valid sensor data

  errors = 0;                         //keep a count of errors found in packet

  RXpacketCount++;
  Serial.print(RXpacketCount);
  Serial.print(F(",PacketsReceived,"));

  LT.startReadSXBuffer(0);
  RXPacketType = LT.readUint8();
  RXDestination = LT.readUint8();
  RXSource = LT.readUint8();

  /************************************************************************
    Highlighted section - this is where the actual sensor data is read from
    the packet
  ************************************************************************/
  temperature = LT.readFloat();             //the BME280 temperature value
  pressure = LT.readFloat();                //the BME280 pressure value
  humidity = LT.readUint16();               //the BME280 humididty value
  voltage = LT.readUint16();                //the battery voltage value
  statusbyte = LT.readUint8();              //a status byte, not currently used
  /************************************************************************/

  LT.endReadSXBuffer();

  printreceptionDetails();                   //print details of reception, RSSI etc
  Serial.println();

  errors = checkPacketValid();

  if (errors == 0)
  {
    Serial.println(F("  Packet is good"));
    ValidPackets++;
    printSensorValues();                     //print the sensor values
    Serial.println();
    printPacketCounts();                     //print count of valid packets and erors                  
    dispscreen1();
    Serial.println();
  }
  else
  {
  Serial.println(F("  Packet is not vlaid"));
  RXpacketErrors++;  
  }
}


uint8_t checkPacketValid()
{
  uint8_t errors = 0;

  if (RXPacketType != Sensor1)
  {
    errors++;
  }

  if (RXDestination != This_Node)
  {
    errors++;
  }

  if (!checkCRCvalue())
  {
    errors++;
  }

  Serial.println();
  Serial.print(F("Error Check Count = "));
  Serial.print(errors);
  return errors;
}


bool checkCRCvalue()
{
  uint16_t CRCSensorData;

  CRCSensorData = LT.CRCCCITTSX(0, (RXPacketL - 3), 0xFFFF);    //calculate the CRC of packet sensor data

  Serial.print(F("(CRC of Received sensor data "));
  Serial.print(CRCSensorData, HEX);
  Serial.print(F(")"  ));

  CRCvalue = ((LT.getByteSXBuffer(17) << 8) + (LT.getByteSXBuffer(16)));

  Serial.print(F("(CRC transmitted "));
  Serial.print(CRCvalue, HEX);
  Serial.print(F(")"  ));

  if (CRCvalue != CRCSensorData)
  {
    Serial.print(F(" Sensor Data Not Valid"));
    return false;
  }
  else
  {
    Serial.print(F(" Sensor Data is Valid"));
    return true;
  }

}


void printSensorValues()
{
  Serial.print(F("Temp,"));
  Serial.print(temperature, 1);
  Serial.print(F("c,Press,"));
  Serial.print(pressure, 0);
  Serial.print(F("Pa,Humidity,"));
  Serial.print(humidity);
  Serial.print(F("%,Voltage,"));
  Serial.print(voltage);
  Serial.print(F("mV,Status,"));
  Serial.print(statusbyte, HEX);
  Serial.print(F(",CRC,"));
  Serial.print(CRCvalue, HEX);
  Serial.flush();
}


void printreceptionDetails()
{
  Serial.print(F("RSSI"));
  Serial.print(PacketRSSI);
  Serial.print(F("dBm,SNR,"));
  Serial.print(PacketSNR);
  Serial.print(F("dB,Length,"));
  Serial.print(LT.readRXPacketL()); 
}


void printPacketCounts()
{
  Serial.print(F("ValidPackets,"));
  Serial.print(ValidPackets);
  Serial.print(F(",Errors,"));
  Serial.print(RXpacketErrors);  
}


void packet_is_Error()
{
  uint16_t IRQStatus;

  RXpacketErrors++;
  IRQStatus = LT.readIrqStatus();

  if (IRQStatus & IRQ_RX_TIMEOUT)
  {
    Serial.print(F("RXTimeout "));
  }
  else
  {
    errors++;
    Serial.print(F("PacketError "));
    printreceptionDetails();
    Serial.print(F(",IRQreg,"));
    Serial.print(IRQStatus, HEX);
    LT.printIrqStatus();
    Serial.println();
  }
}


void dispscreen1()
{
  //show sensor data on display
  disp.clearLine(0);
  disp.setCursor(0, 0);
  disp.print(F("Sensor "));
  disp.print(RXSource);
  disp.clearLine(1);
  disp.setCursor(0, 1);
  disp.print(temperature, 1);
  disp.print(F("c"));
  disp.clearLine(2);
  disp.setCursor(0, 2);
  disp.print(pressure, 0);
  disp.print(F("Pa"));
  disp.clearLine(3);
  disp.setCursor(0, 3);
  disp.print(humidity);
  disp.print(F("%"));
  disp.clearLine(4);
  disp.setCursor(0, 4);
  disp.print(voltage);
  disp.print(F("mV"));
  disp.clearLine(6);
  disp.setCursor(0, 6);
  disp.print(F("ValidPkts "));
  disp.print(ValidPackets);
  disp.setCursor(0, 7);
  disp.print(F("Errors "));
  disp.print(RXpacketErrors);
}


void led_Flash(uint16_t flashes, uint16_t delaymS)
{
  uint16_t index;

  for (index = 1; index <= flashes; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(delaymS);
    digitalWrite(LED1, LOW);
    delay(delaymS);
  }
}


void setup()
{
  pinMode(LED1, OUTPUT);
  led_Flash(2, 125);

  Serial.begin(9600);

  disp.begin();
  disp.setFont(u8x8_font_chroma48medium8_r);

  disp.clear();
  disp.setCursor(0, 0);
  disp.print(F("Check LoRa"));
  disp.setCursor(0, 1);

  SPI.begin();

  if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
  {
    disp.print(F("LoRa OK"));
    led_Flash(2, 125);
  }
  else
  {
    disp.print(F("Device error"));
    Serial.println(F("Device error"));
    while (1)
    {
      led_Flash(50, 50);                                 //long fast speed flash indicates device error
    }
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println(F("Receiver ready"));
  Serial.println();
}

