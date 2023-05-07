
#include "enemy.h"

// joystick setup
#define JOYSTICK_X_PIN A0
#define JOYSTICK_Y_PIN A1
#define BUTTON_PIN 3

int joystickXValue = 0;
int joystickYValue = 0;
ezButton joystickButton(BUTTON_PIN);
int joystickButtonState = 0;

// button setup
const int directionalButtons = A2;
int directionalButtonValue;

// player setup
int maxHeightPlayer;
int maxDepth;

int playerCenterWidth = 120;
int playerCenterHeight = 30;

const int playerSizeX = 10;
const int playerSizeY = 10;

int oldCenterX;
int oldCenterY;

uint16_t playerColour = ST77XX_GREEN;

const uint16_t playerMoveDelay = 20;
uint16_t playerMoveDelayTotal;
uint16_t playerMoveDelayCurrent;

bool playerHit = false;
const uint16_t playerFireDelay = 1000;
uint16_t playerShootCooldownTotal;
uint16_t playerShootCooldownCurrent;

bool justFired = false;
int shotDirection;
int shotCenterWidth;
int shotCenterHeight;

const int teleportDistance = 40;
bool canTeleport = true;
const uint16_t teleportCooldown = 1000;
uint16_t teleportCooldownTotal = 0;
uint16_t teleportCooldownCurrent;

void setUpPlayer(int ceilingHeight, int floorHeight) {
  maxHeightPlayer = ceilingHeight;
  maxDepth = floorHeight;
  joystickButton.setDebounceTime(50);
  playerCenterWidth = 120;
  playerCenterHeight = 30;
  playerHealth = 3;
  playerScore = 0;
}

void drawPlayer(bool draw) {
  uint16_t color = playerColour;
  if (!draw) {
    color = backgroundColour;
  }
  for (int width = playerCenterWidth - (playerSizeX / 2); width <= playerCenterWidth + (playerSizeX / 2); width++) {
    for (int height = playerCenterHeight - (playerSizeY / 2); height <= playerCenterHeight + (playerSizeY / 2); height++) {
      tft.drawPixel(height, width, color);
    }
  }
}

void updatePlayerPosition() {

  int xDir = 0;
  if (playerCenterWidth > oldCenterX) {
    xDir = -1;
  } else if (playerCenterWidth < oldCenterX) {
    xDir = 1;
  }

  int yDir = 0;
  if (playerCenterHeight > oldCenterY) {
    yDir = -1;
  } else if (playerCenterHeight < oldCenterY) {
    yDir = 1;
  }

  if (xDir != 0) {
    for (int width = oldCenterX + (xDir * (playerSizeX / 2)); width <= oldCenterX + (xDir * (playerSizeX / 2)); width++) {
      for (int height = oldCenterY - (playerSizeY / 2); height <= oldCenterY + (playerSizeY / 2); height++) {
        tft.drawPixel(height, width, backgroundColour);
      }
    }

    for (int width = playerCenterWidth + (-xDir * (playerSizeX / 2)); width <= playerCenterWidth + (-xDir * (playerSizeX / 2)); width++) {
      for (int height = playerCenterHeight - (playerSizeY / 2); height <= playerCenterHeight + (playerSizeY / 2); height++) {
        tft.drawPixel(height, width, playerColour);
      }
    }
  }

  if (yDir != 0) {
    for (int width = oldCenterX - (playerSizeX / 2); width <= oldCenterX + (playerSizeX / 2); width++) {
      for (int height = oldCenterY + (yDir * (playerSizeY / 2)); height <= oldCenterY + (yDir * (playerSizeY / 2)); height++) {
        tft.drawPixel(height, width, backgroundColour);
      }
    }

    for (int width = playerCenterWidth - (playerSizeX / 2); width <= playerCenterWidth + (playerSizeX / 2); width++) {
      for (int height = playerCenterHeight + (-yDir * (playerSizeY / 2)); height <= playerCenterHeight + (-yDir * (playerSizeY / 2)); height++) {
        tft.drawPixel(height, width, playerColour);
      }
    }
  }
}

void changePlayerColour(uint16_t color) {
  playerColour = color;
  drawPlayer(true);
}

void checkPlayerEnemyCollision() {
  for (int i = 0; i < numEnemies; i++) {
    if (abs(enemies[i].enemyHeight - playerCenterHeight) < (enemies[i].enemySize / 2 + playerSizeY / 2) && abs(enemies[i].enemyWidth - playerCenterWidth) < (enemies[i].enemySize / 2 + playerSizeX / 2)) {
      playerHealth -= 1;
      removeHearts(playerHealth);
      if (playerHealth <= 0) {
        gameOverSetup();
      } else {
        killEnemy(i);
        tone(speakerPin, NOTE_C2, 250);
        updateScoreValue(enemies[i].enemySize);
        i--;
        drawPlayer(true);
      }
    }
  }
}

void checkPlayerBounds() {
  if (playerCenterHeight + (playerSizeY / 2) > maxHeightPlayer - 1) {
    playerCenterHeight = maxHeightPlayer - 1 - (playerSizeY / 2);
  }
  if (playerCenterHeight - (playerSizeY / 2) < maxDepth + 1) {
    playerCenterHeight = maxDepth + 1 + (playerSizeY / 2);
  }
  if (playerCenterWidth + (playerSizeX / 2) > 239) {
    playerCenterWidth = 239 - (playerSizeX / 2);
  }
  if (playerCenterWidth - (playerSizeX / 2) < 0) {
    playerCenterWidth = 0 + (playerSizeX / 2);
  }
}

void movePlayer() {

  oldCenterX = playerCenterWidth;
  oldCenterY = playerCenterHeight;

  playerMoveDelayCurrent = millis();

  if (playerMoveDelayCurrent - playerMoveDelayTotal > playerMoveDelay) {

    joystickXValue = analogRead(JOYSTICK_X_PIN);
    joystickYValue = analogRead(JOYSTICK_Y_PIN);

    if (joystickXValue < 500) {
      playerCenterHeight++;
      playerMoveDelayTotal = millis();
    } else if (joystickXValue > 540) {
      playerCenterHeight--;
      playerMoveDelayTotal = millis();
    }
    if (joystickYValue < 482) {
      playerCenterWidth++;
      playerMoveDelayTotal = millis();
    } else if (joystickYValue > 522) {
      playerCenterWidth--;
      playerMoveDelayTotal = millis();
    }

    checkPlayerBounds();
  }
  checkPlayerEnemyCollision();
}

void teleport(int direction) {
  if (canTeleport) {
    drawPlayer(false);

    playerCenterHeight += direction * teleportDistance;
    checkPlayerBounds();

    changePlayerColour(GRAY);
    canTeleport = false;
    teleportCooldownTotal = millis();
  }
}

void playerHitenemies() {

  playerHit = false;
  for (int i = 0; i < numEnemies; i++) {
    if ((shotDirection == 1 && enemies[i].enemyWidth > shotCenterWidth) || (shotDirection == -1 && enemies[i].enemyWidth < shotCenterWidth)) {
      if (enemies[i].enemyHeight + (enemies[i].enemySize / 2) >= shotCenterHeight - 1 && enemies[i].enemyHeight - (enemies[i].enemySize / 2) <= shotCenterHeight + 1) {
        enemies[i].takeDamage(1);
        if (enemies[i].enemyHealth <= 0) {
          updateScoreValue(enemies[i].enemySize);
          tone(speakerPin, NOTE_GS1, 500);
          killEnemy(i);
          i--;
        } else {
          tone(speakerPin, NOTE_E4, 250);
          drawEnemy(enemies[i], true);
        }
        playerHit = true;
      }
    }
  }

  if (!playerHit) {
    tone(speakerPin, NOTE_C4, 100);
  }
}

void playerShoot(int direction) {

  if (playerShootCooldownCurrent - playerShootCooldownTotal > playerFireDelay) {

    int bulletSpawnWidth = playerCenterWidth + (direction * (2 + (playerSizeX / 2)));

    for (int height = playerCenterHeight - 1; height <= playerCenterHeight + 1; height++) {
      if (direction > 0) {
        for (int width = bulletSpawnWidth + 1; width <= 239; width++) {
          tft.drawPixel(height, width, ST77XX_MAGENTA);
        }
      } else {
        for (int width = bulletSpawnWidth - 1; width >= 0; width--) {
          tft.drawPixel(height, width, ST77XX_MAGENTA);
        }
      }
    }

    shotDirection = direction;
    shotCenterWidth = bulletSpawnWidth;
    shotCenterHeight = playerCenterHeight;

    justFired = true;
    playerShootCooldownTotal = millis();

    playerHitenemies();
  }
}

void playerClearBullets() {
  for (int height = shotCenterHeight - 1; height <= shotCenterHeight + 1; height++) {
    if (shotDirection > 0) {
      for (int width = shotCenterWidth + 1; width <= 239; width++) {
        tft.drawPixel(height, width, backgroundColour);
      }
    } else {
      for (int width = shotCenterWidth - 1; width >= 0; width--) {
        tft.drawPixel(height, width, backgroundColour);
      }
    }
  }

  if (playerHit) {
    redrawEnemies();
  }
}

void readDirectionalButtonInputs() {
  joystickButton.loop();
  directionalButtonValue = analogRead(directionalButtons);

  if (directionalButtonValue < 1000) {

    /*
      Down:   >= 750 && < 1000
      Left:   >= 650 && < 750
      Up:     >= 480 && < 650
      Right:  < 480
    */

    if (directionalButtonValue >= 750) {
      // Down
      teleport(-1);
    } else if (directionalButtonValue >= 650 && directionalButtonValue < 750) {
      // Left
      playerShoot(-1);
    } else if (directionalButtonValue >= 480 && directionalButtonValue < 650) {
      // Up
      teleport(1);
    } else if (directionalButtonValue < 480) {
      // Right
      playerShoot(1);
    }
  }
}

void updatePlayerValues() {
  if (!canTeleport) {
    teleportCooldownCurrent = millis();
    if (teleportCooldownCurrent - teleportCooldownTotal > teleportCooldown) {
      canTeleport = true;
      changePlayerColour(ST77XX_GREEN);
    }
  }

  playerShootCooldownCurrent = millis();

  if (justFired && millis() - playerShootCooldownTotal > 50) {
    playerClearBullets();
    justFired = false;
  }
}

void playerActions() {
  readDirectionalButtonInputs();
  movePlayer();
}
