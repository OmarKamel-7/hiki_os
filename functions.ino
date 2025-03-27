

String runKeyboardInput(String initial = "") {
  const char* keys[] = {
    "a", "b", "c", "d", "e", "f",
    "g", "h", "i", "j", "k", "l",
    "m", "n", "o", "p", "q", "r",
    "s", "t", "u", "v", "w", "x",
    "y", "z", "←", "_", "OK", "CL"
  };

  const int totalKeys = 30;
  const int cols = 6;
  const int rows = 5;
  int selected = 0;
  String inputText = initial;
  bool done = false;

  while (!done) {
    // Draw Keyboard
    display.clearDisplay();
    display.setTextSize(1);

    // Display input so far
    display.setCursor(0, 0);
    display.print(inputText);

    // Draw the keys
    for (int i = 0; i < totalKeys; i++) {
      int col = i % cols;
      int row = i / cols;
      int x = col * 14;
      int y = 10 + row * 8;

      if (i == selected) {
        display.fillRect(x - 1, y - 1, 12, 9, BLACK);
        display.setTextColor(WHITE, BLACK);
      } else {
        display.setTextColor(BLACK);
      }

      display.setCursor(x, y);
      display.print(keys[i]);
    }

    display.display();

    // Handle buttons
    if (digitalRead(buttonRight) == LOW) {
      if ((selected + 1) % cols != 0) selected++;
      delay(150);
    }
    if (digitalRead(buttonLeft) == LOW) {
      if (selected % cols != 0) selected--;
      delay(150);
    }
    if (digitalRead(buttonDown) == LOW) {
      if (selected + cols < totalKeys) selected += cols;
      delay(150);
    }
    if (digitalRead(buttonUp) == LOW) {
      if (selected - cols >= 0) selected -= cols;
      delay(150);
    }

    if (digitalRead(buttonSelect) == LOW) {
      const char* key = keys[selected];

      if (strcmp(key, "←") == 0) {
        if (inputText.length() > 0)
          inputText.remove(inputText.length() - 1);
      } else if (strcmp(key, "␣") == 0) {
        inputText += " ";
      } else if (strcmp(key, "OK") == 0) {
        done = true;
      } else if (strcmp(key, "CL") == 0) {
        inputText = "";
      } else {
        inputText += key;
      }
      delay(150);  // Debounce
    }

    if (digitalRead(buttonBackspace) == LOW) {
      if (inputText.length() > 0)
        inputText.remove(inputText.length() - 1);
      delay(150);
    }
  }

  return inputText;
}


void drawMainMenu(int selectedRow, int selectedCol) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);

  // Layout: settings, calculator, account, gps, timer, snake
  const int iconX[MAIN_MENU_COUNT] = {0, 26, 52, 0, 26, 52};
  const int iconY[MAIN_MENU_COUNT] = {0, 0, 0, 25, 25, 25};
  const int iconW[MAIN_MENU_COUNT] = {20, 25, 25, 20, 25, 20};
  const int iconH[MAIN_MENU_COUNT] = {20, 25, 25, 20, 25, 20};

  int index = selectedRow * GRID_COLS + selectedCol;

  for (int i = 0; i < MAIN_MENU_COUNT; i++) {
    // Highlight the selected icon
    if (i == index) {
      display.drawRect(iconX[i] - 2, iconY[i] - 2, iconW[i] + 4, iconH[i] + 4, BLACK);
    }

    // Draw icons based on new layout
    switch (i) {
      case 0: // Settings
        display.drawBitmap(iconX[i], iconY[i], epd_bitmap_settings_11zon__2_, 20, 20, BLACK);
        break;
      case 1: // Calculator
        display.drawBitmap(iconX[i], iconY[i], epd_bitmap_OIP__1__11zon__1_, 25, 25, BLACK);
        break;
      case 2: // Account
        display.drawBitmap(iconX[i], iconY[i], epd_bitmap_R_11zon__1_, 25, 25, BLACK);
        break;
      case 3: // GPS
        display.drawBitmap(iconX[i], iconY[i], epd_bitmap_gps_icon, 20, 20, BLACK);
        break;
      case 4: // Timer
        display.drawBitmap(iconX[i], iconY[i], epd_bitmap_OIP__2__11zon, 25, 25, BLACK);
        break;
      case 5: // Snake
        display.drawBitmap(iconX[i], iconY[i], epd_bitmap_snake_icon, 20, 20, BLACK);
        break;
    }
  }

  display.display();


}






void drawSettingsMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);

  for (int i = 0; i < visibleItems && (topMenuIndex + i) < MENU_COUNT; i++) {
    int idx = topMenuIndex + i;
    int y = i * 14;
    if (idx == settingsMenuIndex) {
      display.drawRect(0, y, 84, 14, BLACK);
    }
    display.setCursor(10, y + 3);
    switch (idx) {
      case MENU_BRIGHTNESS: display.print("Brightness"); break;
      case MENU_CONTRAST: display.print("Contrast"); break;
      case MENU_BACKLIGHT: display.print("Backlight"); break;
      case MENU_INVERT: display.print("Invert"); break;
      case MENU_ROTATION: display.print("Rotation"); break;
      case MENU_RESET: display.print("Reset"); break;
    }
  }

  int barHeight = map(visibleItems, 0, MENU_COUNT, 0, 42);
  int barY = map(topMenuIndex, 0, MENU_COUNT, 0, 42);
  display.drawRect(82, 0, 2, 42, BLACK);
  display.fillRect(82, barY, 2, barHeight, BLACK);

  display.display();
}

void openSubMenu(int index) {
  switch (index) {
    case MENU_BRIGHTNESS: adjustBrightness(); break;
    case MENU_CONTRAST: adjustContrast(); break;
    case MENU_BACKLIGHT: toggleBacklight(); break;
    case MENU_INVERT: toggleInvert(); break;
    case MENU_ROTATION: toggleRotation(); break;
    case MENU_RESET: resetSettings(); break;
  }
}


void adjustBrightness() {
  while (true) {
    display.setRotation(rotated ? 2 : 0);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Brightness");
    display.drawRect(0, 20, 84, 8, BLACK);
    display.fillRect(0, 20, map(brightness, 0, 255, 0, 84), 8, BLACK);
    display.setCursor(0, 32);
    display.print("< adj >  Back=A0");
    display.display();

    if (digitalRead(buttonLeft) == LOW) {
      brightness = max(0, brightness - 15);
      analogWrite(backlightPin, backlightOn ? brightness : 0);
      delay(150);
    }
    if (digitalRead(buttonRight) == LOW) {
      brightness = min(255, brightness + 15);
      analogWrite(backlightPin, backlightOn ? brightness : 0);
      delay(150);
    }
    if (digitalRead(buttonBack) == LOW) break;
  }
}

void adjustContrast() {
  while (true) {
    display.setRotation(rotated ? 2 : 0);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Contrast");
    display.drawRect(0, 20, 84, 8, BLACK);
    display.fillRect(0, 20, map(contrast, 0, 100, 0, 84), 8, BLACK);
    display.setCursor(0, 32);
    display.print("< adj >  Back=A0");
    display.display();

    if (digitalRead(buttonLeft) == LOW) {
      contrast = max(0, contrast - 5);
      display.setContrast(contrast);
      delay(150);
    }
    if (digitalRead(buttonRight) == LOW) {
      contrast = min(100, contrast + 5);
      display.setContrast(contrast);
      delay(150);
    }
    if (digitalRead(buttonBack) == LOW) break;
  }
}

void toggleBacklight() {
  backlightOn = !backlightOn;
  analogWrite(backlightPin, backlightOn ? brightness : 0);
}

void toggleInvert() {
  inverted = !inverted;
  display.invertDisplay(inverted);
}

void toggleRotation() {
  rotated = !rotated;
  display.setRotation(rotated ? 2 : 0);
  display.clearDisplay();
  drawSettingsMenu();
}

void resetSettings() {
  brightness = 128;
  contrast = 60;
  backlightOn = true;
  inverted = false;
  rotated = false;
  display.setContrast(contrast);
  analogWrite(backlightPin, brightness);
  display.setRotation(0);
  display.invertDisplay(false);
}// Variables
String calcExpression = "";
int selectedButton = 0;

void calculatorMenu() {
  const char* buttons[] = {
    "7", "8", "9", "/",
    "4", "5", "6", "*",
    "1", "2", "3", "-",
    "0", "C", "=", "+",
    "←" // Backspace button (row 5, col 1)
  };

  const int totalButtons = 17;
  const int cols = 4;
  const int rows = 5;

  while (true) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);

    // Display current expression
    display.setCursor(0, 0);
    display.print(calcExpression);

    // Evaluate and show result if expression is valid
    if (calcExpression.length() > 0 && isExpressionValid(calcExpression)) {
      float result = evaluate(calcExpression);
      display.setCursor(40, 0);
      display.print("= ");
      display.print(result, 2);
    }

    // Draw buttons
    for (int i = 0; i < totalButtons; i++) {
      int x = (i % cols) * 19;
      int y = 9 + (i / cols) * 10;

      if (i == selectedButton)
        display.fillRect(x, y, 21, 11, BLACK);
      else
        display.drawRect(x, y, 21, 11, BLACK);

      display.setCursor(x + 5, y + 2);
      display.setTextColor(i == selectedButton ? WHITE : BLACK);
      display.print(buttons[i]);
      display.setTextColor(BLACK);
    }

    display.display();

    // Navigation
    if (digitalRead(buttonLeft) == LOW) {
      selectedButton = (selectedButton % cols == 0) ? selectedButton + (cols - 1) : selectedButton - 1;
      delay(150);
    }

    if (digitalRead(buttonRight) == LOW) {
      selectedButton = (selectedButton % cols == cols - 1) ? selectedButton - (cols - 1) : selectedButton + 1;
      if (selectedButton >= totalButtons) selectedButton = totalButtons - 1;
      delay(150);
    }

    if (digitalRead(buttonUp) == LOW) {
      selectedButton = (selectedButton < cols) ? selectedButton + (cols * (rows - 1)) : selectedButton - cols;
      if (selectedButton >= totalButtons) selectedButton = totalButtons - 1;
      delay(150);
    }

    if (digitalRead(buttonDown) == LOW) {
      selectedButton = (selectedButton >= cols * (rows - 1)) ? selectedButton % cols : selectedButton + cols;
      if (selectedButton >= totalButtons) selectedButton = totalButtons - 1;
      delay(150);
    }

    // Select button
    if (digitalRead(buttonSelect) == LOW) {
      String btn = buttons[selectedButton];

      if (btn == "C") {
        calcExpression = "";
      } else if (btn == "=") {
        calcExpression = String(evaluate(calcExpression), 2);
      } else if (btn == "←") {
        if (calcExpression.length() > 0) {
          calcExpression.remove(calcExpression.length() - 1); // Backspace logic
        }
      } else {
        calcExpression += btn;
      }

      delay(200);
    }

    // Check physical backspace button
    if (digitalRead(buttonBackspace) == LOW) {
      if (calcExpression.length() > 0) {
        calcExpression.remove(calcExpression.length() - 1);
      }
      delay(200);
    }

    if (digitalRead(buttonBack) == LOW) {
      inCalculator = false;
      drawMainMenu(selectedRow, selectedCol);
      break;
    }
  }
}


// Basic expression evaluator (supports only 1 operator between two numbers)
float evaluate(String expr) {
  char op = 0;
  int opIndex = -1;

  for (int i = 0; i < expr.length(); i++) {
    if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
      op = expr[i];
      opIndex = i;
      break;
    }
  }

  if (opIndex == -1) return expr.toFloat();

  float left = expr.substring(0, opIndex).toFloat();
  float right = expr.substring(opIndex + 1).toFloat();

  switch (op) {
    case '+': return left + right;
    case '-': return left - right;
    case '*': return left * right;
    case '/': return (right != 0) ? left / right : 0;
    default: return 0;
  }
}

bool isExpressionValid(String expr) {
  for (int i = 1; i < expr.length() - 1; i++) {
    if ((expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') &&
        isDigit(expr[i - 1]) && isDigit(expr[i + 1])) {
      return true;
    }
  }
  return false;
}
void timerMenu() {
  const char* timerOptions[] = {"Start Timer", "Set Duration", "Back"};
  timerMenuIndex = 0;

  while (true) {
    display.clearDisplay();
    display.setTextSize(1);
    for (int i = 0; i < TIMER_MENU_COUNT; i++) {
      int y = i * 14;
      if (i == timerMenuIndex)
        display.drawRect(0, y, 84, 14, BLACK);
      display.setCursor(10, y + 3);
      display.print(timerOptions[i]);
    }
    display.display();

    if (digitalRead(buttonUp) == LOW) {
      timerMenuIndex = (timerMenuIndex - 1 + TIMER_MENU_COUNT) % TIMER_MENU_COUNT;
      delay(150);
    }
    if (digitalRead(buttonDown) == LOW) {
      timerMenuIndex = (timerMenuIndex + 1) % TIMER_MENU_COUNT;
      delay(150);
    }

    if (digitalRead(buttonSelect) == LOW) {
      switch (timerMenuIndex) {
        case TIMER_START: runTimer(); break;
        case TIMER_SETTINGS: setTimerDuration(); break;
        case TIMER_BACK: drawMainMenu(selectedRow, selectedCol); return;
      }
      delay(200);
    }

    if (digitalRead(buttonBack) == LOW) {
      drawMainMenu(selectedRow, selectedCol);
      return;
    }
  }
}
void runTimer() {
  timerRunning = true;
  timerStartTime = millis();

  while (timerRunning) {
    unsigned long elapsed = (millis() - timerStartTime) / 1000;
    long remaining = timerDuration - elapsed;

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print("Time: ");
    display.print(remaining);
    display.print("s");
    display.display();

    if (remaining <= 0) {
      timerRunning = false;
      // Time's up - flash LED or sound buzzer
      for (int i = 0; i < 5; i++) {
        digitalWrite(BUILTIN_LED, HIGH);
        delay(200);
        digitalWrite(BUILTIN_LED, LOW);
        delay(200);
      }
    }

    if (digitalRead(buttonBack) == LOW) {
      timerRunning = false;
      return;
    }

    delay(200);
  }
}
void setTimerDuration() {
  while (true) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Set Time (sec): ");
    display.setCursor(20, 20);
    display.setTextSize(2);
    display.print(timerDuration);
    display.display();

    if (digitalRead(buttonUp) == LOW) {
      timerDuration += 5;
      delay(150);
    }
    if (digitalRead(buttonDown) == LOW) {
      timerDuration = max(5, timerDuration - 5);
      delay(150);
    }
    if (digitalRead(buttonSelect) == LOW || digitalRead(buttonBack) == LOW) {
      return;
    }
  }
}

void passwordMenu() {
  String entered = "";
  int currentDigit = 0;

  while (true) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);

    display.setCursor(0, 0);
    display.print("Enter Password:");

    // Show entered digits as * and current digit being selected
    display.setCursor(0, 12);
    for (int i = 0; i < entered.length(); i++) {
      display.print("*");
    }
    if (entered.length() < 4) {
      display.print("[");
      display.print(currentDigit);
      display.print("]");
    }

    display.setCursor(0, 24);
    display.print("Sel=A0 Back=A1");

    display.display();

    // Navigation for digit 0–9
    if (digitalRead(buttonUp) == LOW) {
      currentDigit = (currentDigit + 1) % 10;
      delay(200);
    }

    if (digitalRead(buttonDown) == LOW) {
      currentDigit = (currentDigit - 1 + 10) % 10;
      delay(200);
    }

    // Confirm digit
    if (digitalRead(buttonSelect) == LOW) {
      entered += String(currentDigit);
      currentDigit = 0;
      delay(200);
    }

    // Cancel
    if (digitalRead(buttonBack) == LOW) {
      entered = "";
      delay(200);
    }

    // Check if 4 digits entered
    if (entered.length() == 4) {
      if (entered == password) {
        passwordVerified = true;
        display.clearDisplay();
        display.setCursor(15, 10);
        display.print("welcome to");
        display.setCursor(20,20);
        display.print("hiki os");
        display.display();
        delay(2000);
        showHomeScreen = true;
        clockHour = 10;
        clockMinute = 0;
        drawHomeScreen();

        break;
      } else {
        display.clearDisplay();
        display.setCursor(0, 10);
        display.print("Wrong Password");
        display.display();
        delay(1000);
        entered = "";
      }
    }
  }
}
void drawHomeScreen() {
  display.clearDisplay();
  display.drawRect(0, 0, 84, 48, BLACK);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(10, 2);
  display.print("Welcome!");
  display.println(username);

  display.setCursor(10, 20);
  display.print("Time: ");
  if (clockHour < 10) display.print("0");
  display.print(clockHour);
  display.print(":");
  if (clockMinute < 10) display.print("0");
  display.print(clockMinute);

  display.display();
}

void userSettingsMenu() {
  int index = 0;
  const int count = 2;
  String items[] = {"Change Name", "Change Passcode"};

  while (true) {
    display.setRotation(rotated ? 2 : 0);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("User Settings");
    for (int i = 0; i < count; i++) {
      if (i == index) display.print("> ");
      else display.print("  ");
      display.println(items[i]);
    }
    display.display();

    if (digitalRead(buttonUp) == LOW) {
      index = (index - 1 + count) % count;
      delay(150);
    }
    if (digitalRead(buttonDown) == LOW) {
      index = (index + 1) % count;
      delay(150);
    }
    if (digitalRead(buttonSelect) == LOW) {
      if (index == 0) changeUsername();
      else if (index == 1) changePassword();
      delay(150);
    }
    if (digitalRead(buttonBack) == LOW) {
      drawMainMenu(selectedRow, selectedCol);

      return;
    }
  }
}
void changeUsername() {
  // Show the keyboard with current username as starting text (optional)
  String newUsername = runKeyboardInput(username);  // Pre-fill with current username

  // If a new name was entered, update the global username variable
  if (newUsername.length() > 0) {
    username = newUsername;
  }
}

void changePassword() {
  inputPassword = "";
  char digit = '0';

  while (true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Set New Code:");
    display.setCursor(0, 10);
    display.println(inputPassword + "_");
    display.setCursor(0, 20);
    display.print("Digit: ");
    display.print(digit);
    display.setCursor(0, 30);
    display.println("< > =Next  A0=Back");
    display.display();

    if (digitalRead(buttonLeft) == LOW) {
      digit--;
      if (digit < '0') digit = '9';
      delay(150);
    }
    if (digitalRead(buttonRight) == LOW) {
      digit++;
      if (digit > '9') digit = '0';
      delay(150);
    }
    if (digitalRead(buttonSelect) == LOW) {
      inputPassword += digit;
      delay(150);
    }
    if (digitalRead(buttonBack) == LOW) {
      if (inputPassword.length() > 0) {
        password = inputPassword;
      }
      return;
    }
  }
}

void updateInputDisplay(String input) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0, 0);
  display.print("Input:");
  display.setCursor(0, 10);

  int maxChars = 14;  // The screen can fit approx 14 characters per line at text size 1
  if (input.length() <= maxChars) {
    display.print(input);
  } else {
    // Display only the last part that fits
    display.print(input.substring(input.length() - maxChars));
  }

  display.display();
}

void snakeGame() {
  const int boxSize = 4;
  const int gridWidth = 21;   // 84 / 4
  const int gridHeight = 12;  // 48 / 4
  const int maxSnakeLength = gridWidth * gridHeight;

  struct Point {
    int x;
    int y;
  };

  Point snake[maxSnakeLength];
  int snakeLength = 3;
  int dirX = 1;  // Start moving right
  int dirY = 0;
  int score = 0;

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  // Load high score from EEPROM
  int highScore = EEPROM.read(0);

  // Select difficulty
  int moveDelay;
  if (selectedDifficulty == 0) moveDelay = 400;
  else if (selectedDifficulty == 1) moveDelay = 200;
  else moveDelay = 100;
                     // Hard

  // Initial Snake Position
  snake[0] = {5, 5};
  snake[1] = {4, 5};
  snake[2] = {3, 5};

  // Food Position
  Point food = {random(0, gridWidth), random(0, gridHeight)};

  bool gameOver = false;
  unsigned long lastMove = 0;

  while (!gameOver) {
    if (millis() - lastMove > moveDelay) {
      lastMove = millis();

      // Read input
      if (digitalRead(buttonUp) == LOW && dirY != 1) {
        dirX = 0; dirY = -1;
      } else if (digitalRead(buttonDown) == LOW && dirY != -1) {
        dirX = 0; dirY = 1;
      } else if (digitalRead(buttonLeft) == LOW && dirX != 1) {
        dirX = -1; dirY = 0;
      } else if (digitalRead(buttonRight) == LOW && dirX != -1) {
        dirX = 1; dirY = 0;
      }

      // Move the snake
      for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
      }
      snake[0].x += dirX;
      snake[0].y += dirY;

      // Collision with wall
      if (snake[0].x < 0 || snake[0].x >= gridWidth ||
          snake[0].y < 0 || snake[0].y >= gridHeight) {
        gameOver = true;
        break;
      }

      // Collision with self
      for (int i = 1; i < snakeLength; i++) {
        if (snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
          gameOver = true;
          break;
        }
      }

      // Eat food
      if (snake[0].x == food.x && snake[0].y == food.y) {
        if (snakeLength < maxSnakeLength) snakeLength++;
        food.x = random(0, gridWidth);
        food.y = random(0, gridHeight);
        score++;

        // 🔊 Sound + 💡 Blink LED when eating
        tone(BUZZER_PIN, 1000, 100);
        digitalWrite(BUILTIN_LED, HIGH);
        delay(50);
        digitalWrite(BUILTIN_LED, LOW);
      }

      // Draw
      display.clearDisplay();

      // Border
      display.drawRect(0, 0, display.width(), display.height(), BLACK);

      // Snake
      for (int i = 0; i < snakeLength; i++) {
        display.fillRect(snake[i].x * boxSize, snake[i].y * boxSize, boxSize, boxSize, BLACK);
      }

      // Food
      display.fillRect(food.x * boxSize, food.y * boxSize, boxSize, boxSize, BLACK);

      // Score
      display.setTextColor(BLACK);
      display.setCursor(2, 40);
      display.setTextSize(1);
      display.print("Score: ");
      display.print(score);

      display.display();
    }

    // Exit with back
    if (digitalRead(buttonBack) == LOW) break;
  }

  // Update high score if needed
  if (score > highScore) {
    EEPROM.write(0, score);
     // Needed for ESP devices
    highScore = score;
  }

  // Game Over screen
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(15, 10);
  display.println("Game Over!");
  display.setCursor(10, 25);
  display.print("Score: ");
  display.println(score);
  display.setCursor(10, 38);
  display.print("High: ");
  display.print(highScore);
  display.display();
  delay(2500);

  drawMainMenu(selectedRow, selectedCol);
}

void gameOverScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(15, 10);
  display.println("Game Over!");
  display.setCursor(5, 25);
  display.println("Press BACK to exit");
  display.display();

  // Wait until back is pressed
  while (digitalRead(buttonBack) == HIGH);
  delay(300); // debounce
}

int selectDifficulty() {
  int difficulty = 1; // 0 = Easy, 1 = Medium, 2 = Hard

  while (true) {
    display.clearDisplay();
    display.setTextColor(BLACK);
    display.setTextSize(1);
    display.setCursor(10, 5);
    display.println("Select Difficulty:");

    display.setCursor(15, 20);
    display.print(difficulty == 0 ? "> Easy" : "  Easy");
    display.setCursor(15, 30);
    display.print(difficulty == 1 ? "> Medium" : "  Medium");
    display.setCursor(15, 40);
    display.print(difficulty == 2 ? "> Hard" : "  Hard");

    display.display();

    if (digitalRead(buttonUp) == LOW) {
      difficulty = (difficulty + 2) % 3;
      delay(200);
    } else if (digitalRead(buttonDown) == LOW) {
      difficulty = (difficulty + 1) % 3;
      delay(200);
    } else if (digitalRead(buttonSelect) == LOW) {
      delay(300);
      return difficulty;
    }
  }
}



void snakeMenu() {
  int snakeMenuIndex = 0;
  const int snakeMenuCount = 3;
  const char* snakeMenuItems[] = {"1 Difficulty", "2 Scores", "3 Start"};

  while (true) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0, 0);
    display.println("Snake");

    for (int i = 0; i < snakeMenuCount; i++) {
      if (i == snakeMenuIndex) {
        display.print("> ");
      } else {
        display.print("  ");
      }
      display.println(snakeMenuItems[i]);
    }

    display.display();

    // Handle input
    if (millis() - lastPress > 150) {
      if (digitalRead(buttonUp) == LOW) {
        snakeMenuIndex = (snakeMenuIndex - 1 + snakeMenuCount) % snakeMenuCount;
        lastPress = millis();
      } else if (digitalRead(buttonDown) == LOW) {
        snakeMenuIndex = (snakeMenuIndex + 1) % snakeMenuCount;
        lastPress = millis();
      } else if (digitalRead(buttonSelect) == LOW) {
        lastPress = millis();
        if (snakeMenuIndex == 0) {
          selectedDifficulty = selectDifficulty();
        } else if (snakeMenuIndex == 1) {
          showHighScore();
        } else if (snakeMenuIndex == 2) {
          snakeGame();  // Run with selectedDifficulty
        }
      } else if (digitalRead(buttonBack) == LOW) {
        drawMainMenu(selectedRow, selectedCol);
        break;
      }
    }
  }
}

void showHighScore() {
  int highScore = EEPROM.read(0);

  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(10, 15);
  display.println("High Score");
  display.setCursor(25, 30);
  display.print(highScore);
  display.display();

  delay(2000);
}


void gpsMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.println("COMING SOON...");
  display.display();

  // TODO: Add GPS module handling here.
  delay(2000);
}
