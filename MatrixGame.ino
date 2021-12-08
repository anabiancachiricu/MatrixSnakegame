#include <LiquidCrystal.h>
#include "LedControl.h"

//declare matrix pins
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

//declare diaplay pins
const int RS = 13;
const int enable = 6;
const int d4 = 7;
const int d5 = 4;
const int d6 = 3;
const int d7 = 8;
const int pinBrightnessLCD = 9;
//const int pinContrastLCD = 5;

//decalre joystick pins
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;

bool swState = LOW;
bool lastSwState = LOW;
int xValue = 0;
int yValue = 0;
bool joyMoved = false;
bool joyMovedX = false;
bool joyMovedY = false;
int digit = 0;
int minThreshold = 400;
int minThresholdGame = 200;
int maxThreshold = 600;

int index = 0;
int indexAbout = 0;
int indexSettings = 0;
int indexAlphabet = 0;

String menuItems[] = {"> New Game", "> Highscores", "> Settings", "> About"};
String aboutItems[] = {"~ Snake Game", "~ by Bianca Chiricu", "~ github", "> Back to menu"};
String settingsItems[] = {"~ Starting level", "~ LCD Contrast", "~ LCD Brightness",  "~ Matrix Brightness", "~Set name", "> Back to menu"};
String difficultyItems[] = {"Easy", "Medium", "Hard"};
char alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
/////////////////0    1     2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25
int currentNamePositions[3];
int currentSegment = 0;
String currentMenu = "menu";
int currentDifficultyIndex = 0;

int currentBrightnessLCD = 128;
const int minBrightness = 0;
const int maxBrightness = 255;
const int decreaseBrightness = 10;

int currentContrastLCD = 50;
const int minContrast = 0;
const int maxContrast = 255;
const int decreaseContrast = 10;

//game values
byte matrixBrightness = 2;
const int minMatrixBrightness = 0;
const int maxMatrixBrightness = 15;

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

byte xCurrentPos = 0;
byte yCurrentPos = 0;
byte xLastFoodPos = 0;
byte yLastFoodPos = 0;
byte xNewFoodPos = 0;
byte yNewFoodPos = 0;

const byte moveInterval = 100;
unsigned long long lastMoved = 0;

const byte matrixSize = 8;
bool matrixChanged = true;

const int interruptCycle = 200;
volatile unsigned long long lastInterrupt = 0;

bool matrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};
byte matrixByte[matrixSize] = {
  B00000000,
  B01000100,
  B00101000,
  B00010000,
  B00010000,
  B00010000,
  B00000000,
  B00000000
};

int score=0;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

void setup() {
  // set up the LCD's number of columns and rows:

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinBrightnessLCD, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, FALLING);
  analogWrite(pinBrightnessLCD, currentBrightnessLCD);
  //analogWrite(pinContrastLCD, currentContrastLCD);

  lcd.begin(16, 2);
  //intro();
  displayMenu();
  Serial.begin(9600);

  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  matrix[xPos][yPos] = 1;

}

void loop() {

  if (currentMenu == "menu")
  {
    displayMenu();
  }
  else if (currentMenu == "about")
  {
    displayAbout();
  }
  else if (currentMenu == "settings")
  {
    displaySettings();
  }
  else if (currentMenu == "LCDBrightness")
  {
    displayBrightnessLCD();
  }
  //  else if (currentMenu == "LCDContrast")
  //  {
  //    displayContrastLCD();
  //  }
  // game();
  else if (currentMenu == "MatrixBrightness")
  {
    displayBrightnessMatrix();
  }
  else if (currentMenu == "startingLevel")
  {
    displaystartingLevel();
  }
  else if (currentMenu == "newGame")
  {
    game();
  }
  //  else if (currentMenu == "setName")
  //  {
  //    displaySetName();
  //  }

}

//introduction
void intro()
{
  lcd.begin(16, 2);
  lcd.print("Hello, snake!");
  delay(5000);
  lcd.clear();
  lcd.print("Did you miss me?");
  lcd.setCursor(5, 1);
  lcd.print("ssssss");
  delay(5000);
  lcd.clear();
  lcd.print("I'm small");
  lcd.setCursor(0, 1);
  lcd.print("and hungry.");
  delay(5000);
  lcd.clear();
  lcd.print("I need to eat");
  lcd.setCursor(0, 1);
  lcd.print("so I can grow");
  delay(5000);
  lcd.clear();
  lcd.print("But there are");
  lcd.setCursor(0, 1);
  lcd.print("some traps");
  delay(5000);
  lcd.clear();
  lcd.print("that I need to");
  lcd.setCursor(0, 1);
  lcd.print("avoid to survive");
  delay(5000);
  lcd.clear();
  lcd.print("Will you help me");
  lcd.setCursor(7, 1);
  lcd.print("?");
  delay(5000);
  lcd.clear();
}

void deadScreen()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ups, you died");
  lcd.setCursor(0, 1);
  lcd.print("Your score is:");
  delay(5000);
}

//game menu
void displayMenu()
{
  currentMenu = "menu";
  xValue = analogRead(pinX);

  if (xValue < minThreshold && joyMoved == false)
  {
    if (index > 0) {
      index--;
    } else {
      index = 3;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (index < 3) {
      index++;
    } else {
      index = 0;
    }
    joyMoved = true;
  }

  if (joyMoved == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(menuItems[index]);
    lcd.setCursor(0, 1);
    //lcd.clear();
    if (index < 3)
      lcd.print(menuItems[index + 1]);
    else
      lcd.print(menuItems[0]);

    lcd.setCursor(0, 0);
    lcd.blink();
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

//about menu
void displayAbout()
{
  currentMenu = "about";
  xValue = analogRead(pinX);

  if (xValue < minThreshold && joyMoved == false)
  {
    if (indexAbout > 0) {
      indexAbout--;
    } else {
      indexAbout = 3;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (indexAbout < 3) {
      indexAbout++;
    } else {
      indexAbout = 0;
    }
    joyMoved = true;
  }

  if (joyMoved == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(aboutItems[indexAbout]);
    lcd.setCursor(0, 1);
    //lcd.clear();
    if (indexAbout < 3)
      lcd.print(aboutItems[indexAbout + 1]);
    else
      lcd.print(aboutItems[0]);

    lcd.setCursor(0, 0);
    lcd.blink();
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

//settings menu
void displaySettings()
{
  currentMenu = "settings";
  xValue = analogRead(pinX);

  if (xValue < minThreshold && joyMoved == false)
  {
    if (indexSettings > 0) {
      indexSettings--;
    } else {
      indexSettings = 5;
    }
    joyMoved = true;

  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (indexSettings < 5) {
      indexSettings++;
    } else {
      indexSettings = 0;
    }
    joyMoved = true;

  }

  if (joyMoved == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(settingsItems[indexSettings]);
    lcd.setCursor(0, 1);
    //lcd.clear();
    if (indexSettings < 5)
      lcd.print(settingsItems[indexSettings + 1]);
    else
      lcd.print(settingsItems[0]);

    lcd.setCursor(0, 0);
    lcd.blink();
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

void displaystartingLevel()
{
  currentMenu = "startingLevel";
  xValue = analogRead(pinX);

  if (xValue < minThreshold && joyMoved == false)
  {
    if (currentDifficultyIndex > 0) {
      currentDifficultyIndex--;
    } else {
      currentDifficultyIndex = 2;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (currentDifficultyIndex < 2) {
      currentDifficultyIndex++;
    } else {
      currentDifficultyIndex = 0;
    }
    joyMoved = true;
  }

  if (joyMoved == true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(difficultyItems[currentDifficultyIndex]);
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

void pressButton()
{
  Serial.println(currentMenu);
  if (currentMenu == "menu")
  {
    if (index == 3)
    {
      lcd.clear();
      displayAbout();
    }
    else if (index == 2)
    {
      lcd.clear();
      displaySettings();
    }
    else if (index == 0)
    {
      lcd.clear();
      game();
    }

  }
  else
  {
    if (currentMenu == "about")
    {
      if (indexAbout == 3)
      {
        lcd.clear();
        displayMenu();
      }
    }
    else
    {
      if (currentMenu == "settings")
      {
        if (indexSettings == 5)
        {
          lcd.clear();
          displayMenu();
        }
        else
        {
          if (indexSettings == 2)
          {
            lcd.clear();
            displayBrightnessLCD();
          }
          else
          {
            //            if (indexSettings == 1)
            //            {
            //              lcd.clear();
            //              displayContrastLCD();
            //            }

            if (indexSettings == 3)
            {
              lcd.clear();
              displayBrightnessMatrix();
            }
            else
            {
              if (indexSettings == 0)
              {
                lcd.clear();
                displaystartingLevel();
              }
              //              else
              //              {
              //                if (indexSettings == 4)
              //                {
              //                  lcd.clear();
              //                  displaySetName();
              //                }
              //              }
            }
          }
        }
      }
      else
      {
        if (currentMenu == "LCDBrightness")
        {
          lcd.clear();
          displaySettings();
        }
        else
        {
          //          if (currentMenu == "LCDContrast")
          //          {
          //            lcd.clear();
          //            displaySettings();
          //          }

          if (currentMenu == "MatrixBrightness")
          {
            lcd.clear();
            displaySettings();
          }
          else if (currentMenu == "startingLevel")
          {
            lcd.clear();
            displaySettings();
          }
        }
      }
    }
  }
}

void displayBrightnessLCD()
{
  currentMenu = "LCDBrightness";
  xValue = analogRead(pinX);

  if (xValue < minThreshold && joyMoved == false)
  {
    if (currentBrightnessLCD - decreaseBrightness > minBrightness)
    {
      currentBrightnessLCD = currentBrightnessLCD - decreaseBrightness;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (currentBrightnessLCD + decreaseBrightness < maxBrightness)
    {
      currentBrightnessLCD = currentBrightnessLCD + decreaseBrightness;
    }
    joyMoved = true;
  }

  if (joyMoved == true)
  {
    analogWrite(pinBrightnessLCD, currentBrightnessLCD);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current brightness:");
    lcd.setCursor(6, 1);
    lcd.print(currentBrightnessLCD);
  }

  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//void displaySetName()
//{
//  currentMenu = "setName";
//  xValue = analogRead(pinX);
//  yValue = analogRead(pinY);
//
//  if (yValue > maxThreshold && joyMoved == false)
//  {
//    if (currentSegment > 0)
//    {
//      currentSegment--;
//    }
//    else
//    {
//      currentSegment = 2;
//    }
//    joyMovedY = true;
//  }
//
//  if (yValue < minThreshold && joyMoved == false)
//  {
//    if (currentSegment < 2)
//    {
//      currentSegment++;
//    }
//    else
//    {
//      currentSegment = 0;
//    }
//    joyMovedY = true;
//  }
//
//  if (xValue < minThreshold && joyMoved == false)
//  {
//    if (currentNamePositions[currentSegment] > 0)
//    {
//      currentNamePositions[currentSegment]--;
//    }
//    else
//    {
//      currentNamePositions[currentSegment]=25;
//    }
//    joyMovedX = true;
//  }
//
//  if (xValue > maxThreshold && joyMoved == false) {
//     if (currentNamePositions[currentSegment] < 25)
//    {
//      currentNamePositions[currentSegment]++;
//    }
//    else
//    {
//      currentNamePositions[currentSegment]=0;
//    }
//    joyMovedX = true;
//  }
//
//  if (xValue >= minThreshold && xValue <= maxThreshold)
//  {
//    joyMoved = false;
//  }
//  if (yValue >= minThreshold && yValue <= maxThreshold)
//  {
//    joyMoved = false;
//  }
//
//}
void displayBrightnessMatrix()
{
  currentMenu = "MatrixBrightness";
  xValue = analogRead(pinX);

  if (xValue < minThreshold && joyMoved == false)
  {
    if (matrixBrightness  > minMatrixBrightness)
    {
      matrixBrightness --;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (matrixBrightness < maxMatrixBrightness)
    {
      matrixBrightness ++;
    }
    joyMoved = true;
  }

  if (joyMoved == true)
  {
    lc.setIntensity(0, matrixBrightness);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current brightness:");
    lcd.setCursor(6, 1);
    lcd.print(matrixBrightness);
  }

  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//void displayContrastLCD()
//{
//  currentMenu = "LCDContrast";
//  xValue = analogRead(pinX);
//
//  if (xValue < minThreshold && joyMoved == false)
//  {
//    if (currentContrastLCD - decreaseContrast > minContrast)
//    {
//      currentContrastLCD = currentContrastLCD - decreaseContrast;
//    }
//    joyMoved = true;
//  }
//
//  if (xValue > maxThreshold && joyMoved == false) {
//    if (currentContrastLCD + decreaseContrast < maxContrast)
//    {
//      currentContrastLCD = currentContrastLCD + decreaseContrast;
//    }
//    joyMoved = true;
//  }
//
//  if (joyMoved == true)
//  {
//    analogWrite(pinContrastLCD, currentContrastLCD);
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("Current contrast:");
//    lcd.setCursor(6, 1);
//    lcd.print(currentContrastLCD);
//  }
//
//  if (xValue >= minThreshold && xValue <= maxThreshold)
//  {
//    joyMoved = false;
//  }
//}

void game()
{
  currentMenu = "newGame";
  lcd.setCursor(0, 0);
  lcd.print("Okaaay, let's go!");
  lcd.print(score);
  if (millis() - lastMoved > moveInterval) {
    // game logic
    updatePositions();
    lastMoved = millis();
  }
  if (matrixChanged == true) {
    // matrix display logic
    updateMatrix();
    //generateFood();
    matrixChanged = false;
  }
}

void updateByteMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, matrixByte[row]);
  }
}
void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}
void generateFood() {
  xLastFoodPos = xCurrentPos;
  yLastFoodPos = yCurrentPos;
  xNewFoodPos = random(0, 8);
  yNewFoodPos = random(0, 8);
  matrix[xLastFoodPos][yLastFoodPos] = 0;
  matrix[xNewFoodPos][yNewFoodPos] = 1;
  matrixChanged = true;
}

void updatePositions() {
  int xValue = analogRead(pinX);
  int yValue = analogRead(pinY);
  xLastPos = xPos;
  yLastPos = yPos;
  if (xValue < minThresholdGame) {
    if (xPos < matrixSize - 1) {
      xPos++;
      score++;
    }
    else {
      xPos = 0;
      dead();
    }
  }
  if (xValue > maxThreshold) {
    if (xPos > 0) {
      xPos--;
      score++;
    }
    else {
      xPos = matrixSize - 1;
      dead();
    }
  }
  if (yValue > maxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
      score++;
    }
    else {
      yPos = 0;
      dead();
    }
  }
  if (yValue < minThresholdGame) {
    if (yPos > 0) {
      yPos--;
      score++;
    }
    else {
      yPos = matrixSize - 1;
      dead();
    }
  }
  if (xPos != xLastPos || yPos != yLastPos) {
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;
  }
}

void dead()
{
  currentMenu = "dead";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("You hit your");
   lcd.setCursor(0,1);
  lcd.print("head");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("And died");
  lcd.setCursor(0,1);
  lcd.print("Rest in peace");
  delay(5000);
  lcd.clear();
  lcd.print("Your score is:");
  lcd.print(score);
  delay(5000);
  displayMenu();
  
}
void scrollText(char text[])
{
  int unsigned i = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  while (text[i] != '\0')
  {
    lcd.print(text[i]);
    if (i >= 16)
    {
      lcd.command(0x18); //Scrolling text to Right
    }
    delay(300);
    i++;
  }
  delay(5000);
  lcd.clear();
}

void handleInterrupt() {
  unsigned long long current = millis();
  if (current - lastInterrupt > interruptCycle) {
    lastInterrupt = current;
    pressButton();
  }
}
