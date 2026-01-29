#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>


#define PN532_IRQ   (2)

#define PN532_RESET (3)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(115200);

  // Configurar para leer etiquetas RFID
  nfc.begin();
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  
  Serial.println("Waiting tag...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
    
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);  
  if (success) {
      Serial.println("Trying autentificate block 4 with key KEYA");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);   
      if (success){
        Serial.println("Sector 1 (Blocks 4 a 7) autenticated");
        uint8_t data[16];
 
        memcpy(data, (const uint8_t[]){ 'l', 'u', 'i', 's', 'l', 'l', 'a', 'm', 'a', 's', '.', 'e', 's', 0, 0, 0 }, sizeof data);
        success = nfc.mifareclassic_WriteDataBlock (4, data);
    
        if (success){          
          Serial.println("Data wrote in block 4");
          delay(1000);           
        }else{
          Serial.println("Failure to write data");
          delay(1000);   
        }
      }
      else{
        Serial.println("Failure to authenticate");
        delay(1000); 
      }
    }
}
