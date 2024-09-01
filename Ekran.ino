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

  // Jammer code
  // c = 0;
  // b = true;

  testCalc();

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


unsigned int hexToUIntLastFourBits(const String& hexString) {
    uint16_t number = strtol(hexString.c_str(), nullptr, 16);    
    return number & 0b1111;

}

unsigned char hexToBinary(char hex) {
  if (isdigit(hex))
    return hex - '0';
  else
    return hex - 'A' + 10;
}

void hexStringToBooleanArray(const char* hexString, bool booleanArray[]) {
  for (int i = 0; i < strlen(hexString); i++){
    // Serial.println(hexString[i], DEC);
    unsigned char byte = hexToBinary(hexString[i]);
    // Serial.println(byte, DEC);
    for (int j = 1; j <= 4; j++){
      booleanArray[i*4+4-j] = ((byte & 1) == 1) ? true : false;
      byte >>= 1;
    }
  }
}


void loop() {
  // jammer();

}
