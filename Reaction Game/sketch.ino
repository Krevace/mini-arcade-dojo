#include <LiquidCrystal_I2C.h>

int buttonPins[4] = {2, 3, 4, 5};
int ledPins[4] = {6, 7, 8, 9};

LiquidCrystal_I2C lcd(0x27, 16, 2);

int length = 6;
int count = 0;
int activeLed;
long previousMillis;
double highscore;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A3));
   
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  lcd.init();
  lcd.backlight();

  Restart();
}

void loop() {
  activeLed = random(0, 4);
  digitalWrite(ledPins[activeLed], HIGH);
  
  if (WaitForInput() == activeLed) {
    count++;
    digitalWrite(ledPins[activeLed], LOW);
    if (count == length) {
      double newTime = (double) (millis() - previousMillis) / 1000;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("YOU WIN");
      lcd.setCursor(0, 1);
      lcd.print("Time: ");
      lcd.print(newTime);
      lcd.print(" s");
      if (newTime < highscore || highscore == 0) highscore = newTime;
      delay(500);
      WaitForInput();
      delay(500);
      Restart();
    }
    delay(1000);
  } else {
    digitalWrite(ledPins[activeLed], LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GAME OVER");
    delay(500);
    WaitForInput();
    delay(500);
    Restart();
  }
}

void PrintMenu()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Best: ");
  lcd.print(highscore);
  lcd.print(" s");
  lcd.setCursor(0, 1);
  lcd.print("PRESS ANY BUTTON");
}

int WaitForInput()
{
  while (true) {
    for (int i = 0; i < 4; i++) {
      if (!digitalRead(buttonPins[i])) return i;
    }
  }
}

void Restart()
{
  count = 0;
  PrintMenu();
  WaitForInput();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME START");
  delay(3000);
  previousMillis = millis();
}
