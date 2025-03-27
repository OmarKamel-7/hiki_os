// Nokia 5110 LCD Advanced Menu System with Navigation and Submenus + Enhanced UI
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "icons.h"

// LCD pins: CLK, DIN, DC, CE, RST
Adafruit_PCD8544 display = Adafruit_PCD8544(4, 5, 6, 7, 8);
#include <EEPROM.h>  // For high score saving

#define BUILTIN_LED A5
#define BUZZER_PIN A5  // Or any pin you have connected to a buzzer

// Pins
#define BUILTIN_LED A5
const int backlightPin = 9;
const int buttonLeft = 13;
const int buttonRight = 12;
const int buttonSelect = A0;
const int buttonBack = A1;
const int buttonUp = 10;
const int buttonDown = 11;
const int buttonBackspace = A2;

// Settings
int brightness = 128;
int contrast = 60;
bool backlightOn = true;
bool inverted = false;
bool rotated = true;
unsigned long timerDuration = 10;
unsigned long timerStartTime = 0;
bool timerRunning = false;
int timerMenuIndex = 0;

String password = "0001";
String inputPassword = "";
bool passwordVerified = false;
bool showHomeScreen = true;
unsigned long lastClockUpdate = 0;
int clockHour = 10;
int clockMinute = 0;
String username = "Omar";
String inputUsername = "";
static unsigned long lastPress = 0;
String userInput = "";

int topMainMenuIndex = 0;
int mainMenuIndex = 0;
const int visibleMainMenuItems = 3;
int selectedDifficulty = 1; // 0: Easy, 1: Medium, 2: Hard (Default: Medium)


// Menu Enums
enum MainMenuItem {
  MAIN_SETTINGS,
  MAIN_CALCULATOR,
  MAIN_TIMER,
  MAIN_USER_SETTINGS,
  MAIN_SNAKE,
  MAIN_GPS,
  MAIN_MENU_COUNT
};

enum SettingsItem {
  MENU_BRIGHTNESS,
  MENU_CONTRAST,
  MENU_BACKLIGHT,
  MENU_INVERT,
  MENU_ROTATION,
  MENU_RESET,
  MENU_COUNT
};

enum TimerMenuItem {
  TIMER_START,
  TIMER_SETTINGS,
  TIMER_BACK,
  TIMER_MENU_COUNT
};

int settingsMenuIndex = 0;
int topMenuIndex = 0;
const int visibleItems = 3;
bool inSettings = false;
bool inCalculator = false;
#define MAIN_SETTINGS      0
#define MAIN_CALCULATOR    1
#define MAIN_USER_SETTINGS 2
#define MAIN_TIMER         4
#define MAIN_SNAKE         5
#define MAIN_GPS           3

#define MAIN_MENU_COUNT 6

// Grid Navigation
const int GRID_ROWS = 2;
const int GRID_COLS = 3;
int selectedRow = 0;
int selectedCol = 0;

void setup() {
    // Prepare EEPROM for high score
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(backlightPin, OUTPUT);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);
  pinMode(buttonBack, INPUT_PULLUP);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(buttonBackspace, INPUT_PULLUP);
  randomSeed(analogRead(0));

  analogWrite(backlightPin, brightness);
  display.begin();
  display.setContrast(contrast);
  display.setRotation(rotated ? 2 : 0);
  display.clearDisplay();
  display.display();

  passwordMenu(); // Call your password screen
  drawMainMenu(selectedRow, selectedCol);
}


void loop() {
  // Update main menu index based on grid position
  mainMenuIndex = selectedRow * GRID_COLS + selectedCol;

  // Navigation and input (debounced)
  if (millis() - lastPress > 150) {
    // Navigation within Settings submenu
    if (inSettings) {
      if (digitalRead(buttonUp) == LOW) {
        settingsMenuIndex = (settingsMenuIndex - 1 + MENU_COUNT) % MENU_COUNT;
        if (settingsMenuIndex < topMenuIndex) {
          topMenuIndex = settingsMenuIndex;
        }
        drawSettingsMenu();
        lastPress = millis();
      }
      if (digitalRead(buttonDown) == LOW) {
        settingsMenuIndex = (settingsMenuIndex + 1) % MENU_COUNT;
        if (settingsMenuIndex >= topMenuIndex + visibleItems) {
          topMenuIndex = settingsMenuIndex - visibleItems + 1;
        }
        drawSettingsMenu();
        lastPress = millis();
      }

      if (digitalRead(buttonSelect) == LOW) {
        openSubMenu(settingsMenuIndex);
        drawSettingsMenu();  // Redraw settings menu after returning from submenu
        lastPress = millis();
      }

      if (digitalRead(buttonBack) == LOW) {
        inSettings = false;
        drawMainMenu(selectedRow, selectedCol);
        lastPress = millis();
      }

    } else {
      // Main menu navigation
      if (digitalRead(buttonUp) == LOW) {
        selectedRow = (selectedRow - 1 + GRID_ROWS) % GRID_ROWS;
        drawMainMenu(selectedRow, selectedCol);
        lastPress = millis();
      }
      if (digitalRead(buttonDown) == LOW) {
        selectedRow = (selectedRow + 1) % GRID_ROWS;
        drawMainMenu(selectedRow, selectedCol);
        lastPress = millis();
      }
      if (digitalRead(buttonLeft) == LOW) {
        selectedCol = (selectedCol - 1 + GRID_COLS) % GRID_COLS;
        drawMainMenu(selectedRow, selectedCol);
        lastPress = millis();
      }
      if (digitalRead(buttonRight) == LOW) {
        selectedCol = (selectedCol + 1) % GRID_COLS;
        drawMainMenu(selectedRow, selectedCol);
        lastPress = millis();
      }

      // Select item from main menu
      if (digitalRead(buttonSelect) == LOW) {
        switch (mainMenuIndex) {
          case MAIN_SETTINGS:
            inSettings = true;
            settingsMenuIndex = 0;
            topMenuIndex = 0;
            drawSettingsMenu();
            break;
          case MAIN_CALCULATOR:
            inCalculator = true;
            calculatorMenu();
            break;
          case MAIN_USER_SETTINGS:
            userSettingsMenu();
            break;
          case MAIN_TIMER:
            timerMenu();
            break;
          case MAIN_SNAKE:
            snakeMenu();
            break;

          case MAIN_GPS:
            gpsMenu();
            break;
        }
        lastPress = millis();
      }

      // Back to main menu from other sections
      if (digitalRead(buttonBack) == LOW) {
        inSettings = false;
        inCalculator = false;
        showHomeScreen = false;
        drawMainMenu(selectedRow, selectedCol);
        lastPress = millis();
      }
    }
  }

  // Calculator screen logic
  if (inCalculator) {
    calculatorMenu();
    if (digitalRead(buttonBackspace) == LOW && userInput.length() > 0) {
      userInput.remove(userInput.length() - 1);
      updateInputDisplay(userInput);
      delay(200);  // Simple debounce
    }
  }

  // Home screen clock update every second
  if (showHomeScreen && millis() - lastClockUpdate >= 1000) {
    lastClockUpdate = millis();
    clockMinute++;
    if (clockMinute >= 60) {
      clockMinute = 0;
      clockHour = (clockHour + 1) % 24;
    }
    drawHomeScreen();
  }
}
