// Accepts Graphics Data over a UART Connection

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include <SPI.h>

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS         9
#define TFT_RST        -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         10

#define HEIGHT 128
#define WIDTH 160

#define MAXSIZE 

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

unsigned char opcode = NULL;
unsigned char input = NULL;

int x = 0, y = 0, pos = 0;
unsigned char lo = 0x00, hi = 0x00;

bool checkForEnd = true;
bool ignore = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); while (!Serial); Serial.println();
  tft.initR(INITR_GREENTAB);
  tft.setFont();
  tft.fillScreen(0x0000);

  // Initialize Debug LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}

// Resets all Global Variables to be the default values
void resetAllValues() {
  x = 0;
  y = 0;
  pos = 0;
  lo = 0x00;
  hi = 0x00;
  ignore = false;
  opcode = 0;
  input = 0;
}

// Process a Fillscreen Opcode
void procFillScreen() {
  checkForEnd = false;
  if (pos % 2 == 0) {
    lo = input;
  } else {
    hi = input;
    uint16_t lohi = lo + (hi >> 2);
//    uint16_t lohi = 0x0000;
    tft.fillScreen(lohi);
    lo = 0x00;
    hi = 0x00;
    checkForEnd = true;
    ignore = true;
  }
  pos++;
}

// Process a Draw Screen Opcode
void procDrawScreen() {
  checkForEnd = false;
  if (pos % 2 == 0) {
    lo = input;
  } else {
    hi = input;
    uint16_t lohi = lo + (hi << 8);
//    uint16_t lohi = 0xffff;
    tft.drawPixel(x, y, lohi);
    lo = 0x00;
    hi = 0x00;
    x += 1;
    if (x >= HEIGHT) {
      y += 1;
      x = 0;
    }
    if (y >= WIDTH) {
      checkForEnd = true;
      ignore = true; 
      
    }
  }
  pos++;
}

void serialEvent() {
  while(Serial.available()) {
    input = Serial.read(); // Reads an int!

    // Set the Opcode
    if (opcode == NULL && input != 0) {
//      resetAllValues();
      opcode = input;
      continue;
    }
    // 10 is the newline character, symbolizing the end of a line
    if (checkForEnd && input == 10) {
      Serial.println("End");
      
//      x = 0;
//      y = 0;
//      pos = 0;
//      lo = 0x00;
//      hi = 0x00;
//      ignore = false;
//      opcode = NULL;
//      input = NULL;
      resetAllValues();
      return;
    }
    
    if (ignore) continue;

    // Fill Screen (B)
    if (opcode == 0x42) {
      procFillScreen();
    }

    // Draw Screen (A)
    if (opcode == 0x41) {
      procDrawScreen();
    }
    
  }
  
}
