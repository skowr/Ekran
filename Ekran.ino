#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>

// *** I keep all the below contents in external secret file (config.h) file and not share it ***
#include "config.h"

// #define  FS1000A_DATA_PIN 11 // pin for FS1000A transmitter
// #define  DC5V_ANALOG_PIN 0 // pin for voltage read

// #define  JAM_PULSE 2000 // jammer pulse
// #define  JAM_BLINK 100000 // jammer data
// #define  SIGNAL_BLINK 50 // blink signal

// #define  PULSE_BIT 186 // microseconds
// #define  PULSE_REPEATS 60 // how many times the pulse is being pushed

// #define  VOLT_TRESHOLD 500 // treshold to trigger action
// #define  VOLT_CHANGE_TIME 1000 // miliseconds time of the voltage change to take action
// #define  LOOP_DELAY 500 // probing time for the voltage miliseconds

// // bool APP_DEBUG = true;
// bool APP_DEBUG = false; // should we send debug information on serial port
// bool BLINK_OPERATION = false; // should the Arduino led blink on operation

// char *sigUp =    "<PUT YOUR OWN SIGNAL>"; // Signal for going up
// char *sigDown =  "<PUT YOUR OWN SIGNAL>"; // Signal for going down
// char *sigPause = "<PUT YOUR OWN SIGNAL>"; // Pause signal

// bool bolUp[25*4];
// bool bolDown[25*4];
// bool bolPause[8*4];



// Test data
uint32_t c;
bool b;

enum VoltStats {
  VOLT_LOW,
  VOLT_GO_HIGH,
  VOLT_HIGH,
  VOLT_GO_LOW
};

VoltStats voltStat;
uint16_t voltChangeCounter;
bool blink = false;

void setup() {                
  uint16_t v;

  pinMode(FS1000A_DATA_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  hexStringToBooleanArray(sigUp, bolUp);
  hexStringToBooleanArray(sigDown, bolDown);
  hexStringToBooleanArray(sigPause, bolPause);

  voltChangeCounter = 0;

  v = analogRead(DC5V_ANALOG_PIN);
  if (v > VOLT_TRESHOLD)
    voltStat = VoltStats::VOLT_HIGH;
  else
    voltStat = VoltStats::VOLT_LOW;

  if (APP_DEBUG) {
    Serial.begin(9600); 
    Serial.print("Remote start!");
  }

  if (!BLINK_OPERATION)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

}

// Garbage functions
void testCalc() {

  Serial.begin(9600);
  Serial.println("\n\nStart");
  

  bool bol[strlen(sigUp)*4];

  hexStringToBooleanArray(sigUp, bol);

  for (int i =0; i< sizeof(bol); i++) {
    if (bol[i])
      Serial.print("1");
    else
      Serial.print("0");
  }
}

// Garbage jammer
void jammer() {
  digitalWrite(FS1000A_DATA_PIN, LOW);
  delayMicroseconds(JAM_PULSE);
  digitalWrite(FS1000A_DATA_PIN, HIGH);
  delayMicroseconds(JAM_PULSE);

  c += JAM_PULSE * 2;

  if (b) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (c >= JAM_BLINK)
  {
    b = !b;
    c = 0;
  }

}

// Helper functions HEX -> BIN
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
      booleanArray[(i+1)*4-j] = ((byte & 1) == 1) ? true : false;
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
void trWait(int size) {
  
  digitalWrite(FS1000A_DATA_PIN, LOW);
  for(int i = 0; i<size; i++)
    delayMicroseconds(PULSE_BIT);
}

// Blink signal function
void signalblink() {
  for (int i = 0; i<5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(SIGNAL_BLINK);
    digitalWrite(LED_BUILTIN, LOW);
    delay(SIGNAL_BLINK);
  }
}


void loop() {
  
  //  Check
  uint16_t v;
  v = analogRead(DC5V_ANALOG_PIN);

  // Turningn on start
  if ( v > VOLT_TRESHOLD && voltStat == VoltStats::VOLT_LOW) {
    voltChangeCounter = 0;
    voltStat = VoltStats::VOLT_GO_HIGH;
  }
  // Turningn off
  else if ( v < VOLT_TRESHOLD && voltStat == VoltStats::VOLT_HIGH) {
    voltChangeCounter = 0;
    voltStat = VoltStats::VOLT_GO_LOW;
  }
  // Turned on confirmed. Start signal
  else if ( v > VOLT_TRESHOLD && voltStat == VoltStats::VOLT_GO_HIGH) {
    voltChangeCounter += LOOP_DELAY;
    if (voltChangeCounter >= VOLT_CHANGE_TIME) {
      voltChangeCounter = 0;
      voltStat = VoltStats::VOLT_HIGH;
      signalblink();
      for (int i=0; i<PULSE_REPEATS; i++ ) {
        trSend(bolDown, sizeof(bolDown));
        trWait(sizeof(bolPause));
      }
    }
  }
  // Turned off confirmed. Stop signal
  else if ( v < VOLT_TRESHOLD && voltStat == VoltStats::VOLT_GO_LOW) {
    voltChangeCounter += LOOP_DELAY;
    if (voltChangeCounter >= VOLT_CHANGE_TIME) {
      voltChangeCounter = 0;
      voltStat = VoltStats::VOLT_LOW;
      signalblink();
      for (int i=0; i<PULSE_REPEATS; i++ ) {
        trSend(bolUp, sizeof(bolUp));
        trWait(sizeof(bolPause));
      }
    }
  }

  delay(LOOP_DELAY);

  // Regular blink
  if (BLINK_OPERATION){
    blink = !blink;
    if (blink)
      digitalWrite(LED_BUILTIN, HIGH);
    else
      digitalWrite(LED_BUILTIN, LOW);
  }


  // Debug
  if (APP_DEBUG) {
    Serial.print("Status : ");
    Serial.print(voltStat);
    Serial.print(" Volt : ");
    Serial.print(v);
    Serial.println();
  }
}
