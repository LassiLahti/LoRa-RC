#include <Arduino.h>
#include "RadioLib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NSS 10
#define DIO1 2
#define NRST 9
#define BUSY 8

// TRANSMITTER STRUCT
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

transmitData dataToSend;

// LORA MESSAGE CONFIGS
SX1262 radio = new Module(NSS, DIO1, NRST, BUSY);
int transmissionState = RADIOLIB_ERR_NONE;

// FLAGS
volatile bool receivedFlag = false;

// counter to keep track of transmitted packets
int count = 0;

// SETTER FUNCTIONS
void setFlag(void) {
  receivedFlag = true;
}

void setup() {
  Serial.begin(9600);
  delay(1000); // Give some time for serial monitor to initialize

  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin(868.0, 125.0, 6, 5, 0x34, 4, 20);
  
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); } // Hang to prevent further execution
  }
  // RECEIVER
  radio.setPacketReceivedAction(setFlag);
  Serial.print(F("[SX1262] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

}

void loop(){
  // Check if the flag is set
  if (receivedFlag) {
    // Reset flag
    receivedFlag = false;

    // Create a buffer to hold received data
    uint8_t receivedBytes[sizeof(transmitData)];
    int state = radio.readData(receivedBytes, sizeof(receivedBytes));

    if (state == RADIOLIB_ERR_NONE) {
      // Cast the received bytes to the transmitData struct
      transmitData* receivedData = (transmitData*)receivedBytes;

      // Print the received data
      Serial.print("Controller ID: ");
      Serial.println(receivedData->controller_ID);

      Serial.print("Joystick 1 X: ");
      Serial.println(receivedData->joy1x);

      Serial.print("Joystick 1 Y: ");
      Serial.println(receivedData->joy1y);

      Serial.print("Joystick 2 X: ");
      Serial.println(receivedData->joy2x);

      Serial.print("Joystick 2 Y: ");
      Serial.println(receivedData->joy2y);

      Serial.print("Joystick 1 Switch State: ");
      Serial.println(receivedData->joy1swState ? "Pressed" : "Not Pressed");

      Serial.print("Joystick 2 Switch State: ");
      Serial.println(receivedData->joy2swState ? "Pressed" : "Not Pressed");

      Serial.print("Button 1 State: ");
      Serial.println(receivedData->btnState1 ? "Pressed" : "Not Pressed");

      Serial.print("Button 2 State: ");
      Serial.println(receivedData->btnState2 ? "Pressed" : "Not Pressed");

      Serial.print("Button 3 State: ");
      Serial.println(receivedData->btnState3 ? "Pressed" : "Not Pressed");

      Serial.print("Button 4 State: ");
      Serial.println(receivedData->btnState4 ? "Pressed" : "Not Pressed");

      Serial.print("Button 5 State: ");
      Serial.println(receivedData->btnState5 ? "Pressed" : "Not Pressed");

      Serial.print("Button 6 State: ");
      Serial.println(receivedData->btnState6 ? "Pressed" : "Not Pressed");

      Serial.print("Button 7 State: ");
      Serial.println(receivedData->btnState7 ? "Pressed" : "Not Pressed");

      Serial.print("Button 8 State: ");
      Serial.println(receivedData->btnState8 ? "Pressed" : "Not Pressed");

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // Packet was received, but is malformed
      Serial.println(F("CRC error!"));

    } else {
      // Some other error occurred
      Serial.print(F("Failed, code "));
      Serial.println(state);

    }
  }
}