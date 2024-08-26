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

  c = 0;
  b = true;
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
  for (int i = 0; i < strlen(hexString); ++i) {
    unsigned char byte1 = hexToBinary(hexString[i] >> 4);
    unsigned char byte2 = hexToBinary(hexString[i] & 0x0F);
    booleanArray[i] = (byte1 > 0 && byte2 > 0) ? false : true; // if any bit is set, Boolean value is false
  }
}


/*
const char* hexString = "ABCDEFG"; // replace with your hex string
bool booleanArray[strlen(hexString)];

hexStringToBooleanArray(hexString, booleanArray);

for (int i = 0; i < strlen(hexString); ++i) {
  Serial.print(booleanArray[i] ? "1" : "0"); // print the Boolean values as '0' or '1' to the 
serial monitor
}



*/


void loop() {
  jammer();

}
