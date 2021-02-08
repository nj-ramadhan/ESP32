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

using namespace ControlTableItem; 

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
  
  // Turn on the LED on DYNAMIXEL
  dxl.ledOn(DXL_ID);
  delay(500);
  // Turn off the LED on DYNAMIXEL
  dxl.ledOff(DXL_ID);
  delay(500);

  // Turn off torque when configuring items in EEPROM area
  dxl.torqueOff(DXL_ID);
  dxl.setOperatingMode(DXL_ID, OP_POSITION);
  dxl.torqueOn(DXL_ID);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t key_input;
  DEBUG_SERIAL.println("Select Profile Type :");
  DEBUG_SERIAL.println("[1] Low Accel / High Vel");
  DEBUG_SERIAL.println("[2] Max Accel / Low Vel");
  DEBUG_SERIAL.println("[3] Max Accel / Max Vel");
  
  while(DEBUG_SERIAL.available()==0);
  key_input = DEBUG_SERIAL.read();

  switch(key_input) {
    case '1':
      // Low Profile Acceleration, High Profile Velocity
      // Refer to API documentation for available parameters
      // http://emanual.robotis.com/docs/en/parts/interface/dynamixel_shield/#dynamixelshieldv010-or-above
      dxl.writeControlTableItem(PROFILE_ACCELERATION, DXL_ID, 50);
      dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID, 300);
      break;
    case '2':
      // Max Profile Acceleration, Low Profile Velocity
      dxl.writeControlTableItem(PROFILE_ACCELERATION, DXL_ID, 0);
      dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID, 100);
      break;
    case '3':
      // Max Profile Acceleration, Max Profile Velocity
      // WARNING : Please BE AWARE that this option will make a vibrant motion for PRO(A) or PRO+ series that requires high current supply.
      dxl.writeControlTableItem(PROFILE_ACCELERATION, DXL_ID, 0);
      dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_ID, 0);
      break;
    default:
      break;
  }

  // Set Goal Position in RAW value
  dxl.setGoalPosition(DXL_ID, 0);
  delay(500);
  // Check if DYNAMIXEL is in motion
  while(dxl.readControlTableItem(MOVING, DXL_ID));
  
  // Set Goal Position in angle(degree)
  dxl.setGoalPosition(DXL_ID, 179.0, UNIT_DEGREE);
  delay(500);
  // Check if DYNAMIXEL is in motion
  while(dxl.readControlTableItem(MOVING, DXL_ID));
}
