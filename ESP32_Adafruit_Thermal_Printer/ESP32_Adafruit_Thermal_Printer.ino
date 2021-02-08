#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


#include "WiFi.h"
#include "Adafruit_Thermal.h"
#include <HardwareSerial.h>

HardwareSerial MySerial(1);

const char* ssid = "Redmi5";
const char* password =  "mayarheula";

#define TX_PIN 26 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 25 // Arduino receive   GREEN WIRE   labeled TX on printer

//HardwareSerial Serial1(1);
//HardwareSerial Serial3(3);


//SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first

//Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor
Adafruit_Thermal printer(&MySerial);      // Or Serial2, Serial3, etc


AsyncWebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial1.begin(9600, RX_PIN, TX_PIN);
  MySerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);



  // setup wifi:
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  //setup stampante

  Serial.println("Setup Completed");


  // setup web server:
  server.on("/menu1", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", "<p>This is Menu1!</p>");
    Serial.println("inizia stampa");

    printer.begin();        // Init printer (same regardless of serial type)
    printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
    printer.println("Menu per Roberto");
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Antipasti");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Mini tartare di filetto \ndi Angus argentino\nCicoria e fave\nFiori di zucca in tempura");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Primi");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Risotto alla milanese \n(senza lattosio)\nTortelli all'entrana su crema \ndi formaggio (senza lattosio)\nPaccheri al ragu");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Secondi");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Filetto di Angus ai ferri \ncon patate al forno\nFiletto di salmone servito \nsu asparagi gratinati\nRombo chiodato servito \ncon fagiolini e salsa guacamole\nAnatra all'arancia");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Dessert");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Gelato al latte di mandorla\nSorbetto al limone (senza latte)");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
   
    printer.feed(2);

    printer.sleep();      // Tell printer to sleep
    delay(3000L);         // Sleep for 3 seconds
    printer.wake();       // MUST wake() before printing again, even if reset
   
    /// stampa menu 1
    Serial.println("fine stampa");
  });

  server.on("/menu2", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", "<p>This is Menu2!</p>");
    Serial.println("inizia stampa");

    printer.begin();        // Init printer (same regardless of serial type
    printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
    printer.println(F("Menu per Marina"));
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Antipasti");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Mini tartare di filetto \ndi Angus argentino\nCicoria e fave\nBurratina fresca");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Primi");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Risotto alla milanese\nSpaghetti cacio e pepe\n(senza glutine)");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Secondi");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Filetto di Angus ai ferri \ncon patate al forno\nFiletto di salmone servito \nsu asparagi lessi\nRombo chiodato servito \ncon fagiolini e salsa guacamole\nAnatra all'arancia");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Dessert");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Gelato\nSorbetto al limone\nCreme brulee");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
   
    printer.feed(2);

    printer.sleep();      // Tell printer to sleep
    delay(3000L);         // Sleep for 3 seconds
    printer.wake();       // MUST wake() before printing again, even if reset
   
    /// stampa menu 2
    Serial.println("fine stampa");
  });

  server.on("/menu3", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", "<p>This is Menu3!</p>");
    Serial.println("inizia stampa");

    printer.begin();        // Init printer (same regardless of serial type
    printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
    printer.println(F("Menu per Lidia"));
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Antipasti");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Mini tartare di filetto \ndi Angus argentino\nCicoria e fave\nFiori di zucca in tempura\nBurratina fresca");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Primi");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Risotto alla milanese\nTortelli all'entrana su crema \ndi formaggio\nPaccheri al ragu\nSpaghetti cacio e pepe");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Secondi");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Filetto di Angus ai ferri \ncon patate al forno\nAnatra all'arancia");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
    printer.feed(1);

    printer.setSize('M');       
    printer.println("Dessert");
    printer.feed(1);

    printer.setSize('S');
    printer.println("Gelato\nSorbetto al limone\nCreme brulee");
    printer.setLineHeight(); // Reset to default
    printer.justify('L');
   
    printer.feed(2);

    printer.sleep();      // Tell printer to sleep
    delay(3000L);         // Sleep for 3 seconds
    printer.wake();       // MUST wake() before printing again, even if reset
   
    /// stampa menu 3
    Serial.println("fine stampa");

  });

  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

}
