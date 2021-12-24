/*This is a new approach on the classical snake game.
  The user can play in 3 different ways with 3 different difficulties(easy, medium and hard). The difficulty can be selected from menu, but it also increases along wiith the score (0-30-> easy, 30-60-> medium and >60-> hard).
  When starting the game, the brightness for the LCD and for the matrix are set based on the the values saved in EEPROM. Also, the names and the highscores are read from EEPROM.
  In loop function it is checked what the current menu is and the current menu and it is diaplayed.
  The newgame function initialises the values for the game, then goes to the function game, that is the way the game is played. If the difficulty is easy, the user gains points by moving through the matrix without reaching the walls and extra points for eating the food. If the strating level is set to medium or the score is higher then 30, there are some traps that move in a 10 seconds interval. The game ends if the snake bumps into one trap. If the game is hard, the food also moves in 7 secodds and the traps in 5 seconds.
  When game ends, the function dead is called and the end message is displyed.
  The function pressButton verifies the position in which the user is and based on the currentMenu and the index and then displayes the right menu.
  The functions displayMenu, displayAbout, displayHowToPlay, diaplayHighscores, displaySettings, displayBrightnessLCD, displayBrightnessMatrix, displayStartingLevel and displaySoundState are used to show the desired menu.
  In function setName, the user can set the 4 name he wants to use.
  In function resetHighscore, the highscores and names are reset to 0 and "aaaa".
  The function setSoundState set the sound to on or off.
  Functions displayBrightnessLCD, displayBrightnessMatrix are used to set the brightness for the LCD and the matrix.
  Function displayAbout shows information about the game and the dunction displayHowToPlay shows a quick tutorial about the levels of the game.*/

#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "LedControl.h"

//declare the notes used for basic sounds
#define SCROLL_NOTE 10
#define BUTTON_NOTE 5000
#define EAT_NOTE 2500
#define DEAD_NOTE 100

//declare buzzer pin
#define BUZZERPIN 5
#define BUZZERTONE 1000

//declare matrix pins
#define DINPIN 12
#define CLOCKPIN 11
#define LOADPIN 10

//declare diaplay pins
#define RS 13
#define ENABLE 6
#define D4 7
#define D5 4
#define D6 3
#define D7 8
#define PINBRIGHTNESSLCD 9
//const int pinContrastLCD = 5;

//decalre joystick pins
#define PINSW  2
#define PINX  A0
#define PINY  A1

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

byte index = 0;
byte indexAbout = 0;
byte indexSettings = 0;
byte indexAlphabet = 0;
byte indexHighscore = 0;
byte indexHTP = 0;

String menuItems[] = {"> New Game", "> Highscores", "> Settings", "> About", "> How to play"};
String aboutItems[] = {"   Snake Game", "~ by Bianca Chiricu", "~ https://github.com/anabiancachiricu/MatrixSnakegame", "> Back to menu"};
String settingsItems[] = {"~ Starting level", "~ LCD Contrast", "~ LCD Brightness",  "~ Matrix Brightness", "~ Set name", "~ Reset Highscore", "~ Set Sound", "> Back to menu"};
String difficultyItems[] = {"Easy", "Medium", "Hard"};
String soundItems[] = {"On", "Off"};
char alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

byte currentSegment = 0;
String currentMenu = "menu";
byte currentDifficultyIndex = 0;
byte currentSoundIndex = 0;
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

const int highscoresAddress[3] =
{
  3, 5, 7
};
const int nameAddress[3] =
{
  9, 13, 17
};
int highscores[3] =
{
  0, 0, 0
};

const int highscoresNumber = 3;
const int nameLength = 4;
const int nameCharacters = 12;

char names[12] =
{
  'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'
};

bool updatedHighscore = 0;
byte currentBrightnessLCD = 128;

const byte minBrightness = 0;
const byte maxBrightness = 255;
const byte decreaseBrightness = 10;
const byte debounceInterval = 50;

byte currentContrastLCD = 50;
const byte minContrast = 0;
const byte maxContrast = 255;
const byte decreaseContrast = 10;

//game values
byte matrixBrightness = 2;
const byte minMatrixBrightness = 0;
const byte maxMatrixBrightness = 15;

byte xPos = 3;
byte yPos = 4;
byte xLastPos = 0;
byte yLastPos = 0;

byte xCurrentPos = 0;
byte yCurrentPos = 0;
byte xxCurrentPos = 0;
byte yyCurrentPos = 0;

byte xLastFoodPos = 0;
byte yLastFoodPos = 0;
byte xNewFoodPos = 0;
byte yNewFoodPos = 0;

byte xLastTrapPos = 0;
byte xxLastTrapPos = 0;
byte yLastTrapPos = 0;
byte xNewTrapPos = 0;
byte xxNewTrapPos = 0;
byte yNewTrapPos = 0;

const byte moveInterval = 100;
unsigned long long lastMoved = 0;

const byte htpInterval = 100;
unsigned long long htpMoved = 0;

const int trapIntervalMedium = 10000;
unsigned long long lastTrapMedium = 0;

const int trapIntervalHard = 5000;
unsigned long long lastTrapHard = 0;

const int foodIntervalHard = 7000;
unsigned long long lastFoodHard = 0;

const byte matrixSize = 8;
bool matrixChanged = true;

const int interruptCycle = 200;
volatile unsigned long long lastInterrupt = 0;

bool matrix[matrixSize][matrixSize] =
{
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

bool trapsMatrix[matrixSize][matrixSize] =
{
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

byte matrixByte[matrixSize] =
{
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
const char chr = 'a';

int score = 0;
int highscore = 0;
const byte segmentsCount = 4;

LiquidCrystal lcd(RS, ENABLE, D4, D5, D6, D7);
LedControl lc = LedControl(DINPIN, CLOCKPIN, LOADPIN, 1);

void setup()
{
  // set up the LCD's number of columns and rows:
  initialize();
  pinMode(PINX, INPUT);
  pinMode(PINY, INPUT);
  pinMode(PINSW, INPUT_PULLUP);
  pinMode(PINBRIGHTNESSLCD, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PINSW), handleInterrupt, FALLING);
  analogWrite(PINBRIGHTNESSLCD, currentBrightnessLCD);
  //analogWrite(pinContrastLCD, currentContrastLCD);
  lcd.begin(16, 2);
  intro();
  lcd.print("move joystick");
  displayMenu();
  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  matrix[xPos][yPos] = 1;
}

//verify which menu to display
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
  else if (currentMenu == "howtoplay")
  {
    displayHowToPlay();
    displayMenu();
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

//initialize the brightness for lcd and matrix, the contrast for LCD and the highscores and names with values saved in EEPROM
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

//function to save an int(2 bytes) in EEPROM
void write2BytesEEPROM(int address, int number)
{
  byte byte1 = (number >> 8) & 0xFF;
  byte byte2 = number & 0xFF;
  EEPROM.update(address, byte1);
  EEPROM.update(address + 1, byte2);
}

//function to read an int from EEPROM
int read2BytesEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  int result = (byte1 << 8) + byte2;
  return result;
}

//function to save name(4 chars) in EEPROM
void writeNameEEPROM(int address, char currentName[])
{
  for (int i = 0; i < nameLength; i++)
  {
    EEPROM.update(address + i, currentName[i]);
  }
}

//function to update the highscores
//verify if the current score is bigger than one of the top 3 and update it on the right position
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
    for (int i = 0; i < nameLength; i++)
    {
      names[i] = currentName[i];
    }
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
      for (int i = 4; i < 2 * nameLength; i++)
      {
        names[i] = currentName[i-4];
      }
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
        for (int i = 8; i < 3 * nameLength; i++)
        {
          names[i] = currentName[i-8];
        }
        updatedHighscore = 1;
      }
    }
  }
}

//introduction message to display when the game is started (or reset)
void intro()
{
  lcd.begin(16, 2);
  lcd.print(F("Hello, snake!"));
  delay(5000);
  lcd.clear();
  lcd.print(F("Did you miss me?"));
  lcd.setCursor(5, 1);
  lcd.print(F("ssssss"));
  delay(5000);
  lcd.clear();
  lcd.print(F("I'm small"));
  lcd.setCursor(0, 1);
  lcd.print(F("and hungry."));
  delay(5000);
  lcd.clear();
  lcd.print(F("I need to eat"));
  lcd.setCursor(0, 1);
  lcd.print(F("so I can grow"));
  delay(5000);
  lcd.clear();
  lcd.print(F("But there are"));
  lcd.setCursor(0, 1);
  lcd.print(F("some traps"));
  delay(5000);
  lcd.clear();
  lcd.print(F("that I need to"));
  lcd.setCursor(0, 1);
  lcd.print(F("avoid to survive"));
  delay(5000);
  lcd.clear();
  lcd.print(F("Will you help me"));
  lcd.setCursor(7, 1);
  lcd.print(F("?"));
  delay(5000);
  lcd.clear();
}

//function to display the menu
//function to navigate through menu
void displayMenu()
{
  //set the currentMenu to know where the user is
  currentMenu = "menu";
  turnOffMatrix();

  xValue = analogRead(PINX);
  //navigate though menu
  if (xValue < minThreshold && joyMoved == false)
  {
    if (index > 0) {
      index--;
    } else {
      index = 4;
    }
    joyMoved = true;
  }
  if (xValue > maxThreshold && joyMoved == false)
  {
    if (index < 4)
    {
      index++;
    }
    else
    {
      index = 0;
    }
    joyMoved = true;
  }
  if (joyMoved == true)
  {
    //play sound when navigating through menu
    if (currentSoundIndex == 0)
    {
      tone(BUZZERPIN, SCROLL_NOTE, 100);
    }
    //display the menu on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(menuItems[index]);
    lcd.setCursor(0, 1);
    //lcd.clear();
    if (index < 4)
      lcd.print(menuItems[index + 1]);
    else
      lcd.print(menuItems[0]);
    //blink on the current position
    lcd.setCursor(0, 0);
    lcd.blink();
  }
  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//function to display the highsocres
//function to navigate through highscores
void displayHighscores()
{
  //set the currentMenu to know where the user is
  currentMenu = "highscores";
  //navigate through highscores
  xValue = analogRead(PINX);
  if (xValue < minThreshold && joyMoved == false)
  {
    if (indexHighscore > 0)
    {
      indexHighscore--;
    }
    else
    {
      indexHighscore = 3;
    }
    joyMoved = true;
  }
  if (xValue > maxThreshold && joyMoved == false)
  {
    if (indexHighscore < 3)
    {
      indexHighscore++;
    }
    else
    {
      indexHighscore = 0;
    }
    joyMoved = true;
  }
  if (joyMoved == true)
  {
    //play sound when navigatiing through highscores
    if (currentSoundIndex == 0)
    {
      tone(BUZZERPIN, SCROLL_NOTE, 100);
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
    }
    else
    {
      lcd.print(F("Back"));
    }
    lcd.setCursor(0, 1);
    //diisplay the first 3 highscores and the back button (to go back to the menu)
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
        lcd.print(F("Back"));
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
    //blink on the current position
    lcd.setCursor(0, 0);
    lcd.blink();
  }
  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//about menu
//function to display the about menu
//function to navigate through the about menu
void displayAbout()
{
  //set the currentMenu to know where the user is
  currentMenu = "about";
  //navigate through about menu
  xValue = analogRead(PINX);
  if (xValue < minThreshold && joyMoved == false)
  {
    if (indexAbout > 0)
    {
      indexAbout--;
    }
    else
    {
      indexAbout = 3;
    }
    joyMoved = true;
  }
  if (xValue > maxThreshold && joyMoved == false)
  {
    if (indexAbout < 3)
    {
      indexAbout++;
    }
    else
    {
      indexAbout = 0;
    }
    joyMoved = true;
  }
  if (joyMoved == true)
  {
    //play sound when navigating through about menu
    if (currentSoundIndex == 0)
    {
      tone(BUZZERPIN, SCROLL_NOTE, 100);
    }
    //display the about menu and the back button
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(aboutItems[indexAbout]);
    lcd.setCursor(0, 1);
    //lcd.clear();
    if (indexAbout < 3)
    {
      lcd.print(aboutItems[indexAbout + 1]);
    }
    else
    {
      lcd.print(aboutItems[0]);
    }
    lcd.setCursor(0, 0);
    lcd.blink();
  }
  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//function to display one short tutorial about the game levels and how to play
void displayHowToPlay()
{
  //set the currentMenu to know where the user is
  currentMenu = "howtoplay";
  lcd.setCursor(0, 0);
  lcd.print(F("EASY:"));
  lcd.setCursor(0, 1);
  lcd.print(F("Go after food"));
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("To gain points"));
  lcd.setCursor(0, 1);
  lcd.print(F("MEDIUM:"));
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("There are traps"));
  lcd.setCursor(0, 1);
  lcd.print(F("to avoid"));
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("HARD:"));
  lcd.setCursor(0, 1);
  lcd.print(F("The food"));
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("dissappears"));
  lcd.setCursor(0, 1);
  lcd.print(F("after 7 sec"));
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("GOOD LUCK"));
  delay(3000);
  lcd.clear();
}

//settings menu
//function to display the settings menu
//function to navigate through the settings menu
void displaySettings()
{
  //set the currentMenu to know where the user is
  currentMenu = "settings";
  //navigate through settings
  xValue = analogRead(PINX);
  if (xValue < minThreshold && joyMoved == false)
  {
    if (indexSettings > 0)
    {
      indexSettings--;
    }
    else
    {
      indexSettings = 8;
    }
    joyMoved = true;
  }
  if (xValue > maxThreshold && joyMoved == false)
  {
    if (indexSettings < 8)
    {
      indexSettings++;
    }
    else
    {
      indexSettings = 0;
    }
    joyMoved = true;
  }
  if (joyMoved == true)
  {
    //play sound when navigating through settings
    if (currentSoundIndex == 0)
    {
      tone(BUZZERPIN, SCROLL_NOTE, 100);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(settingsItems[indexSettings]);
    lcd.setCursor(0, 1);
    //lcd.clear();
    if (indexSettings < 8)
    {
      lcd.print(settingsItems[indexSettings + 1]);
    }
    else
    {
      lcd.print(settingsItems[0]);
    }
    //blink on the current position
    lcd.setCursor(0, 0);
    lcd.blink();
  }
  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//starting level (difficulty) menu
//function to display the difficulty menu
//function to navigate through the difficulty menu
void displaystartingLevel()
{
  //set the currentMenu to know where the user is
  currentMenu = "startingLevel";
  //navigate to selecct the starting level
  xValue = analogRead(PINX);
  if (xValue < minThreshold && joyMoved == false)
  {
    if (currentDifficultyIndex > 0)
    {
      currentDifficultyIndex--;
    }
    else
    {
      currentDifficultyIndex = 2;
    }
    joyMoved = true;
  }
  if (xValue > maxThreshold && joyMoved == false)
  {
    if (currentDifficultyIndex < 2)
    {
      currentDifficultyIndex++;
    }
    else
    {
      currentDifficultyIndex = 0;
    }
    joyMoved = true;
  }
  if (joyMoved == true)
  {
    //play sound when navigating though difficulty menu
    if (currentSoundIndex == 0)
    {
      tone(BUZZERPIN, SCROLL_NOTE, 100);
    }
    //display only the current difficulty in order to select it and exit to the settings
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(difficultyItems[currentDifficultyIndex]);
  }
  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//menu to select the sond (on or off)
//function to display the sound menu
//function to navigate through the sound menu
void displaySoundState()
{
  //set the currentMenu to know where the user is
  currentMenu = "setSound";
  //navigate to select if the sun is turned on or off
  xValue = analogRead(PINX);
  if (xValue < minThreshold && joyMoved == false)
  {
    if (currentSoundIndex > 0)
    {
      currentSoundIndex--;
    }
    else
    {
      currentSoundIndex = 1;
    }
    joyMoved = true;
  }
  if (xValue > maxThreshold && joyMoved == false)
  {
    if (currentSoundIndex < 1)
    {
      currentSoundIndex++;
    }
    else
    {
      currentSoundIndex = 0;
    }
    joyMoved = true;
  }
  if (joyMoved == true)
  {
    //play sound when navigating through the sound menu
    if (currentSoundIndex == 0)
    {
      tone(BUZZERPIN, SCROLL_NOTE, 100);
    }
    //display the current sound state
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.blink();
    lcd.print(soundItems[currentSoundIndex]);
  }
  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//function to check which menu to display when pressing the button
void pressButton()
{
  //if the sound is on play sound when pressing the button
  if (currentSoundIndex == 0)
  {
    tone(BUZZERPIN, BUTTON_NOTE, 50);
  }
  //verify where the user is (in which menu and on what position) when the button is pressed and display the right menu
  if (currentMenu == "menu")
  {
    if (index == 3)
    {
      lcd.clear();
      displayAbout();
    }
    else if (index == 4)
    {
      lcd.clear();
      displayHowToPlay();
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
                      displayMenu();
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
        else if (currentMenu == "howtoplay")
        {
          lcd.clear();
          displayMenu();
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

//function to set the brightness for the LCD display
void displayBrightnessLCD()
{
  //set the currentMenu to know where the user is
  currentMenu = "LCDBrightness";
  //set the brightness using the joystick
  xValue = analogRead(PINX);
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
    //if the sound is on, play a sound when selecting the brightness value
    if (currentSoundIndex == 0)
    {
      tone(BUZZERPIN, SCROLL_NOTE, 100);
    }
    //read from EEPROM the value of the brightness and update the value if changed
    if (EEPROM.read(brightnessLCDAddress) != currentBrightnessLCD)
    {
      EEPROM.update(brightnessLCDAddress, currentBrightnessLCD);
    }
    //set the brightness of the LCD and display the current value
    analogWrite(PINBRIGHTNESSLCD, currentBrightnessLCD);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Current brightness:"));
    lcd.setCursor(6, 1);
    lcd.print(currentBrightnessLCD);
  }
  if (xValue >= minThreshold && xValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//function for setting name
//the name can have 4 chars and it is set using the joystick
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
  for (int i = 0; i < segmentsCount; i++)
  {
    lcd.setCursor(i, 0);
    lcd.print(currentName[i]);
  }
}

//function used for showing the name and the back button
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

//function to set the current position in the name
void changeDisplay()
{
  yValue = analogRead(PINY);
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
  //blink on the current position
  lcd.setCursor(0, currentSegment);
  lcd.blink();
  if (yValue >= minThreshold && yValue <= maxThreshold)
  {
    joyMoved = false;
  }
}

//function to set the letter on the current position
void changeNumber(int currentSegment)
{
  //change the letter on the current position
  xValue = analogRead(PINX);
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

//function to set the brightness for the matrix
void displayBrightnessMatrix()
{
  //set the currentMenu to know where the user is
  currentMenu = "MatrixBrightness";
  //set the brightness using the joystick
  xValue = analogRead(PINX);
  if (xValue < minThreshold && joyMoved == false)
  {
    if (matrixBrightness  > minMatrixBrightness)
    {
      matrixBrightness --;
    }
    joyMoved = true;
  }
  if (xValue > maxThreshold && joyMoved == false)
  {
    if (matrixBrightness < maxMatrixBrightness)
    {
      matrixBrightness ++;
    }
    joyMoved = true;
  }
  if (joyMoved == true)
  {
    //if the sound is on, play a sound when selecting the brightness value
    if (currentSoundIndex == 0)
    {
      tone(BUZZERPIN, SCROLL_NOTE, 100);
    }
    //read from EEPROM the value of the brightness and update the value if changed
    if (EEPROM.read(brightnessMatrixAddress) != matrixBrightness)
    {
      EEPROM.update(brightnessMatrixAddress, matrixBrightness);
    }
    //set the brightness of the matrix and display the current value
    lc.setIntensity(0, matrixBrightness);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Current brightness:"));
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
//  xValue = analogRead(PINX);
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
//  tone(BUZZERPIN, SCROLL_NOTE, 100);
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

//function to reset the highscores and the names ( highscores to 0 and names to "aaaa")
void displayResetHighscore()
{
  //set the currentMenu to know where the user is
  currentMenu = "resetHighscore";
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
  //if the sound is set to on, play a soound when resetting the score
  if (currentSoundIndex == 0)
  {
    tone(BUZZERPIN, BUZZERTONE, 500);
  }
  //display a message when resetting the highscore
  lcd.setCursor(0, 0);
  lcd.print(F("Reseting highscores"));
  delay(3000);
  lcd.clear();
  lcd.print(F("Reset commplete"));
  delay(3000);
  //lcd.clear();
  //displayMenu();
}

//function to start the game
void newGame()
{
  //display a message when starting the game
  lcd.setCursor(0, 0);
  lcd.print(F("Okaaay, let's go!"));
  //set the currentMenu to know where the user is
  currentMenu = "newGame";
  //set the first position for the snake
  xPos = 3;
  yPos = 4;
  //verify the difficulty to select the starting score
  if (currentDifficultyIndex == 0)
  {
    score = 0;
  }
  else
  {
    if (currentDifficultyIndex == 1)
    {
      score = 30;
    }
    else
    {
      score = 60;
    }
  }
  matrix[xPos][yPos] = 1;
  generateFood();
  //continue the game
  game();
}

void game()
{
  //set the currentMenu to know where the user is
  currentMenu = "game";
  //update the positions
  if (millis() - lastMoved > moveInterval)
  {
    // game logic
    updatePositions();
    lastMoved = millis();
  }
  //verify if the matrix has changed and update it if true
  if (matrixChanged == true)
  {
    // matrix display logic
    updateMatrix();
    //verify if the snake position is the same as the food position
    //if true, eat it and generate another
    if (xPos == xNewFoodPos && yPos == yNewFoodPos)
    {
      //if sound on, play a sound when eatting
      if (currentSoundIndex == 0)
      {
        tone(BUZZERPIN, EAT_NOTE, 75);
      }
      generateFood();
    }
    //if the score is between 30 and 60, the difficulty is medium
    //move the trap at a 10 seconds interval
    if (score > 30 && score < 60)
    {
      if (millis() - lastTrapMedium > trapIntervalMedium)
      {
        generateTrap();
        lastTrapMedium = millis();
      }
    }
    else
    {
      //if the score is above 60, the difficulty is hard
      //move the trap at a 5 seconds interval
      //move the food at a 7 seconds interval
      if (score > 60)
      {
        if (millis() - lastTrapHard > trapIntervalHard)
        {
          generateTrap();
          lastTrapHard = millis();
        }
        if (millis() - lastFoodHard > foodIntervalHard)
        {
          generateFood();
          lastFoodHard = millis();
        }
      }
    }
    matrixChanged = false;
    //print the current name and the current score on the lcd
    lcd.clear();
    for (int i = 0; i < 4; i++)
    {
      lcd.print(currentName[i]);
    }
    lcd.print(": ");
    lcd.print(score);
  }
}

//function to update byte matrix
void updateByteMatrix()
{
  for (int row = 0; row < matrixSize; row++)
  {
    lc.setRow(0, row, matrixByte[row]);
  }
}

//function to update the matrix
void updateMatrix()
{
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

//function to turn off the whole matrix
void turnOffMatrix()
{
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      matrix[row][col] = 0;
    }
  }
  updateMatrix();
}

//function to turn on the whole matrix
void turnOnMatrix()
{
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      matrix[row][col] = 1;
    }
  }
  updateMatrix();
}

//function to generate food
void generateFood()
{
  //update the positions of the last  food generated
  xLastFoodPos = xNewFoodPos;
  yLastFoodPos = yNewFoodPos;
  //generate positions for the new food
  xNewFoodPos = random(1, 7);
  yNewFoodPos = random(1, 7);
  //turn off the last food and turn on the new one
  matrix[xLastFoodPos][yLastFoodPos] = 0;
  matrix[xNewFoodPos][yNewFoodPos] = 1;
  matrixChanged = true;
}

void generateTrap()
{
  //update the positions of the last  food generated
  xLastTrapPos = xNewTrapPos;
  yLastTrapPos = yNewTrapPos;
  xxLastTrapPos = xxNewTrapPos;
  //turn off the last trap
  matrix[xLastTrapPos][yLastTrapPos] = 0;
  matrix[xxLastTrapPos][yLastTrapPos] = 0;
  trapsMatrix[xLastTrapPos][yLastTrapPos] = 0;
  trapsMatrix[xxLastTrapPos][yLastTrapPos] = 0;
  //generate new trap positions
  xNewTrapPos = random(0, 8);
  yNewTrapPos = random(0, 8);
  //a trap is made up from 2 dots vertically placed
  //turn on the new trap
  matrix[xNewTrapPos][yNewTrapPos] = 1;
  //set the second dot position to be above or below the first one generated
  if (xNewTrapPos < matrixSize - 1)
  {
    xxNewTrapPos = xNewTrapPos + 1;
  }
  else
  {
    xxNewTrapPos = xNewTrapPos - 1;
  }
  //turn on the trap
  matrix[xxNewTrapPos][yNewTrapPos] = 1;
  trapsMatrix[xNewTrapPos][yNewTrapPos] = 1;
  trapsMatrix[xxNewTrapPos][yNewTrapPos] = 1;
  matrixChanged = true;
}

//function to update the snake position
void updatePositions()
{
  //move the snake(dot) trough the matrix using the joystick
  int xValue = analogRead(PINX);
  int yValue = analogRead(PINY);
  xLastPos = xPos;
  yLastPos = yPos;
  //update the positions and the score
  if (xValue < minThresholdGame)
  {
    if (xPos <= matrixSize - 1)
    {
      xPos--;
      score++;
    }
    else
    {
     // xPos = matrixSize - 1;
      dead();
    }
  }
  if (xValue > maxThreshold)
  {
    if (xPos >= 0)
    {
      xPos++;
      score++;
    }
    else
    {
      //xPos = 0;
      dead();
    }
  }
  if (yValue >= maxThreshold)
  {
    if (yPos < matrixSize - 1)
    {
      yPos--;
      score++;
    }
    else
    {
     // yPos = matrixSize - 1;
      dead();
    }
  }
  if (yValue < minThresholdGame)
  {
    if (yPos >= 0)
    {
      yPos++;
      score++;
    }
    else
    {
     // yPos = 0;
      dead();
    }
  }
  //veriify if the dot has moved and update the matrix and the score if so
  //check if the current position is a food or a trap
  //if it food, the score increases with 3
  //if it is a trap, the ggame ends
  if (xPos != xLastPos || yPos != yLastPos)
  {
    if (matrix[xPos][yPos] == 1 && trapsMatrix[xPos][yPos] == 0)
    {
      score += 3;
    }
    if (trapsMatrix[xPos][yPos] == 1 )
    {
      dead();
    }
    //turn off the last position and turn on the new one
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;
  }
}

//function to show the dead screen on lcd
void dead()
{
  //set the currentMenu to know where the user is
  currentMenu = "dead";
  //if the sound is on, play a sound when the game ends
  if (currentSoundIndex == 0)
  {
    tone(BUZZERPIN, DEAD_NOTE, 500);
  }
  lcd.clear();
  //update the highscores
  updateHighscores(score);
  //verify if the highscores modified, and if true display a message that the user reached the top
  if (updatedHighscore == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("You reached the"));
    lcd.setCursor(7, 1);
    lcd.print(F("top 3"));
    delay(3000);
    updatedHighscore = 0;
  }
  //display the ending message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("You hit your"));
  lcd.setCursor(0, 1);
  lcd.print(F("head"));
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("And died"));
  lcd.setCursor(0, 1);
  lcd.print(F("Rest in peace"));
  delay(3000);
  lcd.clear();
  lcd.print(F("Your score is:"));
  lcd.setCursor(0, 1);
  lcd.print(score);
  delay(3000);
  lcd.clear();
  //turn off the matrix and go back to menu
  turnOffMatrix();
  displayMenu();
  turnOffMatrix();
}

//function to scroll the text
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
      lcd.command(0x18);
    }
    delay(300);
    i++;
  }
  delay(5000);
  lcd.clear();
}

//function to handle the noise and to check if the button was pressed
void handleInterrupt()
{
  //verify if the button was pressed and if true go to pressButton function to display the right menu
  unsigned long long current = millis();
  if (current - lastInterrupt > interruptCycle)
  {
    lastInterrupt = current;
    pressButton();
  }
}
