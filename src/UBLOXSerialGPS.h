/*
  Copyright 2020 - Stuart Robinson
  Licensed under a MIT license displayed at the bottom of this document.
  Original published 27/06/20
*/

const PROGMEM  uint8_t ClearConfig[]  = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x19, 0x98}; //21
const PROGMEM  uint8_t SetBalloonMode[]  = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05,
                                            0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC
                                           }; //44
const PROGMEM  uint8_t SaveConfig[]  = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xA9}; //22
const PROGMEM  uint8_t SetCyclicMode[]  = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x01, 0x22, 0x92}; //10
const PROGMEM  uint8_t SoftwareBackup[]  = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};  //16
const PROGMEM  uint8_t PollNavigation[]  = {0xB5, 0x62, 0x06, 0x24, 0x00, 0x00, 0x2A, 0x84}; //8

//these are the commands to turn off NMEA sentences
const PROGMEM  uint8_t GLONASSOff[]  = {0xB5, 0x62, 0x06, 0x3E, 0x0C, 0x00, 0x00, 0x00, 0x20, 0x01, 0x06, 0x08, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x01, 0x8F, 0xB2}; //20
const PROGMEM  uint8_t GPGLLOff[]  = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A}; //16
const PROGMEM  uint8_t GPGLSOff[]  = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46}; //16
const PROGMEM  uint8_t GPGSAOff[]  = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31}; //16
const PROGMEM  uint8_t GPGSVOff[]  = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38}; //16

//response to PollNavigation should be B5 62 06 24 24 00 FF FF 06 03
//accepted response to configuaration command should be UBX-ACK-ACK B5 62 05 01
//fail response to configuaration command should be UBX-ACK-NAK B5 62 05 00

void GPS_OutputOn();
void GPS_OutputOff();
void GPS_PowerOn();
bool GPS_CheckConfiguration();
bool GPS_SendConfig(const uint8_t *Progmem_ptr, uint8_t arraysize, uint8_t replylength, uint8_t attempts);
bool GPS_WaitAck(uint32_t waitms, uint8_t length);
bool GPS_PollNavigation();

bool GPS_ClearConfig();
bool GPS_SaveConfig();
bool GPS_SetBalloonMode();
bool GPS_SetCyclicMode();
bool GPS_SoftwareBackup();
bool GPS_GLONASSOff();
bool GPS_GPGLLOff();
bool GPS_GPGLSOff();
bool GPS_GPGSAOff();
bool GPS_GPGSVOff();
bool GPS_CheckAck();

const uint32_t GPS_WaitAck_mS = 1000;            //number of mS to wait for an ACK response from GPS
const byte GPS_attempts = 5;                     //number of times the sending of GPS config will be attempted.
const byte GPS_Reply_Size = 16;                  //size of GPS reply buffer
const uint16_t GPS_Clear_DelaymS = 2000;         //mS to wait after a GPS Clear command is sent
uint8_t GPS_Reply[GPS_Reply_Size];              //byte array for storing GPS reply to UBX commands


#define UBLOXINUSE                              //so complier can know which GPS library is used
//#define GPSDebug

#ifndef GPSBaud
#define GPSBaud 9600
#endif


void GPS_OutputOn()
{
#ifdef GPSDebug
  Serial.print(F("GPS_On() "));
#endif
  uint8_t GPSchar = 0;
  //turns on serial output from GPS
  GPSserial.begin(GPSBaud);
  GPSserial.write(GPSchar);
}


void GPS_OutputOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_Off() "));
#endif

  GPSserial.end();                       //turns off serial output from GPS
}


void GPS_PowerOn(int8_t pin, uint8_t state)
{
#ifdef GPSDebug
  Serial.print(F("GPS_PowerOn() "));
#endif
  
  if (pin >= 0)
  {
  digitalWrite(pin, state); 
  }
  
}

 
void GPS_PowerOff(int8_t pin, uint8_t state)
{
#ifdef GPSDebug
  Serial.print(F("GPS_PowerOff() "));
#endif

if (pin >= 0)
  {
  digitalWrite(pin, state); 
  }

}



bool GPS_Setup()
{
#ifdef GPSDebug
  Serial.print(F("GPS_Setup() "));
#endif

  if (!GPS_ClearConfig())
  {
    return false;
  }

  if (!GPS_SetBalloonMode())
  {
    return false;
  }
  
  if (!GPS_CheckConfiguration())
  {
    return false;
  }

  if (!GPS_SaveConfig())
  {
    return false;
  }

  return true;
}


bool GPS_CheckConfiguration()
{
  //navigation model setting for UBLOX is at GPS_Reply[8] of poll request reply.

#ifdef GPSDebug
  Serial.print(F("GPS_CheckConfiguration() UBLOX "));
#endif

  byte j;
  GPS_Reply[8] = 0xff;

  GPS_PollNavigation();

  if ( (GPS_Reply[0] == 0xB5) && (GPS_Reply[1] == 0x62) &&  (GPS_Reply[2] == 0x06) && (GPS_Reply[3] == 0X24) )
  {
    j = GPS_Reply[8];

    if (j == 6)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  return false;

}


bool GPS_SendConfig(const uint8_t *Progmem_ptr, uint8_t arraysize, uint8_t replylength, uint8_t attempts)
{
#ifdef GPSDebug
  Serial.print(F("GPS_SendConfig() "));
#endif

  

  uint8_t byteread, index;
  uint8_t config_attempts = attempts;

  Serial.flush();                           //ensure there are no pending interrupts from serial monitor printing

  do
  {
    if (config_attempts == 0)
    {
      Serial.println(F("Fail"));
      Serial.println();
      return false;
    }

    GPS_OutputOff();
	
	Serial.print(F("GPSSend  "));

    for (index = 0; index < arraysize; index++)
    {
      byteread = pgm_read_byte_near(Progmem_ptr++);
      Serial.print(byteread, HEX);
      Serial.print(" ");
    }

    Serial.flush();          //make sure serial out buffer is empty

    GPS_OutputOn();

    Progmem_ptr = Progmem_ptr - arraysize;                  //set Progmem_ptr back to start

    for (index = 0; index < arraysize; index++)
    {
      byteread = pgm_read_byte_near(Progmem_ptr++);
      GPSserial.write(byteread);
    }

    Progmem_ptr = Progmem_ptr - arraysize;                  //set Progmem_ptr back to start

    if (replylength == 0)
    {
#ifdef GPSDebug
      Serial.println(F("Reply not required"));
#endif
      break;
    }

    config_attempts--;
  }
  while (!GPS_WaitAck(GPS_WaitAck_mS, replylength));
  
  Serial.println(F("OK"));
  Serial.println();
  delay(100);                                               //GPS can sometimes be a bit slow getting ready for next config
  
  return true;
}


bool GPS_WaitAck(uint32_t waitms, uint8_t length)
{
#ifdef GPSDebug
  Serial.print(F("GPS_WaitAck() "));
  Serial.print(F(" Reply length "));
  Serial.print(length);
  Serial.print(F(" "));
#endif

  //wait for Ack (UBX-ACK-ACK) response from GPS is 0xb5,0x62, 0x05, 0x01
  //Nack (UBX-ACK-NAK) response from GPS is 0xb5,0x62, 0x05, 0x00

  byte GPSchar;

  unsigned long endms;
  endms = millis() + waitms;
  byte ptr = 0;                             //used as pointer to store GPS reply

  Serial.println();
  Serial.print(F("Received "));
  Serial.flush();
  
  
  do
  {
    if (GPSserial.available())
    {
      GPSchar = GPSserial.read();
    }
  }
  while ((GPSchar != 0xb5) && (millis() < endms));   //use the timeout to ensure a lack of GPS does not cause the program to hang

  if (GPSchar != 0xb5)
  {
    Serial.println(F("Timeout Error"));
    return false;
  }

  GPS_Reply[ptr++] = 0xB5;                           //test if a 0xB5 has been received

  do
  {
    if (GPSserial.available())
    {
      GPS_Reply[ptr++] = GPSserial.read();
    }
  }
  while ((ptr < length) || (millis() >= endms));     //fill buffer, stop when either ptr is (length-1) or timeout


  if (millis() >= endms)                             //check for another timeout
  {
    Serial.print(F("NoReply Error"));
    return false;
  }

  GPS_OutputOff();

  for (ptr = 0; ptr < length; ptr++)
  {
    GPSchar = GPS_Reply[ptr];

    if (GPSchar < 0x10)
    {
      Serial.print(F("0"));
    }

    Serial.print(GPSchar , HEX);
    Serial.print(F(" "));
  }

  Serial.println();

  if (GPS_CheckAck())
  {
    return true;
  }

  return false;
}


bool GPS_PollNavigation()
{
#ifdef GPSDebug
  Serial.print(F("GPS_PollNavigation() "));
#endif
  Serial.println(F("PollNavigation"));
  size_t SIZE = sizeof(PollNavigation);
  if (GPS_SendConfig(PollNavigation, SIZE, 10, GPS_attempts))
  {
  return true;
  }
  return false;
}



bool GPS_CheckAck()
{

#ifdef GPSDebug
  Serial.print(F("GPS_CheckAck() "));
#endif

  if ((GPS_Reply[0] == 0xB5) && (GPS_Reply[1] == 0x62))
  {
#ifdef GPSDebug
    Serial.println(F(" UBX-ACK-ACK"));
#endif
    return true;                           //there has been a UBX-ACK-ACK response
  }
  else
  {
#ifdef GPSDebug
    Serial.println(F(" Not UBX-ACK-ACK "));
#endif
    return false;                           //there has been a UBX-ACK-ACK response
  }
}



/*********************************************************************
  // GPS configuration commands
*********************************************************************/


bool GPS_ClearConfig()
{
  #ifdef GPSDebug
  Serial.print(F("GPS_ClearConfig() "));
  #endif
  
  Serial.println(F("ClearConfig"));
  size_t SIZE = sizeof(ClearConfig);
  if (GPS_SendConfig(ClearConfig, SIZE, 10, GPS_attempts))
  {
    Serial.println(F("Wait clear"));
    Serial.println();
	delay(GPS_Clear_DelaymS);                            //wait a while for GPS to clear its settings
    return true;
  }

  return false;
}


bool GPS_SetBalloonMode()
{
#ifdef GPSDebug
  Serial.print(F("GPS_SetBalloonMode() "));
#endif

  Serial.println(F("SetBalloonMode"));
  size_t SIZE = sizeof(SetBalloonMode);
  if (GPS_SendConfig(SetBalloonMode, SIZE, 10, GPS_attempts))
  {
  return true;
  }

  return false;
}


bool GPS_SaveConfig()
{
#ifdef GPSDebug
  Serial.print(F("GPS_SaveConfig() "));
#endif

  Serial.println(F("SaveConfig"));
  size_t SIZE = sizeof(SaveConfig);
 
  if (GPS_SendConfig(SaveConfig, SIZE, 10, GPS_attempts))
  {
    return true;
  }

  return false;
}


bool GPS_SetCyclicMode()
{
#ifdef GPSDebug
  Serial.print(F("GPS_SetCyclicMode() "));
#endif

  Serial.println(F("SetCyclicMode"));
  size_t SIZE = sizeof(SetCyclicMode);
  
  if (GPS_SendConfig(SetCyclicMode, SIZE, 10, GPS_attempts))
  {
  return true;
  }

  return false;
}


bool GPS_SoftwareBackup()
{
#ifdef GPSDebug
  Serial.print(F("GPS_SoftwareBackup() "));
#endif

  Serial.println(F("SoftwareBackup"));
  size_t SIZE = sizeof(SoftwareBackup);
  
  if (GPS_SendConfig(SoftwareBackup, SIZE, 0, GPS_attempts))
  {
  return true;
  }

  return false;
}



bool GPS_HotStart()
  {
  #ifdef GPSDebug
  Serial.print(F("GPS_HotStart() "));
  #endif

  Serial.println(F("HotStart"));
  GPSserial.println();
  
  return true;  
  
} 



bool GPS_GLONASSOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GLONASSOff() "));
#endif

  size_t SIZE = sizeof(GLONASSOff);
  return GPS_SendConfig(GLONASSOff, SIZE, 10, GPS_attempts);
}


bool GPS_GPGLLOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GPGLLOff() "));
#endif

  size_t SIZE = sizeof(GPGLLOff);
  return GPS_SendConfig(GPGLLOff, SIZE, 10, GPS_attempts);
}


bool GPS_GPGLSOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GPGLSOff() "));
#endif

  size_t SIZE = sizeof(GPGLSOff);
  return GPS_SendConfig(GPGLSOff, SIZE, 10, GPS_attempts);
}


bool GPS_GPGSAOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GPGSAOff() "));
#endif

  size_t SIZE = sizeof(GPGSAOff);
  return GPS_SendConfig(GPGSAOff, SIZE, 10, GPS_attempts);
}


bool GPS_GPGSVOff()
{
#ifdef GPSDebug
  Serial.print(F("GPS_GPGSVOff() "));
#endif

  size_t SIZE = sizeof(GPGSVOff);
  return GPS_SendConfig(GPGSVOff, SIZE, 10, GPS_attempts);
}



/*
  MIT license

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions
  of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
  TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

