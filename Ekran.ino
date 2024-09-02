#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>

#include "config.h"


uint32_t c;
bool b;


void setup() {                

  pinMode(FS1000A_DATA_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  bool bolUp[strlen(sigUp)*4];
  bool bolDown[strlen(sigDown)*4];
  bool bolPause[strlen(sigPause)*4];  

  hexStringToBooleanArray(sigUp, bolUp);
  hexStringToBooleanArray(sigDown, bolDown);
  hexStringToBooleanArray(sigPause, bolPause);

  for (int i=0; i<PULSE_REPEATS; i++ ){
    trSend(bolDown, sizeof(bolDown));
    trWait(sizeof(bolPause));

  }

}


void testCalc() {

  Serial.begin(9600);
  Serial.println("\n\nStart");
  
  
  // char* str = "FFF";
  // bool bol[strlen(str)*4];

  bool bol[strlen(sigUp)*4];

  hexStringToBooleanArray(sigUp, bol);

  for (int i =0; i< sizeof(bol); i++)
  {
    if (bol[i])
      Serial.print("1");
    else
      Serial.print("0");

  }


}

void jammer() {
  digitalWrite(FS1000A_DATA_PIN, LOW);
  delayMicroseconds(JAM_PULSE);
  digitalWrite(FS1000A_DATA_PIN, HIGH);
  delayMicroseconds(JAM_PULSE);

  c += JAM_PULSE * 2;

  if (b)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (c >= JAM_BLINK)
  {
    b = !b;
    c = 0;
  }

}

// Helper functions

unsigned char hexToBinary(char hex) {
  if (isdigit(hex))
    return hex - '0';
  else
    return hex - 'A' + 10;
}

void hexStringToBooleanArray(const char* hexString, bool booleanArray[]) {
  for (int i = 0; i < strlen(hexString); i++){
    unsigned char byte = hexToBinary(hexString[i]);
    for (int j = 1; j <= 4; j++){
      booleanArray[i*4+4-j] = ((byte & 1) == 1) ? true : false;
      byte >>= 1;
    }
  }
}

// Send signal
void trSend(bool* bol, int size)
{

  for(int i = 0; i<size; i++){
    if (bol[i])
      digitalWrite(FS1000A_DATA_PIN, HIGH);
    else
      digitalWrite(FS1000A_DATA_PIN, LOW);

    delayMicroseconds(PULSE_BIT);
  }
}

// Wait given number of samples
void trWait(int size)
{
  // Pause
  digitalWrite(FS1000A_DATA_PIN, LOW);
  for(int i = 0; i<size; i++)
    delayMicroseconds(PULSE_BIT);
}


void loop() {
  // No loop now.

}
