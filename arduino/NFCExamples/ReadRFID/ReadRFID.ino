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
  while (!nfc.getFirmwareVersion()) {
    Serial.print("PN53x no encontrado");
    delay(5000);
  }
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  Serial.println("Esperando tarjeta");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  
  Serial.println("Leyendo tarjeta...");

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);  
  if (success) {
      Serial.println("Intentando autentificar bloque 4 con clave KEYA");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);   
      if (success){
        Serial.println("Sector 1 (Bloques 4 a 7) autentificados");
        uint8_t data[16];
          
        success = nfc.mifareclassic_ReadDataBlock(4, data);    
        if (success){          
          Serial.println("Datos leidos de sector 4:");
          nfc.PrintHexChar(data, 16);
          Serial.println("");               
        }else{
          Serial.println("Fallo al leer tarjeta");
        }
      }else{
        Serial.println("Fallo autentificar tarjeta");
      }
    }
    delay(5000);
}