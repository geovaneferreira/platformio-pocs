#include <Arduino.h>
#include <RadioLib.h>
#include <SPI.h>
#include "SSD1306.h" 

void setFlag(void);

//-------------
// If you use as sender change transmit_pck to 1
// Modem Lora SX1262 -> Using in ebyte 900m22s
// Modem Lora SX1276 -> Using in TGO Lora PCI
//------------
#define transmit_pck 0
#define TYPE_MODEM_LORA 1262

#if TYPE_MODEM_LORA == 1262
  #define RADIO_RXEN 27
  #define RADIO_TXEN 26

  #define hasOled 0
  #define SCK     18//5    // GPIO5  -- SX1278's SCK
  #define MISO    19//19   // GPIO19 -- SX1278's MISO
  #define MOSI    23//27   // GPIO27 -- SX1278's MOSI
  #define NSS     5//18   // GPIO18 -- SX1278's CS
  #define RST     35//14   // GPIO14 -- SX1278's RESET
  #define DI1     22//22//26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
  #define BUSY    34
  String MODEM_LORA  = "SX1262";
  SX1262 radio = new Module(NSS, DI1, RST, BUSY, SPI);
  #if hasOled == 1
    SSD1306 display(0x3c, 33, 32);
  #endif
#elif TYPE_MODEM_LORA == 1276
  #define hasOled 1
  #define SCK     5    // GPIO5  -- SX1278's SCK
  #define MISO    19   // GPIO19 -- SX1278's MISO
  #define MOSI    27   // GPIO27 -- SX1278's MOSI
  #define NSS     18   // GPIO18 -- SX1278's CS
  #define RST     14   // GPIO14 -- SX1278's RESET
  #define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
  #if hasOled == 1
    SSD1306 display(0x3c, 4, 15);
  #endif
  String MODEM_LORA  = "SX1276";
  SX1276 radio = new Module(NSS, DI0, RST, 3, SPI);
#endif

void setup() {
  #if hasOled == 1
    pinMode(16,OUTPUT);
    pinMode(2,OUTPUT);
    digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
    delay(50); 
    digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
    display.init();
    display.flipScreenVertically();  
    display.setFont(ArialMT_Plain_10);
  #endif
  Serial.begin(9600);
  SPI.begin(SCK, MISO, MOSI, NSS);
  Serial.println("[" + MODEM_LORA + "] Initializing ... ");

  #if TYPE_MODEM_LORA == 1262
    int state  = radio.begin(433.0, 125.0, 7, 5, SX126X_SYNC_WORD_PRIVATE, 10, 8, 0);  
  #elif TYPE_MODEM_LORA == 1276
    int state  = radio.begin();   
  #endif  
  radio.reset();
  
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  #if transmit_pck == 0
  #if TYPE_MODEM_LORA == 1262
    radio.setRfSwitchPins(RADIO_RXEN, RADIO_TXEN);
    radio.setDio1Action(setFlag);
  #elif TYPE_MODEM_LORA == 1276
    radio.setDio0Action(setFlag);
  #endif  
  // start listening for LoRa packets
  Serial.print("[" + MODEM_LORA + "] Starting to listen ... ");
  state = radio.startReceive();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  #endif
  

  Serial.println(F("All settings succesfully changed!"));
}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;
// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }
  // we got a packet, set the flag
  receivedFlag = true;
}

int counter = 0;
void loop() {
    #if transmit_pck == 0
    if(receivedFlag) {
      // disable the interrupt service routine while
      // processing the data
      enableInterrupt = false;

      // reset flag
      receivedFlag = false;
      String str;
      int state = radio.readData(str);
      //you can also receive data as byte array
      
      //byte byteArr[12];
      //int state = radio.receive(byteArr, 12);
      if (state == ERR_NONE) {
        // packet was successfully received
        Serial.println(F("success!"));

        // print the data of the packet
        Serial.print(F("[LLCC68] Data:\t\t\t"));
        Serial.println(str);
        ////for(int i=0; i<sizeof(byteArr); i++)
        //  Serial.print(byteArr[i]);

        // print the RSSI (Received Signal Strength Indicator)
        // of the last received packet
        Serial.print(F("[LLCC68] RSSI:\t\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        // print the SNR (Signal-to-Noise Ratio)
        // of the last received packet
        Serial.print(F("[LLCC68] SNR:\t\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

        #if hasOled == 1
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_10);
        
        display.drawString(0, 15, "Receiver: ");
        display.drawStringMaxWidth(0 , 26 , 128, str);
        display.drawString(0, 0, "RSSI: " + String(radio.getRSSI()));  
        display.display();
        #endif
      } else if (state == ERR_RX_TIMEOUT) {
        // timeout occurred while waiting for a packet
        Serial.println(F("timeout!"));

      } else if (state == ERR_CRC_MISMATCH) {
        // packet was received, but is malformed
        Serial.println(F("CRC error!"));

      } else {
        // some other error occurred
        Serial.print(F("failed, code "));
        Serial.println(state);
      }
       // put module back to listen mode
      radio.startReceive();

      // we're ready to receive more packets,
      // enable interrupt service routine
      enableInterrupt = true;
    }

    #else
    Serial.print("[" + MODEM_LORA + "] Transmitting packet ... ");
    #if hasOled == 1
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    
    display.drawString(0, 15, "Send: " + String(counter));
    display.display();
    #endif
    // you can transmit C-string or Arduino string up to
    // 256 characters long
    // NOTE: transmit() is a blocking method!
    //       See example SX126x_Transmit_Interrupt for details
    //       on non-blocking transmission method.
    int state = radio.transmit("Hi! " + String(counter));
    counter++;
    if(counter > 1000)
      counter = 0;
    // you can also transmit byte array up to 256 bytes long
    /*
      byte byteArr[] = {0x01, 0x23, 0x45, 0x56, 0x78, 0xAB, 0xCD, 0xEF};
      int state = radio.transmit(byteArr, 8);
    */

    if (state == ERR_NONE) {
      // the packet was successfully transmitted
      Serial.println(F("success!"));

      // print measured data rate
      Serial.print("[" + MODEM_LORA + "] Datarate:\t");
      Serial.print(radio.getDataRate());
      Serial.println(F(" bps"));

    } else if (state == ERR_PACKET_TOO_LONG) {
      // the supplied packet was longer than 256 bytes
      Serial.println(F("too long!"));

    } else if (state == ERR_TX_TIMEOUT) {
      // timeout occured while transmitting packet
      Serial.println(F("timeout!"));

    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);

    }

    // wait for a second before transmitting again
    delay(1000);
  #endif
}