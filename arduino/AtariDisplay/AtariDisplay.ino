/*********************************************************************
AtariDisplay for Adafuit's Monochrome OLEDs based on SSD1306 drivers
This example is for a 128x64 size display using I2C to communicate
*********************************************************************/

// ///////////////////////////////////////////////////////////////////
// Display setup
// ///////////////////////////////////////////////////////////////////

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// ///////////////////////////////////////////////////////////////////
// Temperature sensor setup
// ///////////////////////////////////////////////////////////////////

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


// ///////////////////////////////////////////////////////////////////
// The images to be displayed
// ///////////////////////////////////////////////////////////////////

const unsigned char atariLogo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc4, 0x11, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc4, 0x11, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc4, 0x11, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc4, 0x11, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc4, 0x11, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc4, 0x11, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x84, 0x11, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x84, 0x10, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x84, 0x10, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x84, 0x10, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x84, 0x10, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x84, 0x10, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x84, 0x10, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x84, 0x10, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x8c, 0x10, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x0c, 0x18, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x0c, 0x18, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x0c, 0x18, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x0c, 0x18, 0x7f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x0c, 0x18, 0x3f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x0c, 0x18, 0x3f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfc, 0x1c, 0x18, 0x3f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfc, 0x1c, 0x1c, 0x1f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x1c, 0x1c, 0x0f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf8, 0x1c, 0x1c, 0x0f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x3c, 0x1e, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x3c, 0x1e, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xc0, 0x7c, 0x1f, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x7c, 0x1f, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0xfc, 0x1f, 0x80, 0x7f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x01, 0xfc, 0x1f, 0xc0, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x03, 0xfc, 0x1f, 0xe0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x03, 0xfc, 0x1f, 0xe0, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x07, 0xfc, 0x1f, 0xf0, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x1f, 0xfc, 0x1f, 0xf8, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x3f, 0xfc, 0x1f, 0xfe, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x7f, 0xfc, 0x1f, 0xff, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0xff, 0xfc, 0x1f, 0xff, 0xc0, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x07, 0xff, 0xfc, 0x1f, 0xff, 0xf0, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x7f, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xf3, 0xe0, 0x07, 0xcf, 0xf0, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xe1, 0xc0, 0x07, 0x87, 0xe0, 0x1e, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xe1, 0xc0, 0x07, 0x87, 0xe0, 0x0f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xc1, 0xfc, 0x7f, 0x07, 0xe3, 0xcf, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0xfc, 0x7f, 0x03, 0xe7, 0xc7, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xc8, 0xfc, 0x7f, 0x23, 0xe7, 0xc7, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x8c, 0xfc, 0x7e, 0x23, 0xe7, 0xc7, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x8c, 0x7c, 0x7e, 0x33, 0xe7, 0x8f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x8c, 0x7c, 0x7e, 0x71, 0xe7, 0x0f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x9c, 0x7c, 0x7c, 0x71, 0xe6, 0x1f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x1c, 0x3c, 0x7c, 0x71, 0xe6, 0x3f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x3c, 0x7c, 0x00, 0xe6, 0x3f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x3c, 0x7c, 0x00, 0xe6, 0x1f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x3f, 0x1c, 0x78, 0xf8, 0xe7, 0x1f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x3f, 0x1c, 0x78, 0xfc, 0x67, 0x8f, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x3f, 0x1c, 0x79, 0xfc, 0x67, 0x87, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x04, 0x7f, 0x1c, 0x71, 0xfc, 0x63, 0xc6, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
};

void setup()   {                
  // setup for auto power-off
  pinMode(7,OUTPUT);    // connect MOSFET to Pin 7
  digitalWrite(7,HIGH); // set to HIGH to keep arduino on after user releases pushbutton

  // setup for temperature sensor(s)
  sensors.begin();
  
  // setup for display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // +++ check that 3C is the address of your display, or change this value!

  // repeat things twice
  for(int i=0; i < 2; i++) {
    showImage(atariLogo);
    delay(4000); 
    display.clearDisplay();
    showTemperatures();
  }

  // power-off
  digitalWrite(7,LOW);  
}

void loop() {}

void showImage(const unsigned char* image) {
    display.clearDisplay(); // Make sure the display is cleared
    display.drawBitmap(0, 0, image, 128, 64, WHITE);
    display.display();
}

void showTemperatures() {
  /* use this for testing
  display.setTextSize(2); // 1..8 for this display
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println(String(sensors.getDeviceCount())+" sensors.");
  display.display();
  delay(2000);
  */
      
  display.setTextColor(WHITE);

  for (int j= 0; j < 5; j++) {
    //sensors.requestTemperatures(); // Send the command to get temperatures
    for (int i= 0; i < sensors.getDeviceCount(); i++) {
      display.clearDisplay();

      String label= "Temp";
      /*
      if (sensors.getDeviceCount() > 1) {
        label= label + " " + i;
      }
      */
      display.setTextSize(3); // 1..8 for this display
      display.setCursor(0,0);
      display.println(label + ":");

      float temp= 25;//sensors.getTempCByIndex(i);
      if (temp >= -40.0) {
        String tempFormatted= String(temp);
        int j= tempFormatted.indexOf(".");
        if (j >= 0) {
          tempFormatted= tempFormatted.substring(0, j+2);
        }
        display.setCursor(0,3*8+8);
        display.println(tempFormatted);
        display.setCursor((tempFormatted.length()-1)*3*8,3*8+8-4);
        display.setTextSize(2);
        display.println("\x9");
        display.display();
        delay(2000);
      }
    }
  }
}
