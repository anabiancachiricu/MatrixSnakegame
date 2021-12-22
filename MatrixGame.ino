#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"

#define SCROLL_NOTE 10
#define BUTTON_NOTE 5000
#define EAT_NOTE 2500

//declare buzzer pin
const int buzzerPin = 5;
int buzzerTone = 1000;

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
int indexHighscore = 0;

String menuItems[] = {"> New Game", "> Highscores", "> Settings", "> About"};
String aboutItems[] = {"~ Snake Game", "~ by Bianca Chiricu", "~ https://github.com/anabiancachiricu/MatrixSnakegame", "> Back to menu"};
String settingsItems[] = {"~ Starting level", "~ LCD Contrast", "~ LCD Brightness",  "~ Matrix Brightness", "~ Set name", "~ Reset Highscore", "~ Set Sound", "> Back to menu"};
String difficultyItems[] = {"Easy", "Medium", "Hard"};
String soundItems[] = {"On", "Off"};
char alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
/////////////////0    1     2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25

int currentSegment = 0;
String currentMenu = "menu";
int currentDifficultyIndex = 0;
int currentSoundIndex = 0;
bool displayChanged = false;
int segmentValue[4] =
{
  0, 0, 0, 0
};
char currentName[8] =
{
  'a', 'a', 'a', 'a', 'b', 'a', 'c', 'k'
};

const int brightnessLCDAddress = 0;
const int contrastLCDAddress = 1;
const int brightnessMatrixAddress = 2;
const int highscoresAddress[3] = {
  3, 5, 7
};
const int nameAddress[3] = {
  9, 13, 17
};
int highscores[3] = {
  0, 0, 0
};
const int highscoresNumber = 3;
const int nameLength = 4;
const int nameCharacters = 12;
char names[12] = {
  'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'
};
bool updatedHighscore = 0;

int currentBrightnessLCD = 128;
const int minBrightness = 0;
const int maxBrightness = 255;
const int decreaseBrightness = 10;
const int debounceInterval = 50;
int currentContrastLCD = 50;
const int minContrast = 0;
const int maxContrast = 255;
const int decreaseContrast = 10;

//game values
byte matrixBrightness = 2;
const int minMatrixBrightness = 0;
const int maxMatrixBrightness = 15;

byte xPos = 3;
byte yPos = 4;
byte xLastPos = 0;
byte yLastPos = 0;

byte xCurrentPos = 0;
byte yCurrentPos = 0;

byte xLastFoodPos = 0;
byte yLastFoodPos = 0;
byte xNewFoodPos = 0;
byte yNewFoodPos = 0;

byte xLastTrapPos = 0;
byte yLastTrapPos = 0;
byte xNewTrapPos = 0;
byte yNewTrapPos = 0;

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
const int minScore = 0;
const int chr = 'a';

int score = 0;
int highscore = 0;
const int segmentsCount = 4;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

void setup()
{
  // set up the LCD's number of columns and rows:

  initialize();

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

void loop()
{

  if (currentMenu == "menu")
  {
    displayMenu();
  }
  else if (currentMenu == "about")
  {
    displayAbout();
  }
  else if (currentMenu == "highscores")
  {
    displayHighscores();
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
  else if (currentMenu == "MatrixBrightness")
  {
    displayBrightnessMatrix();
  }
  else if (currentMenu == "startingLevel")
  {
    displaystartingLevel();
  }
  else if (currentMenu == "setSound")
  {
    displaySoundState();
  }
  else if (currentMenu == "newGame")
  {
    newGame();
  }
  else if (currentMenu == "game")
  {
    game();
  }
  else if (currentMenu == "setName")
  {
    interruptBlink();
  }
  else if (currentMenu == "changeName")
  {
    setName();
  }
  else if (currentMenu == "resetHighscore")
  {
    displayResetHighscore();
  }

}

void initialize()
{
  currentBrightnessLCD = EEPROM.read(brightnessLCDAddress);
  //currentContrastLCD=EEPROM.read(contrastLCDAddress);
  matrixBrightness = EEPROM.read(brightnessMatrixAddress);
  for (int i = 0; i < highscoresNumber; i++)
  {
    highscores[i] = read2BytesEEPROM(highscoresAddress[i]);
  }
  for (int j = 0; j < nameCharacters; j++)
  {
    names[j] = (char) EEPROM.read(nameAddress[0] + j);
  }

}

void write2BytesEEPROM(int address, int number)
{
  byte byte1 = (number >> 8) & 0xFF;
  byte byte2 = number & 0xFF;
  EEPROM.update(address, byte1);
  EEPROM.update(address + 1, byte2);
}

int read2BytesEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  int result = (byte1 << 8) + byte2;
  return result;
}

void writeNameEEPROM(int address, char currentName[])
{
  for (int i = 0; i < nameLength; i++)
  {
    EEPROM.update(address + i, currentName[i]);
  }
}

void updateHighscores(int score)
{
  if (score > highscores[0])
  {
    highscores[2] = highscores[1];
    highscores[1] = highscores[0];
    highscores[0] = score;
    for (int i = 4; i < nameCharacters; i++)
    {
      names[i] = names[i - 4];
    }
    for (int i = 0; i < highscoresNumber; i++)
    {
      write2BytesEEPROM(highscoresAddress[i], highscores[i]);
    }
    writeNameEEPROM(nameAddress[0], currentName);
    updatedHighscore = 1;
  }
  else
  {
    if (score > highscores[1])
    {
      highscores[2] = highscores[1];
      highscores[1] = score;
      for (int i = 8; i < nameCharacters; i++)
      {
        names[i] = names[i - 4];
      }
      for (int i = 0; i < highscoresNumber; i++)
      {
        write2BytesEEPROM(highscoresAddress[i], highscores[i]);
      }
      writeNameEEPROM(nameAddress[1], currentName);
      updatedHighscore = 1;
    }
    else
    {
      if (score > highscores[2])
      {
        highscores[2] = score;
        for (int i = 0; i < highscoresNumber; i++)
        {
          write2BytesEEPROM(highscoresAddress[i], highscores[i]);
        }
        writeNameEEPROM(nameAddress[2], currentName);
        updatedHighscore = 1;
      }
    }
  }
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
    if (currentSoundIndex == 0)
    {
      tone(buzzerPin, SCROLL_NOTE, 100);
    }
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

void displayHighscores()
{
  currentMenu = "highscores";
  xValue = analogRead(pinX);

  if (xValue < minThreshold && joyMoved == false)
  {
    if (indexHighscore > 0) {
      indexHighscore--;
    } else {
      indexHighscore = 3;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false)
  {
    if (indexHighscore < 3) {
      indexHighscore++;
    } else {
      indexHighscore = 0;
    }
    joyMoved = true;
  }

  if (joyMoved == true)
  {
    if (currentSoundIndex == 0)
    {
      tone(buzzerPin, SCROLL_NOTE, 100);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    if (indexHighscore < 3)
    {
      for (int i = nameLength * indexHighscore; i < nameLength * indexHighscore + nameLength; i++)
      {
        lcd.print(names[i]);
      }
      lcd.print(":");
      lcd.print(highscores[indexHighscore]);
      Serial.println("here1");
    }
    else
    {
      lcd.print("Back");
      Serial.println("here2");
    }
    lcd.setCursor(0, 1);
    //lcd.clear();
    if (indexHighscore < 2)
    {
      for (int i = nameLength * (indexHighscore + 1); i < nameLength * (indexHighscore + 1) + nameLength; i++)
      {
        lcd.print((char)names[i]);
      }
      lcd.print(":");
      lcd.print(highscores[indexHighscore + 1]);
    }
    else
    {
      if (indexHighscore == 2)
      {
        lcd.print("Back");
      }
      else
      {
        for (int i = nameLength * 0; i < 0 + nameLength; i++)
        {
          lcd.print((char)names[i]);
        }
        lcd.print(":");
        lcd.print(highscores[0]);
      }

    }
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
    if (currentSoundIndex==0)
{
  tone(buzzerPin, SCROLL_NOTE, 100);
} 
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
      indexSettings = 8;
    }
    joyMoved = true;

  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (indexSettings < 8) {
      indexSettings++;
    } else {
      indexSettings = 0;
    }
    joyMoved = true;

  }

  if (joyMoved == true)
  {
    if (currentSoundIndex==0)
    {
      tone(buzzerPin, SCROLL_NOTE, 100);
    }   
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(settingsItems[indexSettings]);
    lcd.setCursor(0, 1);
    //lcd.clear();
    if (indexSettings < 8)
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
    if (currentSoundIndex==0)
    {
      tone(buzzerPin, SCROLL_NOTE, 100);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(difficultyItems[currentDifficultyIndex]);
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

void displaySoundState()
{
  currentMenu = "setSound";
  xValue = analogRead(pinX);

  if (xValue < minThreshold && joyMoved == false)
  {
    if (currentSoundIndex > 0) {
      currentSoundIndex--;
    } else {
      currentSoundIndex = 1;
    }
    joyMoved = true;
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (currentSoundIndex < 1) {
      currentSoundIndex++;
    } else {
      currentSoundIndex = 0;
    }
    joyMoved = true;
  }

  if (joyMoved == true)
  {
    if (currentSoundIndex==0)
    {
      tone(buzzerPin, SCROLL_NOTE, 100);
    }    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(soundItems[currentSoundIndex]);
  }

  if (xValue >= minThreshold && xValue <= maxThreshold) {
    joyMoved = false;
  }
}

void pressButton()
{
  if (currentSoundIndex==0)
  {
     tone(buzzerPin, BUTTON_NOTE, 50);
  }
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
    else if (index == 1)
    {
      lcd.clear();
      displayHighscores();
    }
    else if (index == 0)
    {
      lcd.clear();
      newGame();
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
      if (currentMenu == "highscores")
      {
        lcd.clear();
        displayMenu();
      }
      else
      {
        if (currentMenu == "settings")
        {
          if (indexSettings == 7)
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
                else
                {
                  if (indexSettings == 4)
                  {
                    lcd.clear();
                    setName();
                  }
                  else
                  {
                    if (indexSettings == 5)
                    {
                      lcd.clear();
                      displayResetHighscore();
                    }
                    else
                    {
                      if (indexSettings == 6)
                      {
                        lcd.clear();
                        displaySoundState();
                      }
                    }
                  }
                }
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
            else if (currentMenu == "setSound")
            {
              lcd.clear();
              displaySettings();
            }

            else
            {
              if (currentMenu == "setName")
              {
                if (currentSegment < 4)
                {
                  lcd.clear();
                  setName();
                }
                else
                {
                  lcd.clear();
                  displaySettings();
                }
              }
            }
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
    if (currentSoundIndex==0)
    {
      tone(buzzerPin, SCROLL_NOTE, 100);
    }
    if (EEPROM.read(brightnessLCDAddress) != currentBrightnessLCD)
    {
      EEPROM.update(brightnessLCDAddress, currentBrightnessLCD);
    }
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

void setName()
{
  currentMenu = "setName";
  showDigits();
  if (displayChanged)
  {
    changeNumber(currentSegment);
  }
  else
  {
    changeDisplay();
  }
}

void interruptBlink()
{
  static unsigned long lastDebounceTime = 0;
  currentMenu = "changeName";
  if (millis() - lastDebounceTime > debounceInterval)
  {
    displayChanged = !displayChanged;
    lastDebounceTime = millis();
  }

  setName();
}

void showDigit(int displayNumber)
{

  //delay(5);

  // lcd.begin(16,2);
  for (int i = 0; i < segmentsCount; i++)
  {
    lcd.setCursor(i, 0);
    lcd.print(currentName[i]);
  }


}

void showDigits()
{
  for (int i = 0; i < segmentsCount; i++)
  {
    lcd.setCursor(i, 0);
    lcd.print(alphabet[segmentValue[i]]);
  }
  for (int i = 4; i < 8; i++)
  {
    Serial.println("here");
    lcd.setCursor(i - 4, 1);
    lcd.print(currentName[i]);
  }
  if (currentSegment < 4)
  {
    lcd.setCursor(currentSegment, 0);
    lcd.blink();
  }
  else
  {
    lcd.setCursor(currentSegment - 4, 1);
    lcd.blink();
  }

}

void changeDisplay()
{
  yValue = analogRead(pinY);

  if (yValue > maxThreshold && joyMoved == false)
  {
    if (currentSegment > 0)
    {
      currentSegment--;
    }
    else
    {
      currentSegment = 4;
    }
    joyMoved = true;
  }

  if (yValue < minThreshold && joyMoved == false)
  {
    if (currentSegment < 4)
    {
      currentSegment++;
    }
    else
    {
      currentSegment = 0;
    }
    joyMoved = true;
  }
  lcd.setCursor(0, currentSegment);
  lcd.blink();

  if (yValue >= minThreshold && yValue <= maxThreshold)
  {
    joyMoved = false;

  }

}

void changeNumber(int currentSegment)
{
  xValue = analogRead(pinX);
  if (currentSegment < 4)
  {
    if (xValue < minThreshold && joyMoved == false)
    {
      if (segmentValue[currentSegment] > 0)
      {
        segmentValue[currentSegment]--;
      }
      else
      {
        segmentValue[currentSegment] = 25;
      }
      currentName[currentSegment] = alphabet[segmentValue[currentSegment]];
      joyMoved = true;
    }

    if (xValue > maxThreshold && joyMoved == false)
    {
      if (segmentValue[currentSegment] < 25)
      {
        segmentValue[currentSegment]++;
      }
      else
      {
        segmentValue[currentSegment] = 0;
      }
      currentName[currentSegment] = alphabet[segmentValue[currentSegment]];
      joyMoved = true;
    }

    if (xValue >= minThreshold && xValue <= maxThreshold)
    {
      joyMoved = false;
    }
  }
}

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
    if (currentSoundIndex==0)
    {
      tone(buzzerPin, SCROLL_NOTE, 100);
    } 
    if (EEPROM.read(brightnessMatrixAddress) != matrixBrightness)
    {
      EEPROM.update(brightnessMatrixAddress, matrixBrightness);
    }
    lc.setIntensity(0, matrixBrightness);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current brightness:");
    lcd.setCursor(6, 1);
    lcd.print(matrixBrightness);
    turnOnMatrix();
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
//if (currentSoundIndex==0)
//{
//  tone(buzzerPin, SCROLL_NOTE, 100);
//}
//    
//     if (EEPROM.read(contrastLCDAddress) != currentContrastLCD)
//    {
//       EEPROM.update(contrastLCDAddress, currentContrastLCD);
//    }
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

void displayResetHighscore()
{
  currentMenu = ("resetHighscore");
  highscores[2] = minScore;
  highscores[1] = minScore;
  highscores[0] = minScore;
  for (int i = minScore; i < nameCharacters; i++)
  {
    names[i] = chr;
  }
  for (int i = 0; i < nameLength; i++)
  {
    currentName[i] = chr;
  }
  for (int i = 0; i < highscoresNumber; i++)
  {
    write2BytesEEPROM(highscoresAddress[i], highscores[i]);
    writeNameEEPROM(nameAddress[i], currentName);
  }
  if (currentSoundIndex==0)
  {
    tone(buzzerPin, buzzerTone, 500);
  }
  lcd.setCursor(0, 0);
  lcd.print("Reseting highscores");
  delay(5000);
  lcd.clear();
  lcd.print("Reset commplete");
  delay(5000);
  displayMenu();

}

void newGame()
{
  lcd.setCursor(0, 0);
  lcd.print("Okaaay, let's go!");
  currentMenu = "newGame";
  xPos = 3;
  yPos = 4;
  score = 0;
  matrix[xPos][yPos] = 1;
  generateFood();
  game();

}
void game()
{
  currentMenu = "game";
  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print("Okaaay, let's go!");
  //lcd.print(score);
  if (millis() - lastMoved > moveInterval) {
    // game logic
    updatePositions();
    lastMoved = millis();
  }
  if (matrixChanged == true) {
    // matrix display logic
    updateMatrix();
    if (xPos == xNewFoodPos && yPos == yNewFoodPos)
    {
      if (currentSoundIndex == 0)
      {
        tone(buzzerPin, EAT_NOTE, 75);
      }
      generateFood();
    }

    if (score > 30)
      if (xPos != xNewTrapPos && yPos != yNewTrapPos)
        generateTrap();
    matrixChanged = false;
    lcd.clear();
    for (int i = 0; i < 4; i++)
    {
      lcd.print(currentName[i]);
    }
    lcd.print(": ");
    lcd.print(score);
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

void turnOffMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      matrix[row][col] = 0;
    }
  }
  updateMatrix();
}

void turnOnMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      matrix[row][col] = 1;
    }
  }
  updateMatrix();
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

void generateTrap() {
  xLastTrapPos = xCurrentPos;
  yLastTrapPos = yCurrentPos;
  xNewTrapPos = random(0, 8);
  yNewTrapPos = random(0, 8);

  matrix[xLastTrapPos][yLastTrapPos] = 0;
  if ( matrix[xLastTrapPos][yLastTrapPos + 1] == 1)
    matrix[xLastTrapPos][yLastTrapPos + 1] = 0;
  else if ( matrix[xLastTrapPos][yLastTrapPos - 1] == 1)
    matrix[xLastTrapPos][yLastTrapPos - 1] = 0;

  matrix[xNewTrapPos][yNewTrapPos] = 1;
  if (yNewTrapPos < matrixSize - 1)
    matrix[xNewTrapPos][yNewTrapPos + 1] = 1;
  else
    matrix[xNewTrapPos][yNewTrapPos - 1] = 1;
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

    if (matrix[xPos][yPos] == 1)
      score += 3;

    if ((matrix[xPos][yPos] == 1 && matrix[xPos][yPos + 1] == 1) || (matrix[xPos][yPos] == 1 && matrix[xPos][yPos + 1] == 1))
      dead();

    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;

  }

}

void dead()
{
  currentMenu = "dead";
  lcd.clear();
  turnOffMatrix();
  updateHighscores(score);
  if (updatedHighscore == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You reached the");
    lcd.setCursor(7, 1);
    lcd.print("top 3");
    updatedHighscore = 0;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You hit your");
  lcd.setCursor(0, 1);
  lcd.print("head");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("And died");
  lcd.setCursor(0, 1);
  lcd.print("Rest in peace");
  delay(5000);
  lcd.clear();
  lcd.print("Your score is:");
  lcd.setCursor(0, 1);
  lcd.print(score);
  delay(5000);
  lcd.clear();
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
