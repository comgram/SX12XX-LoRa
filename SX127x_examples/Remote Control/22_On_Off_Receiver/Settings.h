/*******************************************************************************************************
  LoRaTracker Programs for Arduino - Copyright of the author Stuart Robinson - 02/12/19

  http://www.LoRaTracker.uk

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/


//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Mikrobus Pro Mini, 
//be sure to change the definitiosn to match your own setup. Some pins such as DIO1,
//DIO2, may not be in used by this sketch so they do not need to be connected and
//should be set to -1.

const int8_t NSS = 10;                          //select on LoRa device
const int8_t NRESET = 9;                        //reset on LoRa device
const int8_t DIO0 = 3;                          //DIO0 on LoRa device, used for RX and TX done 
const int8_t DIO1 = -1;                         //DIO1 on LoRa device, normally not used so set to -1
const int8_t DIO2 = -1;                         //DIO2 on LoRa device, normally not used so set to -1
const int8_t LED1 = 8;                          //On board LED, logic high is on

#define LORA_DEVICE DEVICE_SX1278               //this is the device we are using

const int8_t OUTPUT0 = 2;
const int8_t OUTPUT1 = 4;
const int8_t OUTPUT2 = A3;

const uint32_t RXIdentity = 1234554321;         //define an identity number, the receiver must use the same number
                                                //range is 0 to 4294967296

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_125;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF7;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_5;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting

