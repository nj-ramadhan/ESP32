#include "qrcode.h"

void setup() {
    Serial.begin(115200);
 
    // Start time
    uint32_t dt = millis();
  
    // Create the QR code
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    String myString = String("3-" + String(27));
    int str_len = myString.length() + 1; 
    char qrString[str_len];
    myString.toCharArray(qrString, str_len);
    qrcode_initText(&qrcode, qrcodeData, 3, 0, qrString);
  
    // Delta time
    dt = millis() - dt;
    Serial.print("QR Code Generation Time: ");
    Serial.print(dt);
    Serial.print("\n");
 
    for (uint8_t y = 0; y < qrcode.size; y++) {
        // Each horizontal module
        for (uint8_t x = 0; x < qrcode.size; x++) {
//            Serial.print(qrcode_getModule(&qrcode, x, y) ? "1": "0");
            Serial.print(qrcode_getModule(&qrcode, x, y) ? "\u2588":" ");
        }
        Serial.print("\n");
    }
}
 
void loop() {
  delay(50);
}
