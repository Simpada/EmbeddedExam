
#include "gameStats.h"

uint16_t lastSpawnTime;
long spawnInterval = 2000;
const long minInterval = 5;

const int intervalDecrement = 10;
uint16_t lastIntervalDecreaseTime = 0;

const uint16_t enemyMoveDelay = 10;
uint16_t enemyMoveDelayTotal;
uint16_t enemyMoveDelayCurrent;

const int lanes[6] = { 100, 86, 72, 58, 44, 30 };

uint16_t backgroundColour;

#ifndef ENEMY_H
#define ENEMY_H

class Enemy {
public:
  int enemyWidth = 0;
  int enemyHeight = 0;

  int enemySize = 0;

  int enemyDirection = 0;

  int enemyHealth = 0;
  uint16_t enemyColor = 0x0000;

  Enemy()
    : enemyWidth(0), enemyHeight(0), enemySize(0), enemyDirection(0), enemyHealth(0), enemyColor(0) {}

  Enemy(int width, int height, int size, int direction, int health, uint16_t color) {
    enemyWidth = width;
    enemyHeight = height;
    enemySize = size;
    enemyDirection = direction;
    enemyHealth = health;
    enemyColor = color;
  }

  bool move() {

    for (int width = enemyWidth + (-enemyDirection * (enemySize / 2)); width <= enemyWidth + (-enemyDirection * (enemySize / 2)); width++) {
      for (int height = enemyHeight - (enemySize / 2); height <= enemyHeight + (enemySize / 2); height++) {
        tft.drawPixel(height, width, backgroundColour);
      }
    }
    enemyWidth += enemyDirection;

    for (int width = enemyWidth + (enemyDirection * (enemySize / 2)); width <= enemyWidth + (enemyDirection * (enemySize / 2)); width++) {
      for (int height = enemyHeight - (enemySize / 2); height <= enemyHeight + (enemySize / 2); height++) {
        tft.drawPixel(height, width, enemyColor);
      }
    }

    if (enemyWidth < -enemySize || enemyWidth > 240 + enemySize) {
      return false;
    } else {
      return true;
    }
  }

  void takeDamage(int amount) {
    enemyHealth -= amount;

    if (enemyHealth <= 1) {
      enemyColor = ST77XX_YELLOW;
    } else if (enemyHealth <= 2) {
      enemyColor = ST77XX_ORANGE;
    }
  }
};

#endif

// This must be declared here, as it requires the enemy class to be declared beforehand
const int maxEnemies = 20;
Enemy enemies[maxEnemies];
int numEnemies = 0;


void clearEnemies() {
  for (int i = 0; i < numEnemies; i++) {
    enemies[i] = Enemy();
  }
  numEnemies = 0;
}

void setUpEnemies(uint16_t worldColor) {
  backgroundColour = worldColor;
  spawnInterval = 2000;
  clearEnemies();
}

void addNewEnemy(Enemy enemy) {
  if (numEnemies < maxEnemies) {
    enemies[numEnemies] = enemy;
    numEnemies++;
  }
}

void removeEnemy(int index) {
  if (index >= 0 && index < numEnemies) {
    for (int i = index; i < numEnemies - 1; i++) {
      enemies[i] = enemies[i + 1];
    }
    numEnemies--;
  }
}



void drawEnemy(Enemy enemy, bool draw) {

  int halfSize = enemy.enemySize / 2;
  int centerHeight = enemy.enemyHeight - halfSize;
  int centerWidth = enemy.enemyWidth - halfSize;

  if (draw) {
    tft.fillRect(centerHeight, centerWidth, enemy.enemySize + 1, enemy.enemySize + 1, enemy.enemyColor);
  } else {
    tft.fillRect(centerHeight, centerWidth, enemy.enemySize + 1, enemy.enemySize + 1, backgroundColour);
  }
}

void redrawEnemies() {
  for (int i = 0; i < numEnemies; i++) {
    drawEnemy(enemies[i], true);
  }
}

Enemy createEnemy(int enemyType, int lane) {
  int size, health;
  uint16_t color;

  switch (enemyType) {
    case 0:
      size = 10;
      health = 1;
      color = ST77XX_YELLOW;
      break;
    case 1:
      size = 16;
      health = 2;
      color = ST77XX_ORANGE;
      break;
    case 2:
      size = 24;
      health = 3;
      color = ST77XX_RED;
      break;
    default:
      size = 10;
      health = 1;
      color = ST77XX_YELLOW;
      break;
  }

  int direction = random(2) == 0 ? -1 : 1;
  int width = direction == 1 ? -size : 240 + size;
  int height = lanes[lane];

  return Enemy(width, height, size, direction, health, color);
}

void spawnEnemy() {

  int enemyType = random(0, 3);
  int lane = random(0, 6);

  Enemy newEnemy = createEnemy(enemyType, lane);

  addNewEnemy(newEnemy);

  drawEnemy(newEnemy, true);
}

void killEnemy(int index) {

  Enemy enemy = enemies[index];
  drawEnemy(enemy, false);

  removeEnemy(index);
}

void moveAllEnemies() {

  enemyMoveDelayCurrent = millis();

  if (enemyMoveDelayCurrent - enemyMoveDelayTotal > enemyMoveDelay) {

    for (int i = 0; i < numEnemies; i++) {
      if (!enemies[i].move()) {
        removeEnemy(i);
        i--;
      }
    }
    enemyMoveDelayTotal = millis();
  }
}

void spawnEnemiesOverTime() {

  uint16_t currentTime = millis();

  if (currentTime - lastSpawnTime > spawnInterval) {
    spawnEnemy();
    lastSpawnTime = currentTime;

    if (currentTime - lastIntervalDecreaseTime > 2000) {
      lastIntervalDecreaseTime = currentTime;
      if (spawnInterval > minInterval) {
        spawnInterval -= intervalDecrement;
      }
    }
  }
}

void enemyActions() {
  spawnEnemiesOverTime();
  moveAllEnemies();
}
