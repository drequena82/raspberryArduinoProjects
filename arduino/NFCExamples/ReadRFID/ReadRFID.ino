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
  Serial.print("SCL:");
  Serial.println(SCL);
  Serial.print("SDA: ");
  Serial.println(SDA);
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  Serial.println("Waiting for an ISO14443A Card ...");
  Serial.println("Esperando tarjeta");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
    
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);  
  if (success) {
      Serial.println("Intentando autentificar bloque 4 con clave KEYA");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);   
      if (success){
        Serial.println("Sector 1 (Bloques 4 a 7) autentificados");
        uint8_t data[16];
 
        memcpy(data, (const uint8_t[]){ 'l', 'u', 'i', 's', 'l', 'l', 'a', 'm', 'a', 's', '.', 'e', 's', 0, 0, 0 }, sizeof data);
        success = nfc.mifareclassic_WriteDataBlock (4, data);
    
        if (success){          
          Serial.println("Datos escritos en bloque 4");
          delay(10000);           
        }
        else{
          Serial.println("Fallo al escribir tarjeta");
          delay(1000);   
        }
      }
      else{
        Serial.println("Fallo autentificar tarjeta");
        delay(1000); 
      }
    }
}