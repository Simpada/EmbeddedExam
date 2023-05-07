
#include "Arduino.h"
#include <Adafruit_ST7789.h>
//#include <SD.h>
#include "pitches.h"

#define GRAY 0x2222

const int speakerPin = 7;

// tft setup
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Stat setup
bool singlePlayer;

const int heartTopPoint = 129;
const int heartLeftmostPoint = 6;

const int heartRightmostPoint = heartLeftmostPoint + 13;

int playerHealth = 3;

long playerScore = 0;
const char* scoreFile = "highscore.txt";

// Gameover variables
bool gameOver = false;

const int gameOverMelody[] = {
  NOTE_A3, NOTE_G3, NOTE_F3, NOTE_E3, NOTE_D3, NOTE_C3
};

const int gameOverNoteDurations[] = {
  4, 4, 4, 4, 2, 1
};

bool playAgainTextVisible = true;
uint16_t playAgainBlinkStart;
uint16_t playAgainBlinkCurrent;


void drawScore() {
  tft.setTextColor(ST77XX_CYAN);
  tft.setRotation(1);
  tft.setCursor(90, 4);

  tft.setTextSize(2);
  tft.print("Score:");

  tft.setCursor(165, 4);
  tft.print(playerScore);

  tft.setRotation(0);
  tft.setTextSize(1);
}

void updateScoreValue(int increase) {
  tft.setRotation(1);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(165, 4);
  tft.print(playerScore);

  tft.setTextColor(ST77XX_CYAN);
  tft.setCursor(165, 4);
  playerScore += increase;
  tft.print(playerScore);

  tft.setRotation(0);
  tft.setTextSize(1);
}

void drawHearts(int numberOfHearts) {
  for (int heartNumber = 0; heartNumber < numberOfHearts; heartNumber++) {
    int offset = 25 * heartNumber;
    int currentDrawHeight = heartTopPoint;


    for (int j = 0; j < 2; j++) {
      for (int drawWidth = heartLeftmostPoint + 2 + offset; drawWidth <= heartRightmostPoint - 2 + offset; drawWidth++) {
        if (drawWidth > heartRightmostPoint - heartLeftmostPoint + offset || drawWidth < heartRightmostPoint - heartLeftmostPoint + offset - 1) {
          tft.drawPixel(currentDrawHeight, drawWidth, ST77XX_RED);
        }
      }
      currentDrawHeight--;
    }

    for (int j = -1; j < 4; j++) {
      for (int k = 0; k < 2; k++) {
        int indent = 0;
        if (j > 0) {
          indent = j * 2;
        }
        for (int drawWidth = heartLeftmostPoint + offset + indent; drawWidth <= heartRightmostPoint + offset - indent; drawWidth++) {
          tft.drawPixel(currentDrawHeight, drawWidth, ST77XX_RED);
        }
        currentDrawHeight--;
      }
    }
  }
}

void removeHearts(int number) {
  if (number >= 0 && number <= 2) {
    int offset = 25 * number;
    int currentDrawHeight = heartTopPoint;

    for (int height = currentDrawHeight; height > currentDrawHeight - 12; height--) {
      for (int width = heartLeftmostPoint + offset; width <= heartRightmostPoint + offset; width++) {
        tft.drawPixel(height, width, ST77XX_BLACK);
      }
    }
  }
}


// bool initSD() {
//   return SD.begin(10);
// }

// int getHighscore() {

//   if (!initSD()) {
//     return 0;
//   }

//   File file = SD.open(scoreFile, FILE_READ);
//   if (file) {
//     int highscore = file.parseInt();
//     file.close();
//     return highscore;
//   } else {
//     return 0;
//   }
// }

// void saveScore() {

//   if (!initSD()) {
//     return;
//   }

//   int highscore = getHighscore();
//   if (playerScore > highscore) {
//     SD.remove(scoreFile);

//     File file = SD.open(scoreFile, FILE_WRITE);
//     if (file) {
//       file.println(playerScore);
//       file.close();
//     }
//   }
// }

void drawGameOverMenu() {

  tft.fillScreen(ST77XX_BLACK);

  tft.setRotation(1);

  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(3);
  tft.setCursor(40, 15);
  tft.print("GAME OVER");

  tft.setTextSize(2);
  // tft.setCursor(20, 110);
  // tft.print("HIGHSCORE:");
  // tft.setCursor(145, 110);
  // tft.print(getHighscore());

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(65, 50);
  tft.print("SCORE:");
  tft.setCursor(145, 50);
  tft.print(playerScore);
}

void gameOverSetup() {

  gameOver = true;

  for (int thisNote = 0; thisNote < sizeof(gameOverMelody) / sizeof(int); thisNote++) {
    int noteDuration = 1000 / gameOverNoteDurations[thisNote];
    tone(speakerPin, gameOverMelody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speakerPin);
  }

  // saveScore();
  drawGameOverMenu();
  tft.setTextSize(1);
  playAgainBlinkStart = millis();
}

bool gameOverMenu() {

  playAgainBlinkCurrent = millis();

  if (playAgainBlinkCurrent - playAgainBlinkStart >= 700) {
    if (playAgainTextVisible) {
      tft.setTextColor(ST77XX_WHITE);
    } else {
      tft.setTextColor(ST77XX_BLACK);
    }
    tft.setCursor(30, 80);
    tft.print("CLICK ANY BUTTON TO PLAY AGAIN");
    playAgainTextVisible = !playAgainTextVisible;
    playAgainBlinkStart = millis();
  }
}
