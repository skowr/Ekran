/*
Freq 431,691392M

Up signal:

<Signal>    1000111011101000111011101110111011101000100010001000100011101000100010001000100010001000111010001 (18323 samples)
            1000111011101000111011101110111011101000100010001000100011101000100010001000100010001110100010001
                                                                                                 ^^?
            8ee8eeeee88888e8888888e88
            8ee8eeeee88888e888888e888
<Pause>     0000000000000000000000000000000 (5817 samples)

560 us == 3 bits == 567 samples
186 us == 1 bir == 187 samples

*/


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

/*
    std::string hexString = "FF"; // Example hex string    
    unsigned int result = hexToUIntLastFourBits(hexString);    
    std::cout << "Converted value with last 4 bits is: " << result << "\n";
    return 0;

}

*/

void loop() {
  jammer();

}
