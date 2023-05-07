
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <ezButton.h>

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define SW_PIN   3  // Arduino pin connected to SW  pin

ezButton button(SW_PIN);

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int bValue = 0; // To store value of the button

int pixelW = 70;
int pixelH = 120;
int color = 0;

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(20);
  Serial.print(F("Hello! ST77xx TFT Test"));

  tft.init(135, 240);
  // Visible limit is 134 : 239

  Serial.println(F("Initialized"));
  tft.fillScreen(ST77XX_BLACK);

  tft.drawPixel(pixelW, pixelH, ST77XX_RED);

}

// 1st number Y axis (up/down)
// 2nd number x axis (left/right)

void loop() {
  
  button.loop();

  int oldPixelW = pixelW;
  int oldPixelH = pixelH;

  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  bValue = button.getState();

  if (button.isPressed()) {
    tft.drawPixel(oldPixelW, oldPixelH, ST77XX_BLACK);
    pixelW = 70;
    pixelH = 120;
  }

  if (xValue < 515) {
    pixelW++;
  } else if (xValue > 525) {
    pixelW--;
  }  

  if (pixelW > 134) {
    pixelW = 134;
  }
  if (pixelW < 0) {
    pixelW = 0;
  }

  if (yValue < 497) {
    pixelH++;
  } else if (yValue > 507) {
    pixelH--;
  }  
  if (pixelH > 239) {
    pixelH = 239;
  }
  if (pixelH < 0) {
    pixelH = 0;
  }

  if (pixelH != oldPixelH || pixelW != oldPixelW) {
    tft.drawPixel(oldPixelW, oldPixelH, ST77XX_BLACK);
    tft.drawPixel(pixelW, pixelH, ST77XX_RED);

  }

  delay(10);

}
















