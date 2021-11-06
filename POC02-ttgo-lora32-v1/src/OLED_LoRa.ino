////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
////                        RECIVER
////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/*#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include "images.h"

#define SCK     34//5    // GPIO5  -- SX1278's SCK
#define MISO    32//19   // GPIO19 -- SX1278's MISO
#define MOSI    35//27   // GPIO27 -- SX1278's MOSI
#define SS      33//18   // GPIO18 -- SX1278's CS
#define RST     23//14   // GPIO14 -- SX1278's RESET
#define DI0     22//26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    915E6//868E6

//SSD1306 display(0x3c, 4, 15);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

void logo(){
  // display.clear();
  // display.drawXbm(0,5,logo_width,logo_height,logo_bits);
  // display.display();
}

void loraData(){
  // display.clear();
  // display.setTextAlignment(TEXT_ALIGN_LEFT);
  // display.setFont(ArialMT_Plain_10);
  // display.drawString(0 , 15 , "Received "+ packSize + " bytes");
  // display.drawStringMaxWidth(0 , 26 , 128, packet);
  // display.drawString(0, 0, rssi);  
  // display.display();
  Serial.println("Received "+ packSize + " bytes");
  Serial.println(packet);
}

void cbk(int packetSize) {
  packet ="";
  packSize = String(packetSize,DEC);
  for (int i = 0; i < packetSize; i++) { packet += (char) LoRa.read(); }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  loraData();
}

void setup() {
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high、
  
  Serial.begin(9600);
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Receiver Callback");
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("init ok");
  // display.init();
  // display.flipScreenVertically();  
  // display.setFont(ArialMT_Plain_10);
  // logo();
  // delay(1000);

  // display.clear();
  // display.setTextAlignment(TEXT_ALIGN_LEFT);
  // display.setFont(ArialMT_Plain_10);
  // display.drawString(0 , 23 , "Listening to packets...");
  // display.drawStringMaxWidth(0 , 26 , 128, packet);
  // display.display();
  //LoRa.onReceive(cbk);
  //LoRa.receive();
 
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) { cbk(packetSize);  }
  delay(10);
}
*/

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////
////                        SENDER
////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  
#include "SSD1306.h" 
#include "images.h"

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    434E6

unsigned int counter = 0;

SSD1306 display(0x3c, 4, 15);
String rssi = "RSSI --";
String packSize = "--";
String packet ;

void logo(){
  display.clear();
  display.drawXbm(0,5,logo_width,logo_height,logo_bits);
  display.display();
}

void setup() {
  pinMode(16,OUTPUT);
  pinMode(2,OUTPUT);
  
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  Serial.begin(9600);
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Sender Test");
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 Serial.println("SpreadingFactor");
 Serial.println(LoRa.getSpreadingFactor());

 Serial.println("SignalBandwidth");
 Serial.println(LoRa.getSignalBandwidth());

  //LoRa.onReceive(cbk);
//  LoRa.receive();
  Serial.println("init ok");
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_10);
  logo();
  delay(1500);
}

void loop() {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  
  display.drawString(0, 0, "Sending packet: ");
  display.drawString(90, 0, String(counter));
  display.display();

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello");
  LoRa.print(counter);
  //LoRa.print("PING");
  LoRa.endPacket();

  counter++;
  if(counter >= 10)
    counter = 0;
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
