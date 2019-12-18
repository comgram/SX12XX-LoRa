/*******************************************************************************************************
  LoRaTracker Programs for Arduino - Copyright of the author Stuart Robinson - 16/12/19

  http://www.LoRaTracker.uk

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

//*******  Setup hardware pin definitions here ! ***************

//These are the pin definitions for one of my own boards, the Easy Mikrobus Pro Mini, 
//be sure to change the definitiosn to match your own setup. Some pins such as DIO1,
//DIO2, BUZZER SWITCH1 may not be in used by this sketch so they do not need to be
//connected and should be set to -1.

#define NSS 10                                  //select on LoRa device
#define NRESET 9                                //reset on LoRa device
#define DIO0 3                                  //DIO0 on LoRa device, used for RX and TX done 
#define DIO1 -1                                 //DIO1 on LoRa device, normally not used so set to -1
#define DIO2 -1                                 //DIO2 on LoRa device, normally not used so set to -1
#define LED1 8                                  //On board LED, high for on

#define LORA_DEVICE DEVICE_SX1278               //this is the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 434000000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_010;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF8;       //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_8;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting

const int8_t TXpower = 10;                       //LoRa TX power in dBm, 10dBm = 10mW

#define packet_delay 2500                        //mS delay between transmissions

#define TXBUFFER_SIZE 64                         //TX buffer size


