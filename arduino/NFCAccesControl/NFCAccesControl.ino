#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "config.h"
#include "API.hpp"
#include "ESP32_Utils.hpp"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define PN532_IRQ   (2)
#define PN532_RESET (3)
#define MODE_PIN (7)
#define MAX_TIME (5000)
#define DEVICE_LEVEL (1)

bool writeMode = false;
unsigned long buttonTime = 0;
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    print(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  print("Starting access control!");
  print("Starting access control!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); 
  Serial.println((versiondata>>8) & 0xFF, DEC);

  // Inizialice wifi
  connectWiFi_STA();
  print("Waiting for an ISO14443A Card ...");
}

void loop() {
  manageMode();
  if(writeMode){
    writeNFCCard();
  }else{
    readNFCCard();
  }

  // Wait a bit before trying again
  print("\n\nSend a character to scan another tag!");
  Serial.flush();
  delay(2000);
}

/**
  Print text into oled display
*/
void print(String text) {
  Serial.println(text);
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(text);
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  /*
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  */
}

/**
  Function to manage read/write mode
*/
void manageMode(){
  int buttonDown = digitalRead(MODE_PIN);
  if(buttonDown == HIGH){
    if(buttonTime == 0){
      buttonTime = millis();
    }
    if(buttonTime > 0 && (millis() - buttonTime) > MAX_TIME){
      writeMode = !writeMode;
      print("Mode changed!");
    }
  }else{
    buttonTime = 0;
  }
}

char* getCardData(uint8_t* data){
  size_t len = sizeof(data);
  char safeString[len + 1]; // +1 para el terminador nulo
  memcpy(safeString, data, len);
  safeString[len] = '\0'; // Forzamos el cierre de la cadena
  Serial.println(safeString);

  return safeString;
}

void checkKey(uint8_t* data){
  char* strData = getCardData(data);
  print("Sending card data...");
  JsonDocument doc = getCardCredentials(strData);
  if(!doc["level"]){
    print("Send error!");
  }else{
    unlock(doc);
  }
}

void unlock(JsonDocument doc){
  uint8 level = doc["level"];
  String name = String(doc["username"]);
  if(level <= DEVICE_LEVEL){
    print("Welcome " + name);
    digitalWrite(D7, HIGH);
    delay(5000);
  }else{
    print("Not allowed " + name);
  }
  digitalWrite(D7, LOW);
}

void writeNFCCard(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  print("Waiting to write an ISO14443A Card ...");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);  
  if (success) {
      print("Trying autentificate block 4 with key KEYA");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);   
      if (success){
        print("Sector 1 (Blocks 4 a 7) autenticated");
        uint8_t data[16];
 
        memcpy(data, (const uint8_t[]){ 'l', 'u', 'i', 's', 'l', 'l', 'a', 'm', 'a', 's', '.', 'e', 's', 0, 0, 0 }, sizeof data);
        success = nfc.mifareclassic_WriteDataBlock (4, data);
    
        if (success){          
          print("Data wrote in block 4");         
        }else{
          print("Failure to write data");   
        }
      }else{
        print("Failure to authenticate");
      }
    }
}

void readNFCCard(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  print("Waiting to read an ISO14443A Card ...");
  // Wait for an NTAG203 card.  When one is found 'uid' will be populated with
  // the UID, and uidLength will indicate the size of the UUID (normally 7)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    print("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);
    print(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    print("");
    if (uidLength == 4){
      print("Seems to be an MIFARE Classic tag (4 byte UID)");
      print("Trying autentificate block 4 with key KEYA");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);   
      if (success){
        print("Sector 1 (Blocks 4 to 7) autenticated: ");
          uint8_t data[16];
          success = nfc.mifareclassic_ReadDataBlock(4, data);    
          if (success){          
            print("Data read from block :");
            nfc.PrintHexChar(data, 16);
            checkKey(data);
            delay(100);           
          }else{
            print("Failure to read tag");
          }
      }else{
          print("Not success to autenticate");
      }
    }
  }
}
