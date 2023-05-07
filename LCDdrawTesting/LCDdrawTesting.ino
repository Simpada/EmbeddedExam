
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8


Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


  int Width = 60;
  int Height = -20;

void setup() {

  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));

  tft.init(135, 240);
  // Width = 135
  // Height = 240

  // Visible limit is 134 : 239

  Serial.println(F("Initialized"));


  tft.fillScreen(ST77XX_BLACK);


  tft.drawPixel(0, 0, ST77XX_BLUE); 
  tft.drawPixel(0, 239, ST77XX_RED);

  tft.drawPixel(134, 239, ST77XX_YELLOW);
  tft.drawPixel(134, 0, ST77XX_GREEN);
}

void loop() {

  delay(10);

  if (Height < 240) {

    for(int i = 0; i < 20; i++) {
      tft.drawPixel(Width + i, Height, ST77XX_BLACK);
    }

    Height++;

    for(int i = 0; i < 20; i++) {
      tft.drawPixel(Width + i, Height + 19, ST77XX_BLUE);
    }
  } else {
    Height = -20;
  }
  
}
