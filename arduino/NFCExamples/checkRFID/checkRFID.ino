#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>


#define PN532_IRQ   (2)

#define PN532_RESET (3)  // Not connected by default on the NFC Shield

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(115200);
  nfc.begin();
  while (!nfc.getFirmwareVersion()) {
    Serial.print("PN53x no encontrado");
    delay(5000);
  }

  // Mostrar datos del sensor
  Serial.print("Found chip PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); 
  Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // Configurar para leer etiquetas RFID
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  
  Serial.println("Esperando tarjeta ISO14443A");
}

// Funcion auxiliar para mostrar el buffer
void printArray(byte *buffer, byte bufferSize) {
   for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
   }
}
 

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  
  Serial.println("Leyendo tarjetas...");
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);  
  
  if (success) {
    Serial.println("Tarjeta encontrada");
    Serial.print("UID Longitud: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID: "); 
    printArray(uid, uidLength);
    Serial.println("");
  } else
  {    
    Serial.println("Tarjeta no encontrada");
  }
  delay(1000);
}
