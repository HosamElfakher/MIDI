/***************************includes*****************************/
#include "hw_mapping.h"
#include "sw_logical_mapping.h"

/**************************preprocessors************************/
#define MAX_DEBOUNCE_TIME_MS                 (50)

//#define DEBUG

/*************************functions decleration******************/
/*
 * @brief this function is used to turn ON/OFF a MIDI note
 * 
 * @param cmd     MIDI command
 * @param note    MIDI pitch
 * @param vel     Pitch velocity
 */
void TriggerNote(uint8_t cmd, uint8_t note, uint8_t vel);

/*************************Local variables***********************/
const uint8_t DirectKeys[MAX_DIRECT_KEYS_NUMBER] = {
                                                      KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8,
                                                      KEY9, KEY10, KEY11, KEY12, KEY13, KEY14, KEY15, KEY16,
                                                      KEY17, KEY18, KEY19, KEY20, KEY21, KEY22, KEY23, KEY24,
                                                      KEY25, KEY31, KEY32, KEY33, KEY34, KEY35, KEY36, KEY37,
                                                      KEY38, KEY39, KEY40, KEY40, KEY42, KEY43, KEY44, KEY45,
                                                      KEY46, KEY47, KEY48, KEY49
                                                    };

uint8_t KeysNoteStatus[MAX_DIRECT_KEYS_NUMBER][2], DetectedKeysCount;

uint8_t portA_currentStatus = 0,
        portA_lastStatus = 0,
        portAinstantReading;

        
uint32_t debounceTime;

uint8_t pinA_midiNoteMapping[8] = {PINA_0_NOTE, PINA_1_NOTE, PINA_2_NOTE, PINA_3_NOTE, PINA_4_NOTE, PINA_5_NOTE, PINA_6_NOTE, PINA_7_NOTE};

/***********************Functions defination*********************/
void setup() 
{
  uint8_t i;
  for(i = 0; i < MAX_DIRECT_KEYS_NUMBER; i++)
  {
    pinMode(DirectKeys[i], INPUT);
  }
  Serial.begin(31250);

  #ifdef DEBUG
  Serial1.begin(9600);
  #endif
}


/***************************************************************/
void loop() 
{
  /*Keys detection section*/
  portAinstantReading = PINA; 

  if(portAinstantReading != portA_lastStatus)
  {
    debounceTime = millis();
  }

  if((millis() - debounceTime) >= MAX_DEBOUNCE_TIME_MS)
  {
    if(portAinstantReading != portA_currentStatus)
    {
      uint8_t bitNum, mask;
      for(bitNum = 0; bitNum < 8; bitNum++)
      {
        mask = (1 << bitNum); 
        if((mask & portAinstantReading) != (mask & portA_currentStatus))
        {
          DetectedKeysCount++;
          KeysNoteStatus[bitNum][0] = pinA_midiNoteMapping[bitNum];
          KeysNoteStatus[bitNum][1] = (mask & portAinstantReading);
        }
      }
      portA_currentStatus = portAinstantReading;
      
    }
  }
  portA_lastStatus = portAinstantReading;
      
  /*Keys processign action*/
  if(DetectedKeysCount) //greater than zero
  {
    #ifdef DEBUG
    Serial1.write("Number of detected keys = ");
    Serial1.write(DetectedKeysCount);
    Serial1.write("\n");
    #endif
    while(DetectedKeysCount--) //Loop over the whole detected Keys
    {
      if(KeysNoteStatus[DetectedKeysCount][1]) //ON note
      {
        TriggerNote(0x90, KeysNoteStatus[DetectedKeysCount][0], 0x45);
      }
      else
      {
        TriggerNote(0x80, KeysNoteStatus[DetectedKeysCount][0], 0);
      }
    }
    DetectedKeysCount = 0; //double resetting 
  }//End of Keys detection processing

}

/***********************************************************/
void TriggerNote(uint8_t cmd, uint8_t note, uint8_t vel)
{
  Serial.write(cmd);
  Serial.write(note);
  Serial.write(vel);
}
