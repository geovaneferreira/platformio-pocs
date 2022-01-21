#include <Arduino.h>
#include <lorawan.h>


//ABP Credentials 
const char *devAddr = "47020db5";
const char *nwkSKey = "7694a9fa668b59676a14fc03ba9b960c";
const char *appSKey = "66e458d609e77eea04627a4d94141d37";

const unsigned long interval = 10000;    // 10 s interval to send message
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

char myStr[50];
char outStr[255];
byte recvStatus = 0;

#define RADIO_RESET_PORT        14
#define RADIO_MOSI_PORT         27
#define RADIO_MISO_PORT         19
#define RADIO_SCLK_PORT         5
#define RADIO_NSS_PORT          18
#define RADIO_DIO_0_PORT        26
#define RADIO_DIO_1_PORT        33
#define RADIO_DIO_2_PORT        32

const sRFM_pins RFM_pins = {
  .CS = RADIO_NSS_PORT,
  .RST = RADIO_RESET_PORT,
  .DIO0 = RADIO_DIO_0_PORT,
  .DIO1 = RADIO_DIO_1_PORT,
  .DIO2 = RADIO_DIO_2_PORT,
  .DIO5 = 1,
};

void setup() {
  // Setup loraid access
  Serial.begin(9600);
  while(!Serial);
  if(!lora.init()){
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(SF8BW125);

  // set channel to random
  lora.setChannel(MULTI);
  
  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
}

void loop() {
  // Check interval overflow
  if(millis() - previousMillis > interval) {
    previousMillis = millis(); 

    sprintf(myStr, "Counter-%d", counter); 

    Serial.print("Sending: ");
    Serial.println(myStr);
    
    lora.sendUplink(myStr, strlen(myStr), 0, 1);
    counter++;
  }

  recvStatus = lora.readData(outStr);
  if(recvStatus) {
    Serial.println(outStr);
  }
  
  // Check Lora RX
  lora.update();
}