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
const uint8_t DXL_ID_CNT = 2;
const uint8_t DXL_ID_LIST[DXL_ID_CNT] = {0, 1};
const uint16_t user_pkt_buf_cap = 128;
uint8_t user_pkt_buf[user_pkt_buf_cap];

const uint16_t SR_START_ADDR = 0;
const uint16_t SR_ADDR_LEN = 10; //2+4+4
const uint16_t SW_START_ADDR = 104; //Goal velocity
const uint16_t SW_ADDR_LEN = 4;
typedef struct sr_data{
  int16_t present_current;
  int32_t present_velocity;
  int32_t present_position;
} __attribute__((packed)) sr_data_t;
typedef struct sw_data{
  int32_t goal_velocity;
} __attribute__((packed)) sw_data_t;


sr_data_t sr_data[DXL_ID_CNT];
DYNAMIXEL::InfoSyncReadInst_t sr_infos;
DYNAMIXEL::XELInfoSyncRead_t info_xels_sr[DXL_ID_CNT];

sw_data_t sw_data[DXL_ID_CNT];
DYNAMIXEL::InfoSyncWriteInst_t sw_infos;
DYNAMIXEL::XELInfoSyncWrite_t info_xels_sw[DXL_ID_CNT];

// New way of creating dxl
Dynamixel2Arduino dxl;

// Our custom handler with RX and TX pins specified.
ESP32SerialPortHandler esp_dxl_port(DXL_SERIAL, DXL_RX_PIN, DXL_TX_PIN, DXL_DIR_PIN);

using namespace ControlTableItem;

void setup() {
  // put your setup code here, to run once:
  uint8_t i;
//  pinMode(LED_BUILTIN, OUTPUT);
  DEBUG_SERIAL.begin(115200);
  dxl.begin(115200);
  dxl.setPortProtocolVersion(1.0);
  
  for(i=0; i<DXL_ID_CNT; i++){
    dxl.torqueOff(DXL_ID_LIST[i]);
    dxl.setOperatingMode(DXL_ID_LIST[i], OP_VELOCITY);
    dxl.torqueOn(DXL_ID_LIST[i]);
  }

  // Fill the members of structure to syncRead using external user packet buffer
  sr_infos.packet.p_buf = user_pkt_buf;
  sr_infos.packet.buf_capacity = user_pkt_buf_cap;
  sr_infos.packet.is_completed = false;
  sr_infos.addr = SR_START_ADDR;
  sr_infos.addr_length = SR_ADDR_LEN;
  sr_infos.p_xels = info_xels_sr;
  sr_infos.xel_count = 0;  

  for(i=0; i<DXL_ID_CNT; i++){
    info_xels_sr[i].id = DXL_ID_LIST[i];
    info_xels_sr[i].p_recv_buf = (uint8_t*)&sr_data[i];
    sr_infos.xel_count++;
  }
  sr_infos.is_info_changed = true;


  // Fill the members of structure to syncWrite using internal packet buffer
  sw_infos.packet.p_buf = nullptr;
  sw_infos.packet.is_completed = false;
  sw_infos.addr = SW_START_ADDR;
  sw_infos.addr_length = SW_ADDR_LEN;
  sw_infos.p_xels = info_xels_sw;
  sw_infos.xel_count = 0;

  sw_data[0].goal_velocity = 0;
  sw_data[1].goal_velocity = 100;
  for(i=0; i<DXL_ID_CNT; i++){
    info_xels_sw[i].id = DXL_ID_LIST[i];
    info_xels_sw[i].p_data = (uint8_t*)&sw_data[i].goal_velocity;
    sw_infos.xel_count++;
  }
  sw_infos.is_info_changed = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t try_count = 0;
  uint8_t i, recv_cnt;
  
  for(i=0; i<DXL_ID_CNT; i++){
    sw_data[i].goal_velocity+=5;
    if(sw_data[i].goal_velocity >= 200){
      sw_data[i].goal_velocity = 0;
    }
  }
  sw_infos.is_info_changed = true;

  DEBUG_SERIAL.print("\n>>>>>> Sync Instruction Test : ");
  DEBUG_SERIAL.println(try_count++);
  if(dxl.syncWrite(&sw_infos) == true){
    DEBUG_SERIAL.println("[SyncWrite] Success");
    for(i=0; i<sw_infos.xel_count; i++){
      DEBUG_SERIAL.print("  ID: ");DEBUG_SERIAL.println(sw_infos.p_xels[i].id);
      DEBUG_SERIAL.print("\t Goal Velocity: ");DEBUG_SERIAL.println(sw_data[i].goal_velocity);
    }
  }else{
    DEBUG_SERIAL.print("[SyncWrite] Fail, Lib error code: ");
    DEBUG_SERIAL.print(dxl.getLastLibErrCode());
  }
  DEBUG_SERIAL.println();

  delay(250);

  recv_cnt = dxl.syncRead(&sr_infos);
  if(recv_cnt > 0){
    DEBUG_SERIAL.print("[SyncRead] Success, Received ID Count: ");
    DEBUG_SERIAL.println(recv_cnt);
    for(i=0; i<recv_cnt; i++){
      DEBUG_SERIAL.print("  ID: ");DEBUG_SERIAL.print(sr_infos.p_xels[i].id);
      DEBUG_SERIAL.print(", Error: ");DEBUG_SERIAL.println(sr_infos.p_xels[i].error);
      DEBUG_SERIAL.print("\t Present Current: ");DEBUG_SERIAL.println(sr_data[i].present_current);
      DEBUG_SERIAL.print("\t Present Velocity: ");DEBUG_SERIAL.println(sr_data[i].present_velocity);
      DEBUG_SERIAL.print("\t Present Position: ");DEBUG_SERIAL.println(sr_data[i].present_position);
    }
  }else{
    DEBUG_SERIAL.print("[SyncRead] Fail, Lib error code: ");
    DEBUG_SERIAL.println(dxl.getLastLibErrCode());
  }
  DEBUG_SERIAL.println("=======================================================");

//  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(750);
}
