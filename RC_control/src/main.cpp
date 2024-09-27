#include <Arduino.h>
#include "RadioLib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// BUTTON DEFINE
#define BUTTONPIN A4

//-------------JOYSTICKS----------------
#define JOY1X A0
#define JOY1Y A1
#define JOY1SW 7
#define JOY2X A2
#define JOY2Y A3
#define JOY2SW 6
//--------------------------------------

//----------------LORA-----------------
// PINS
#define NSS 10
#define DIO1 2
#define NRST 9
#define BUSY 8

// TRANSMISSION STRUCT
// Try to add CRC here and to receiver
struct __attribute__ ((packed)) transmitData {
  uint8_t controller_ID; // Figure something out to make it unique and secure
  uint8_t joy1x;
  uint8_t joy1y;
  uint8_t joy2x;
  uint8_t joy2y;
  bool joy1swState;
  bool joy2swState;
  bool btnState1;
  bool btnState2;
  bool btnState3;
  bool btnState4;
  bool btnState5;
  bool btnState6;
  bool btnState7;
  bool btnState8;
};

transmitData controlData;

// MESSAGE CONFIGS
SX1262 radio = new Module(NSS, DIO1, NRST, BUSY);
int transmissionState = RADIOLIB_ERR_NONE;

// FLAGS
volatile bool transmittedFlag = false;

// SETTER FUNCTION
void setFlagTransmit(void) {
  transmittedFlag = true;
}
//-------------------------------------

//-------------BUTTONS------------------
const int numButtons = 8; 
int buttonThresholds[numButtons] = {50, 150, 250, 350, 450, 550, 650, 750}; // SUBJECT TO CHANGE WITH TESTING
//--------------------------------------


void setup() {
  Serial.begin(9600);
  delay(1000); // Give some time for serial monitor to initialize
  pinMode(BUTTONPIN, INPUT);

  // EU SETTINGS
  Serial.print(F("[Transmitter] Initializing with EU settings... "));
  int state = radio.begin(868.0, 125.0, 6, 5, 0x34, 4, 20);
  
  // Check successful configuration
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); } // Hang to prevent further execution
  }

  // Set interrupt
  radio.setPacketSentAction(setFlagTransmit);
  Serial.print(F("[SX1262] Sending first packet ... "));
    
  transmissionState = radio.startTransmit("Hello World!");
  if (transmissionState == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(transmissionState);
    while (true) { delay(10); }
  }
}


void loop() {

    // BUTTON LADDER TEST FUNCTIONALITY
  int sensorValue = analogRead(BUTTONPIN);
  
  for (int i = 0; i < numButtons; i++) {
    if (sensorValue > buttonThresholds[i] && (i == numButtons - 1 || sensorValue <= buttonThresholds[i + 1])) {
      Serial.print("Button ");
      Serial.print(i + 1);
      Serial.println(" Pressed");
      break;
    }
  }
  if (transmittedFlag) {
    radio.finishTransmit();
    uint8_t* bytePtr = (uint8_t*)&controlData;
    size_t dataSize = sizeof(controlData); // Get the size of the struct
    transmittedFlag = false;

    Serial.print(F("[SX1262] Sending packet... "));

    // Send the packet
    transmissionState = radio.startTransmit(bytePtr, dataSize);
    if (transmissionState == RADIOLIB_ERR_NONE) {
      Serial.println(F("Transmission successful!"));
    } else {
      Serial.print(F("Transmission failed, code "));
      Serial.println(transmissionState);
    }
    
  }
  int value = analogRead(BUTTONPIN);
  Serial.println(value);
  delay(100);
}