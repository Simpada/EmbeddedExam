

#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <ezButton.h>

// Setup for TFT Screen
#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Setup for Joystick
#define JOYSTICK_X_PIN  A0 // Arduino pin connected to VRX pin
#define JOYSTICK_Y_PIN  A1 // Arduino pin connected to VRY pin
#define BUTTON_PIN   3  // Arduino pin connected to SW  pin

ezButton joystickButton (BUTTON_PIN);
int joystickButtonState = 0;

// Setup for Buttons
int directionalButtons = A2;
int directionalButtonValue;


// Setup for Accelerometer


int joystickXValue = 0; // To store value of the X axis
int joystickYValue = 0; // To store value of the Y axis


// Player Settings
int playerCenterX = 70;
int playerCenterY = 30;

int playerSizeX = 10;
int playerSizeY = 10;

int oldCenterX;
int oldCenterY;

uint16_t color = ST77XX_RED;


void setup() {
  
  Serial.begin(9600);
  joystickButton.setDebounceTime(20);

  Serial.print(F("Hello! ST77xx TFT Test"));

  tft.init(135, 240);
  // Visible limit is 134 : 239

  Serial.println(F("Initialized"));

  tft.fillScreen(ST77XX_BLACK);

  for (int i = playerCenterX - (playerSizeX / 2); i <= playerCenterX + (playerSizeX / 2); i++) {
    for (int j = playerCenterY - (playerSizeY / 2); j <= playerCenterY + (playerSizeY / 2); j++) {
      tft.drawPixel(j, i, color);
    }
  }
}

void loop() {

  joystickButton.loop();

  directionalButtonValue = analogRead(directionalButtons);

  if (directionalButtonValue < 1000) {

    if (directionalButtonValue >= 750 && directionalButtonValue < 1000) {
      Serial.println("Button1");
      color = ST77XX_WHITE;
    } else if (directionalButtonValue >= 650 && directionalButtonValue < 750) {
      Serial.println("Button2");
      color = ST77XX_GREEN;
    } else if (directionalButtonValue >= 480 && directionalButtonValue < 650) {
      Serial.println("Button3");
      color = ST77XX_BLUE;
    } else if (directionalButtonValue < 480) {
      Serial.println("Button4");
      color = ST77XX_YELLOW;
    } 
  }

  for (int i = playerCenterX - (playerSizeX / 2); i <= playerCenterX + (playerSizeX / 2); i++) {
    for (int j = playerCenterY - (playerSizeY / 2); j <= playerCenterY + (playerSizeY / 2); j++) {
      tft.drawPixel(j, i, color);
    }
  }
}
































