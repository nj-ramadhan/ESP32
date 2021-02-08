#include <WiFi.h>
#include <MQTT.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <HX711.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
WiFiClient net;
MQTTClient client;

//setting wifi
const char ssid[] = "Redmi5";
const char pass[] = "mayarheula";

//pin address
const int pinLed = 5;
const int px_inductive = 34;
const int px_capacitive = 32;
const int px_optic = 33;
const int pinKunciLogam = 25;
const int pinKunciPlastik = 26;
const int pinKunciCampur = 27;

//local variable
int sesi_logam = 0;
int sesi_plastik = 0;
int sesi_campur = 0;
int sesi_poin = 0;

//cloud variable
int total_logam;
int total_plastik;
int total_campur;

int kunci_logam;
int kunci_plastik;
int kunci_campur;

int login;

//temp variable to handle debounce
bool logamState;             // the current reading from the input pin
bool lastLogamState = LOW;   // the previous reading from the input pin
bool plastikState;             // the current reading from the input pin
bool lastPlastikState = LOW;   // the previous reading from the input pin
bool adaState;             // the current reading from the input pin
bool lastAdaState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTimeLogam = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTimePlastik = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTimeAda = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 1000; 
unsigned long lastMillis = 0;

#define DELAY_OPEN  5000 // Delay open setting
#define POIN_LOGAM  70 // metal trash gain
#define POIN_PLASTIK  40 // plastic trash gain
#define POIN_CAMPUR  5 // mixed trash gain

#define SERVOMIN  300 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  150 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  1200 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 500 // Analog servos run at ~50 Hz updates

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nconnecting...");
  while (!client.connect("iotrash_hardware", "nj_ramadhan_iotrash_001", "iotrash_001_admin")) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nconnected!");
  client.subscribe("/total_logam");
  client.subscribe("/total_plastik");
  client.subscribe("/total_campur");
  client.subscribe("/kunci_logam");
  client.subscribe("/kunci_plastik");
  client.subscribe("/kunci_campur");
  client.subscribe("/login");
}

void messageReceived(String &topic, String &payload) {
  if (topic == "/total_logam") {
    Serial.print("Total Sampah Logam ");
    Serial.println(payload);
    total_logam = payload.toInt();
  }
  if (topic == "/total_plastik") {
    Serial.print("Total Sampah Plastik ");
    Serial.println(payload);
    total_plastik = payload.toInt();
  }
  if (topic == "/total_campur") {
    Serial.print("Total Sampah Campur ");
    Serial.println(payload);
    total_campur = payload.toInt();
  }
  
  if (topic == "/kunci_logam") {
    Serial.print("Kunci Bin Logam ");
    Serial.println(payload);
    kunci_logam = payload.toInt();
    digitalWrite(pinKunciLogam, kunci_logam);
  }
  if (topic == "/kunci_plastik") {
    Serial.print("Kunci Bin Plastik ");
    Serial.println(payload);
    kunci_plastik = payload.toInt();
    digitalWrite(pinKunciPlastik, kunci_plastik);
  }
  if (topic == "/kunci_campur") {
    Serial.print("Kunci Bin Campur ");
    Serial.println(payload);
    kunci_campur = payload.toInt();
    digitalWrite(pinKunciCampur, kunci_campur);
  }

  if (topic == "/login") {
    Serial.print("Login ");
    Serial.println(payload);
    login = payload.toInt();
  }  
}

void sampah_logam(){
    total_logam++;
    sesi_logam++;
    client.publish("/total_logam", String(total_logam));
    client.publish("/sesi_logam", String(sesi_logam));
    for (uint16_t microsec = USMAX; microsec > USMIN; microsec--) {
      pwm.writeMicroseconds(0, microsec);}
    delay(DELAY_OPEN);
    for (uint16_t microsec = USMIN; microsec < USMAX; microsec++) {
      pwm.writeMicroseconds(0, microsec);}
    //delay(DELAY_OPEN); 
}

void sampah_plastik(){
    total_plastik++;
    sesi_plastik++;
    client.publish("/total_plastik", String(total_plastik));
    client.publish("/sesi_plastik", String(sesi_plastik));
    for (uint16_t microsec = USMAX; microsec > USMIN; microsec--) {
      pwm.writeMicroseconds(0, microsec);}
    delay(DELAY_OPEN);
    for (uint16_t microsec = USMIN; microsec < USMAX; microsec++) {
      pwm.writeMicroseconds(0, microsec);}
//    delay(DELAY_OPEN);  
}

void sampah_campur(){
    total_campur++;
    sesi_campur++;
    client.publish("/total_campur", String(total_campur));
    client.publish("/sesi_campur", String(sesi_campur));
    for (uint16_t microsec = USMAX; microsec > USMIN; microsec--) {
      pwm.writeMicroseconds(0, microsec);}
    delay(DELAY_OPEN);
    for (uint16_t microsec = USMIN; microsec < USMAX; microsec++) {
      pwm.writeMicroseconds(0, microsec);}
    //delay(DELAY_OPEN);  
}

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(pinKunciLogam, OUTPUT);
  pinMode(pinKunciPlastik, OUTPUT);
  pinMode(pinKunciCampur, OUTPUT);

  pinMode(px_inductive, INPUT);
  pinMode(px_capacitive, INPUT);
  pinMode(px_optic, INPUT);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);
  digitalWrite(pinLed, HIGH);

  connect();
}

void loop() {
  client.loop();

  int readLogam = !digitalRead(px_inductive);
  int readPlastik = !digitalRead(px_capacitive);
  int readAda = !digitalRead(px_optic);
  //Serial.print(readLogam);
 // Serial.print(readPlastik);
  //Serial.println(readAda);

  if ((millis() - lastDebounceTimeLogam) > debounceDelay) {
    if (readLogam != lastLogamState) {
      lastDebounceTimeLogam = millis();
      logamState = readLogam;
      if (readLogam == HIGH) {
        Serial.println("Sensor logam aktif");
        sampah_logam();
        }
    }
  }
  lastLogamState = readLogam;
  
  if ((millis() - lastDebounceTimePlastik) > debounceDelay) {
    if (readPlastik != lastPlastikState) {
      lastDebounceTimePlastik = millis();
      plastikState = readPlastik;
      if (readPlastik == HIGH && !logamState) {
        Serial.println("Sensor plastik aktif");
        sampah_campur();
        }
    }
  }
  lastPlastikState = readPlastik;

  if ((millis() - lastDebounceTimeAda) > debounceDelay) {
    if (readAda != lastAdaState) {
      lastDebounceTimeAda = millis();
      adaState = readAda;
      if (readAda == HIGH && !logamState) {
        Serial.println("Sensor optik aktif");
        sampah_plastik();
        }
    }
  }  
  lastAdaState = readAda;
  
  delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  digitalWrite(pinLed, LOW);
  // publish a message roughly every second.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    client.publish("/status", "iotrash hardware connected");

    if (login){
        sesi_poin = (sesi_logam * POIN_LOGAM) + (sesi_plastik * POIN_PLASTIK) + (sesi_campur * POIN_CAMPUR);
        Serial.print("Selamat anda memiliki poin ");
        Serial.println(sesi_poin);
        client.publish("/poin", String(sesi_poin));     
    }
    else if (!login){
      sesi_logam = 0;
      sesi_plastik = 0;
      sesi_campur = 0;
      client.publish("/sesi_logam", String(sesi_logam));
      client.publish("/sesi_plastik", String(sesi_plastik));
      client.publish("/sesi_campur", String(sesi_campur));

      sesi_poin = 0;
      client.publish("/poin", String(sesi_poin));
    }
    
    digitalWrite(pinLed, HIGH);
    delay(100);
    }
  }
