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
/**
Sectores: 16 sectores en total.
Bloques por sector: 
  Cada sector tiene 4 bloques (0, 1, 2 y 3).
Capacidad de bloque: 
  Cada bloque tiene 16 bytes.
Total de bloques: 16 * 4 = 64 
  Capacidad total: 64 * 16 bytes = 1024 bytes (1K).

Es importante recordar que no todos los bloques están disponibles para guardar datos de usuario:
1. Data Blocks (Bloques de datos): Son los bloques 0, 1 y 2 de cada sector. 
  Se usan para almacenar tu información.
2. Sector Trailer (Bloque 3): Es el último bloque de cada sector. 
  Contiene las **Keys A y B** y los **Access Bits**. 
No debe usarse para datos generales.
3. Manufacturer Block (Bloque 0 del Sector 0): 
  Contiene el UID (identificador único) de la tarjeta y 
  datos del fabricante. En las tarjetas estándar, este bloque viene bloqueado de fábrica y es de **solo lectura**.

> **Dato curioso:** Aunque la tarjeta se llama "1K", el espacio real disponible para datos de usuario 
(restando los trailers y el bloque del fabricante) es de **752 bytes**.

*/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define PN532_IRQ   (2)
#define PN532_RESET (3)
#define MAX_TIME (5000)

bool writeMode = false;
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
uint8_t nfc_key[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero
  pinMode(D7,INPUT);
  pinMode(D8,OUTPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.print(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

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
  delay(2000);
  writeMode = digitalRead(D7) == HIGH;
  print("Waiting for an ISO14443A Card ...");
}

void loop() {
  if(writeMode){
    writeNFCCard();
  }else{
    readNFCCard();
  }
  Serial.flush();
  delay(2000);
}

/**
  Print text into oled display
*/
void print(String text) {
  Serial.println(text);
  display.clearDisplay();
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(text);
  display.display();      // Show initial text
  delay(500);
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
  JsonDocument doc = TEST_MODE ? getMock() : getCardCredentials(strData);
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
    digitalWrite(D8, HIGH);
    delay(5000);
  }else{
    print("Not allowed " + name);
  }
  print("Locked!");
  digitalWrite(D8, LOW);
}

void writeNFCCard(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  print("Waiting to write an ISO14443A Card ...");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);  
  if (success) {
      print("Trying autentificate block 4 with key KEYA");
      //Autentication with sector 1, block 4
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, nfc_key);   
      if (success){
        print("Sector 1 (Blocks 4 a 7) autenticated");
        uint8_t data[16];
        generateRandomValues(data,sizeof data);
        success = nfc.mifareclassic_WriteDataBlock (1, data);
    
        if (success){          
          print("Data wrote in block");   
          nfc.PrintHexChar(data, 16);
          if(!TEST_MODE){
            newCard(data,"USER xxxxx",1);
          }       
        }else{
          print("Failure to write data");   
        }
      }else{
        print("Failure to authenticate");
      }
    }
}

void generateRandomValues(uint8_t* values, unsigned long length) {
  randomSeed(D0);
  for (int i = 0; i < length; i++) {
    values[i] = (uint8_t)random(0, 256);
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
    Serial.print(uidLength);
    Serial.print(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
      print("Seems to be an MIFARE Classic tag (4 byte UID)");
      print("Trying autentificate block 4 with key KEYA");
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, nfc_key);   
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
