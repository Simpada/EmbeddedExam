
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <ezButton.h>

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define SW_PIN   2  // Arduino pin connected to SW  pin

ezButton button(SW_PIN);

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int bValue = 0; // To store value of the button



int floorHeight = 10;

// Player Settings
int playerCenterX = 70;
int playerCenterY = 30;

int oldCenterX;
int oldCenterY;

// Actual size will always be 1 higher than this value
int playerSizeX = 20;
int playerSizeY = 20;

uint16_t initialplayerFallTimer = 0;
uint16_t playerFallTimer = 0;

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(20);
  Serial.print(F("Hello! ST77xx TFT Test: "));

  tft.init(135, 240);
  // Visible limit is 134 : 239

  Serial.println(F("Initialized"));
  tft.fillScreen(ST77XX_BLACK);

  // Draw Floor
  for (int i = 0; i < floorHeight; i++) {
    for (int j = 0; j < 240; j++) {
      tft.drawPixel(i, j, 0xFC00);
    }
  }
  for (int j = 0; j < 240; j++) {
      tft.drawPixel(floorHeight, j, ST77XX_BLUE);
  }
  

  for (int i = playerCenterX - (playerSizeX / 2); i <= playerCenterX + (playerSizeX / 2); i++) {
    for (int j = playerCenterY - (playerSizeY / 2); j <= playerCenterY + (playerSizeY / 2); j++) {
      tft.drawPixel(j, i, ST77XX_WHITE);
    }
  }
  tft.drawPixel(playerCenterY, playerCenterX, ST77XX_RED);

}

void loop() {
  button.loop();
  
  oldCenterX = playerCenterX;
  oldCenterY = playerCenterY;

  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  bValue = button.getState();

  if (xValue < 500) {
    playerCenterY++;
  } else if (xValue > 540) {
    playerCenterY--;
  }  

  if (playerCenterY + (playerSizeY / 2) > 134) {
    playerCenterY = 134 - (playerSizeY / 2);
  }
  if (playerCenterY - (playerSizeY / 2) < floorHeight + 1) {
    playerCenterY = floorHeight + 1 + (playerSizeY / 2);
  }

  if (yValue < 482) {
    playerCenterX++;
  } else if (yValue > 522) {
    playerCenterX--;
  }  
  if (playerCenterX + (playerSizeX / 2) > 239) {
    playerCenterX = 239 - (playerSizeX / 2);
  }
  if (playerCenterX - (playerSizeX / 2) < 0) {
    playerCenterX = 0 + (playerSizeX / 2);
  }

  if (playerCenterX != oldCenterX || playerCenterY != oldCenterY) {
    drawPlayer();
    //delay(2);
  }
}

void drawPlayer() {

  int xDir = 0;
  if (playerCenterX > oldCenterX) {
    xDir = -1;
  } else if (playerCenterX < oldCenterX) {
    xDir = 1;
  } 

  int yDir = 0;
  if (playerCenterY > oldCenterY) {
    yDir = -1; 
  } else if (playerCenterY < oldCenterY) {
    yDir = 1;
  }

  if (xDir != 0) {
    for (int i = oldCenterX + ( xDir * (playerSizeX / 2)) ; i <= oldCenterX + ( xDir * (playerSizeX / 2)); i++) {
      for (int j = oldCenterY - (playerSizeY / 2); j <= oldCenterY + (playerSizeY / 2); j++) {
        tft.drawPixel(j, i, ST77XX_BLACK);
      }
    }
  }
  
  if (yDir != 0) {
   for (int i = oldCenterX - (playerSizeX / 2); i <= oldCenterX + (playerSizeX / 2); i++) {
      for (int j = oldCenterY + ( yDir * (playerSizeY / 2)); j <= oldCenterY + ( yDir * (playerSizeY / 2)); j++) {
        tft.drawPixel(j, i, ST77XX_BLACK);
      }
    }
  }

  for (int i = playerCenterX - (playerSizeX / 2); i <= playerCenterX + (playerSizeX / 2); i++) {
    for (int j = playerCenterY - (playerSizeY / 2); j <= playerCenterY + (playerSizeY / 2); j++) {
      tft.drawPixel(j, i, ST77XX_WHITE);
    }
  }
  tft.drawPixel(playerCenterY, playerCenterX, ST77XX_RED);
}


















