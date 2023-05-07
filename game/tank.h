
#include "player.h"
#include <basicMPU6050.h>

// accelerometer setup
basicMPU6050<> accelerometer;

// tank setup
int tankTopLeftWidth;
int tankTopLeftHeight;

int maxHeightTank;
int minHeightTank;

int oldTankTopLeftWidth;

const uint16_t tankMoveDelay = 2;
uint16_t tankMoveDelayTimerStart;
uint16_t tankMoveDelayTimerCurrent;

const int tankButton = 5;
int tankButtonState = 0;

bool tankHit = false;
const uint16_t tankFireDelay = 1000;
uint16_t tankShootCooldownTotal;
uint16_t tankShootCooldownCurrent;

bool tankJustFired = false;
int tankShotCenterWidth;
uint16_t backgroundColour2;

bool moveLeft = true;

void setUpTank(int ceilingHeight, int floorHeight) {
  accelerometer.setup();
  accelerometer.setBias();
  maxHeightTank = ceilingHeight;
  minHeightTank = floorHeight;
  pinMode(tankButton, INPUT);
}

void drawTank() {

  tankTopLeftWidth = 40;
  tankTopLeftHeight = minHeightTank - 3;

  for (int height = tankTopLeftHeight; height > tankTopLeftHeight - 14; height--) {
    for (int width = tankTopLeftWidth; width < tankTopLeftWidth + 12; width++) {
      tft.drawPixel(height, width, ST77XX_GREEN);
    }
  }

  for (int height = tankTopLeftHeight - 2; height > tankTopLeftHeight - 12; height--) {
    tft.drawPixel(height, tankTopLeftWidth - 1, GRAY);
    tft.drawPixel(height, tankTopLeftWidth - 2, GRAY);
    tft.drawPixel(height, tankTopLeftWidth - 3, GRAY);
    tft.drawPixel(height, tankTopLeftWidth + 12, GRAY);
    tft.drawPixel(height, tankTopLeftWidth + 13, GRAY);
    tft.drawPixel(height, tankTopLeftWidth + 14, GRAY);
  }

  for (int height = tankTopLeftHeight - 4; height > tankTopLeftHeight - 10; height--) {
    for (int width = tankTopLeftWidth + 2; width < tankTopLeftWidth + 10; width++) {
      tft.drawPixel(height, width, GRAY);
    }
  }

  for (int height = tankTopLeftHeight + 2; height > tankTopLeftHeight - 4; height--) {
    for (int widthOffset = 4; widthOffset < 8; widthOffset++) {
      tft.drawPixel(height, tankTopLeftWidth + widthOffset, GRAY);
    }
  }
}

void updateTankPosition() {
  if (tankTopLeftWidth > oldTankTopLeftWidth) {
    // Moved Right
    tft.drawPixel(tankTopLeftHeight + 1, tankTopLeftWidth + 3, ST77XX_ORANGE);
    tft.drawPixel(tankTopLeftHeight + 2, tankTopLeftWidth + 3, ST77XX_ORANGE);

    for (int i = 0; i >= -13; i--) {
      int offsetOrange = 0;
      if (i < -1 && i > -12) {
        offsetOrange = -3;
      }
      tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth - 1 + offsetOrange, ST77XX_ORANGE);

      int offsetGreen = 0;
      if (i < -3) {
        offsetGreen = -2;
      }
      if (i >= -9) {
        tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth + 3 + offsetGreen, ST77XX_GREEN);
      }
      tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth + 11, ST77XX_GREEN);
    }

    for (int i = 2; i >= -11; i--) {
      int offset = 0;
      if (i < -3) {
        offset = 2;
      }
      if (i >= -9) {
        tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth + 7 + offset, GRAY);
      }
      if (i <= -2) {
        tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth - 1, GRAY);
        tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth + 14, GRAY);
      }
    }
  } else if (tankTopLeftWidth < oldTankTopLeftWidth) {
    // Moved Left
    tft.drawPixel(tankTopLeftHeight + 1, tankTopLeftWidth + 8, ST77XX_ORANGE);
    tft.drawPixel(tankTopLeftHeight + 2, tankTopLeftWidth + 8, ST77XX_ORANGE);

    for (int i = 0; i >= -13; i--) {
      int offsetOrange = 0;
      if (i < -1 && i > -12) {
        offsetOrange = 3;
      }
      tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth + 12 + offsetOrange, ST77XX_ORANGE);

      int offsetGreen = 0;
      if (i < -3) {
        offsetGreen = 2;
      }
      if (i >= -9) {
        tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth + 8 + offsetGreen, ST77XX_GREEN);
      }
      tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth, ST77XX_GREEN);
    }

    for (int i = 2; i >= -11; i--) {
      int offset = 0;
      if (i < -3) {
        offset = -2;
      }
      if (i >= -9) {
        tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth + 4 + offset, GRAY);
      }
      if (i <= -2) {
        tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth - 3, GRAY);
        tft.drawPixel(tankTopLeftHeight + i, tankTopLeftWidth + 12, GRAY);
      }
    }
  }
}

void moveTank(bool singlePlayer) {

  tankMoveDelayTimerCurrent = millis();
  oldTankTopLeftWidth = tankTopLeftWidth;

  if (singlePlayer) {

    if (tankMoveDelayTimerCurrent - tankMoveDelayTimerStart > tankMoveDelay * 4) {

      if (moveLeft) {
        tankTopLeftWidth--;

        if (tankTopLeftWidth < 3) {
          tankTopLeftWidth = 3;
          moveLeft = false;
        }
      } else {
        tankTopLeftWidth++;

        if (tankTopLeftWidth > 225) {
          tankTopLeftWidth = 225;
          moveLeft = true;
        }
      }
    }
  } else {
    accelerometer.updateBias();
    //  .ax() and .ay() to read axis
    if (tankMoveDelayTimerCurrent - tankMoveDelayTimerStart > tankMoveDelay) {
      if (accelerometer.ay() > 0.1) {
        tankTopLeftWidth--;
      } else if (accelerometer.ay() < -0.1) {
        tankTopLeftWidth++;
      }

      if (tankTopLeftWidth < 3) {
        tankTopLeftWidth = 3;
      }
      if (tankTopLeftWidth > 225) {
        tankTopLeftWidth = 225;
      }
    }
  }
}

void tankHitEnemy() {
  tankHit = false;

  for (int i = 0; i < numEnemies; i++) {
    if ((tankShotCenterWidth <= enemies[i].enemyWidth + enemies[i].enemySize / 2) && (tankShotCenterWidth + 4 >= enemies[i].enemyWidth - enemies[i].enemySize / 2)) {
      enemies[i].takeDamage(2);
      if (enemies[i].enemyHealth <= 0) {
        updateScoreValue(enemies[i].enemySize);
        tone(speakerPin, NOTE_GS1, 500);
        killEnemy(i);
        i--;
      } else {
        tone(speakerPin, NOTE_E4, 250);
        drawEnemy(enemies[i], true);
      }
      tankHit = true;
    }
  }

  if (!tankHit) {
    tone(speakerPin, NOTE_C4, 100);
  }
}

void tankHitPlayer() {
  if (tankShotCenterWidth >= playerCenterWidth - (playerSizeX / 2) && tankShotCenterWidth <= playerCenterWidth + (playerSizeX / 2)) {
    drawPlayer(true);
  }
}

void tankShoot() {

  int bulletSpawnWidth = tankTopLeftWidth + 4;

  for (int height = tankTopLeftHeight + 3; height < maxHeightTank; height++) {
    for (int width = bulletSpawnWidth; width < bulletSpawnWidth + 4; width++) {
      tft.drawPixel(height, width, ST77XX_MAGENTA);
    }
  }

  tankShootCooldownTotal = millis();
  tankJustFired = true;
  tankShotCenterWidth = bulletSpawnWidth;
  tankHitEnemy();
  tankHitPlayer();
}

void tankClearBullets() {

  for (int height = tankTopLeftHeight + 3; height < maxHeightTank; height++) {
    for (int width = tankShotCenterWidth; width < tankShotCenterWidth + 4; width++) {
      tft.drawPixel(height, width, backgroundColour);
    }
  }
  tankHitPlayer();

  if (tankHit) {
    redrawEnemies();
  }
}

void readTankButtonInputs(bool singlePlayer) {

  tankShootCooldownCurrent = millis();

  if (singlePlayer) {
    if (tankShootCooldownCurrent - tankShootCooldownTotal > tankFireDelay * 2) {
      tankShoot();
    }
  } else {
    if (tankShootCooldownCurrent - tankShootCooldownTotal > tankFireDelay) {

      tankButtonState = digitalRead(tankButton);

      if (tankButtonState == HIGH) {
        tankShoot();
      }
    }
  }
}

void tankActions(bool singlePlayer) {

  if (tankJustFired && millis() - tankShootCooldownTotal > 50) {
    tankClearBullets();
    tankJustFired = false;
  }

  readTankButtonInputs(singlePlayer);
  moveTank(singlePlayer);
}
