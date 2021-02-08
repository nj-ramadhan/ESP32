#include <Dynamixel2Arduino.h>

// Debug messages will appear on USB/serial monitor connection.
#define DEBUG_SERIAL Serial

// This is the other tab in the Arduino IDE
#include "ESP32SerialPortHandler.cpp"

// Port and pins specific to your ESP32 configuration.
#define DXL_SERIAL   Serial1
const uint8_t DXL_DIR_PIN = 4; //  DIR pin
const uint8_t DXL_RX_PIN = 39; //  RX PIN
const uint8_t DXL_TX_PIN = 32; //  TX PIN

#define MAX_BAUD  5
const int32_t baud[MAX_BAUD] = {57600, 115200, 1000000, 2000000, 3000000};

// The old way of creating dxl
//Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);

// New way of creating dxl
Dynamixel2Arduino dxl;

// Our custom handler with RX and TX pins specified.
ESP32SerialPortHandler esp_dxl_port(DXL_SERIAL, DXL_RX_PIN, DXL_TX_PIN, DXL_DIR_PIN);

const uint8_t DXL_ID = 0;
const float DXL_PROTOCOL_VERSION = 1.0;

void setup() {
  // put your setup code here, to run once:
  dxl.setPort(esp_dxl_port);
    
  // Use UART port of DYNAMIXEL Shield to debug.
  DEBUG_SERIAL.begin(115200);
  
  // Set Port baudrate to 57600bps. This has to match with DYNAMIXEL baudrate.
  dxl.begin(115200);
  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  // Get DYNAMIXEL information
  dxl.ping(DXL_ID);

  // Turn off torque when configuring items in EEPROM area
  dxl.torqueOff(DXL_ID);
  dxl.setOperatingMode(DXL_ID, OP_VELOCITY);
  dxl.torqueOn(DXL_ID);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // Please refer to e-Manual(http://emanual.robotis.com) for available range of value. 
  // Set Goal Velocity using RAW unit
  dxl.setGoalVelocity(DXL_ID, 200);
  delay(1000);
  // Print present velocity
  DEBUG_SERIAL.print("Present Velocity(raw) : ");
  DEBUG_SERIAL.println(dxl.getPresentVelocity(DXL_ID));
  delay(1000);

  // Set Goal Velocity using RPM
  dxl.setGoalVelocity(DXL_ID, 25.8, UNIT_RPM);
  delay(1000);
  DEBUG_SERIAL.print("Present Velocity(rpm) : ");
  DEBUG_SERIAL.println(dxl.getPresentVelocity(DXL_ID, UNIT_RPM));
  delay(1000);

  // Set Goal Velocity using percentage (-100.0 [%] ~ 100.0 [%])
  dxl.setGoalVelocity(DXL_ID, -10.2, UNIT_PERCENT);
  delay(1000);
  DEBUG_SERIAL.print("Present Velocity(ratio) : ");
  DEBUG_SERIAL.println(dxl.getPresentVelocity(DXL_ID, UNIT_PERCENT));
  delay(1000);
}
