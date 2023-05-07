
#include <SPI.h>
#include <ezButton.h>
#include "tank.h"

#define SKYBLUE 0x165e

// Variables for Audio
const int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
const int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

// Variables for GamePlay
bool gameStarted = false;

bool playTextVisible = true;
uint16_t playBlinkStart;
uint16_t playBlinkCurrent;

// Variables for World / Map
const uint16_t worldColor = SKYBLUE;

const int floorHeight = 18;
const int ceilingHeight = 112;

void setup() {
  Serial.begin(9600);

  // Serial.print("Initializing SD card...");

  // if (!SD.begin(4)) {
  //   Serial.println("initialization failed!");
  //   while (1)
  //     ;
  // }
  // Serial.println("initialization done.");

  Serial.print("TFT init : ");
  tft.init(135, 240);
  // Visible limit is 0-134 : 0-239
  tft.fillScreen(ST77XX_BLACK);
  
  Serial.println("SUCCESS");

  initActors();

  drawStartScreen();
}

void loop() {
  if (gameStarted) {
    if (!gameOver) {
      updateScreen();
      updatePlayerValues();
      enemyActions();
      tankActions(singlePlayer);
      playerActions();
    } else {
      gameOverMenu();
      checkForRestart();
    }
  } else {
    startButtonBlink();
    checkForGameStart();
  }
}

void initActors() {
  Serial.println("Starting setup");

  setUpPlayer(ceilingHeight, floorHeight);
  Serial.println("Player setup complete");

  setUpTank(ceilingHeight, floorHeight);
  Serial.println("Tank setup complete");

  setUpEnemies(worldColor);
  Serial.println("Enemy setup complete");

  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(speakerPin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speakerPin);
  }
}

void drawStartScreen() {

  tft.setRotation(0);
  int pixelSize = 10;

  // Draws Top and Bottom
  for (int width = 0; width < 240; width++) {
    for (int height = 134; height > 134 - pixelSize; height--) {
      tft.drawPixel(height, width, ST77XX_MAGENTA);
    }
    for (int height = 0; height < pixelSize; height++) {
      tft.drawPixel(height, width, ST77XX_MAGENTA);
    }
  }

  // Draws left and right
  for (int height = 0; height < 135; height++) {
    for (int width = 0; width < pixelSize; width++) {
      tft.drawPixel(height, width, ST77XX_MAGENTA);
    }
    for (int width = 239; width > 239 - pixelSize; width--) {
      tft.drawPixel(height, width, ST77XX_MAGENTA);
    }
  }

  // Draws checkered pattern
  for (int row = 7; row > 1; row--) {
    bool skip = false;
    if (row % 2 == 0) {
      skip = true;
    }

    // Draws left side
    for (int width = pixelSize; width < (pixelSize * row); width++) {

      if (width % 10 == 0 && !skip) {
        width += 10;
      } else if (skip) {
        skip = false;
      }

      // Draws lower left
      for (int height = pixelSize * (8 - row); height < (pixelSize * (8 - row)) + pixelSize; height++) {
        tft.drawPixel(height, width, ST77XX_MAGENTA);
      }
      // Drawws upper left
      for (int height = 134 - (pixelSize * (8 - row)); height > 134 - ((pixelSize * (8 - row)) + pixelSize); height--) {
        tft.drawPixel(height, width, ST77XX_MAGENTA);
      }
    }

    if (row % 2 == 0) {
      skip = true;
    }

    // Draws right side
    for (int width = 239 - pixelSize; width > 239 - (pixelSize * row); width--) {

      if ((width + 1) % 10 == 0 && !skip) {
        width -= 10;
      } else if (skip) {
        skip = false;
      }

      // Draws lower right
      for (int height = pixelSize * (8 - row); height < (pixelSize * (8 - row)) + pixelSize; height++) {
        tft.drawPixel(height, width, ST77XX_MAGENTA);
      }
      // Draws upper right
      for (int height = 134 - (pixelSize * (8 - row)); height > 134 - ((pixelSize * (8 - row)) + pixelSize); height--) {
        tft.drawPixel(height, width, ST77XX_MAGENTA);
      }
    }
  }

  tft.setRotation(1);

  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
  tft.setCursor(74, 25);
  tft.print("WARGAMES");

  tft.setTextSize(1);
  // tft.setCursor(72, 100);
  // tft.print("HIGHSCORE:");
  // tft.setCursor(105, 110);
  // tft.print(getHighscore());

  playBlinkStart = millis();
}

void startButtonBlink() {

  playBlinkCurrent = millis();

  if (playBlinkCurrent - playBlinkStart >= 700) {
    if (playTextVisible) {
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.setTextColor(ST77XX_BLACK);
    }
    tft.setCursor(50, 58);
    tft.print("CLICK ANY BUTTON TO PLAY");
    tft.setCursor(36, 72);
    tft.print("CLICK BUMPER FOR MULTIPLAYER");
    playTextVisible = !playTextVisible;
    playBlinkStart = millis();
  }
}

void checkForGameStart() {

  directionalButtonValue = analogRead(directionalButtons);
  joystickButton.loop();
  tankButtonState = digitalRead(tankButton);

  if (directionalButtonValue < 1000) {
    singlePlayer = true;
    startGame();
  }
  if (joystickButton.isPressed() || tankButtonState == HIGH) {
    singlePlayer = false;
    startGame();
  }
}

void startGame() {
  tft.setRotation(0);

  tft.fillScreen(worldColor);

  drawGameScene();
  startTimers();

  gameStarted = true;

  int seedValue = millis();
  randomSeed(seedValue);
}

void checkForRestart() {

  directionalButtonValue = analogRead(directionalButtons);
  joystickButton.loop();
  tankButtonState = digitalRead(tankButton);

  if (directionalButtonValue < 1000 || joystickButton.isPressed() || tankButtonState == HIGH) {
    setUpPlayer(ceilingHeight, floorHeight);
    setUpEnemies(worldColor);

    tft.fillScreen(ST77XX_BLACK);
    drawStartScreen();

    gameOver = false;
    gameStarted = false;

    delay(500);
  }
}

void drawGameScene() {
  drawWorld();
  drawHearts(playerHealth);
  drawScore();
  drawPlayer(true);
  drawTank();
}

void drawWorld() {
  // Draws floor and ceiling
  for (int j = 0; j < 240; j++) {
    for (int i = 0; i < floorHeight; i++) {
      tft.drawPixel(i, j, 0xFC00);
    }

    for (int i = 134; i > ceilingHeight; i--) {
      tft.drawPixel(i, j, ST77XX_BLACK);
    }

    tft.drawPixel(134, j, 0xFC00);
    tft.drawPixel(ceilingHeight, j, 0xFC00);
    tft.drawPixel(ceilingHeight + 1, j, 0xFC00);
  }

  for (int i = 134; i > ceilingHeight; i--) {

    tft.drawPixel(i, 0, 0xFC00);

    tft.drawPixel(i, 76, 0xFC00);
    tft.drawPixel(i, 77, 0xFC00);

    tft.drawPixel(i, 239, 0xFC00);
  }
}

void startTimers() {
  enemyMoveDelayTotal = millis();
  lastSpawnTime = millis();
  tankMoveDelayTimerStart = millis();
  tankShootCooldownTotal = millis();
  playerMoveDelayTotal = millis();
  playerShootCooldownTotal = millis();
}

void updateScreen() {

  if (tankTopLeftWidth != oldTankTopLeftWidth) {
    updateTankPosition();
    tankMoveDelayTimerStart = millis();
  }
  if (playerCenterWidth != oldCenterX || playerCenterHeight != oldCenterY) {
    updatePlayerPosition();
  }
}
