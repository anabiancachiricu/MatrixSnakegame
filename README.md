# MatrixSnakeGame

A new approach for the basic snake game, created using the Arduino Uno board, a lcd display, a 8x8 led matrix and a joystick.

## Game description

At first, the score is calculated based on how many moves the user makes before hitting a wall. The difficulty increases, there will appear some obstacles that the snake should avoid and also some food, that determines bonus points.  

## How to play

Using the joystick, the player must collect the food in order to gain points. He should not hit the walls,, or the game will end. Based on the current score, the difficulty increases. After 30 points, traps appear and change the positions every 10 seconds. After reaching score 60, the food also changes its position at 7 seconds, and the traps move faster, at 5 seconds. The player should avoid the traps and the walls to survive.

## Components

- Arduino UNO Board
- 8x8 Matrix
- 16x2 LCD
- joystick
- buzzer

# Requirements:

## Menu:
The game has a main menu, that contains the following:
- New Game
- Highscores
- About
- Settings
- How to play
Each of them contains also a submenu.
The values for Brightness for LCD, for matrix, names and highscores are stored in EEPROM.

## Highscores
Displays the first 3 highscores and the names of the players that reached them. It also has a back button, to go to main menu. The values are stored in EEPROM.

## About
Displays information about the game:
- Title of the game
- Creator of the game
- Link to github of the game
- button to go bback

## How to play
Displays a shhort tutorial about the game levels

## Settings
Each option of the settings menu opens another menu.
- Starting level : the user can change the difficulty of the game(easy, medium, hard)
- LCD Contrast : the user can change the contrast of the LCD
- LCD Brightness : the user can change the brightness of the LCD
- Matrix Brightness : the user can change the brightness of the matrix
- Set Name : the user can set a 4 letters name
- Reset Highscore : the user can reset all highscores to 0 and all names to "aaaa"
- Set Sound : the user can set the sound to be on or off
- Back to menu 

# Demo:
https://youtu.be/9cXE3XVe6co

# Picture of the setup
![alt text](https://github.com/anabiancachiricu/MatrixSnakegame/blob/main/matrixSetup.jpeg)

