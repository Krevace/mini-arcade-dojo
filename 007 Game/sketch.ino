#include <LiquidCrystal_I2C.h>
#include <Segment.h>  

#define SHOOT_BTN 2
#define RELOAD_BTN 3
#define SHIELD_BTN 4
#define DEAD_LED 5
#define SEG_DATA A0
#define SEG_LATCH A1
#define SEG_CLOCK A2

LiquidCrystal_I2C lcd(0x27, 16, 2);
Segment sevseg(A0, A2, A1, -1, 7, false);

int ammo = 0;
int enemyAmmo = 0;
int move = 0;
int enemyMove = 0;
long currentMillis;
long previousMillis;
bool alive = true;
bool enemyAlive = true;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A3));
   
  pinMode(SHOOT_BTN, INPUT_PULLUP);
  pinMode(RELOAD_BTN, INPUT_PULLUP);
  pinMode(SHIELD_BTN, INPUT_PULLUP);
  pinMode(SEG_DATA, OUTPUT);
  pinMode(SEG_LATCH, OUTPUT);
  pinMode(SEG_CLOCK, OUTPUT);
  pinMode(DEAD_LED, OUTPUT);
  digitalWrite(DEAD_LED, LOW);

  lcd.init();
  lcd.backlight();
  UpdateStatus();
  previousMillis = millis();
}

void loop() {
  currentMillis = millis();
  if (GetInput()) UpdateStatus();
  if (move && enemyMove) {
    if (move == 1) {
      if (enemyMove != 3) enemyAlive = false;
      ammo--;
    }
    if (enemyMove == 1) {
      if (move != 3) alive = false;
      enemyAmmo--;
    }
    if (move == 2) ammo++;
    if (enemyMove == 2) enemyAmmo++;
    if (move == 4) enemyAlive = false;
    if (enemyMove == 4) alive = false;

    delay(1000);

    sevseg.display(String(ammo)[0]);  
    if (!alive || !enemyAlive) lcd.clear();
    if (!alive) {
      digitalWrite(DEAD_LED, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("P1 DIED");
      if (enemyAlive) {
        lcd.setCursor(0, 1);
        lcd.print("P2 WON");
      }
    }
    if (!enemyAlive) {
      if (alive) {
        lcd.setCursor(0, 0);
        lcd.print("P1 WON");
      }
      lcd.setCursor(0, 1);
      lcd.print("P2 DIED");
    }
   
    delay(2000);
    if (!alive || !enemyAlive) {
      ammo = 0;
      enemyAmmo = 0;
      alive = true;
      enemyAlive = true;
      digitalWrite(DEAD_LED, LOW);
    }
    move = 0;
    enemyMove = 0;
    previousMillis = millis();
    UpdateStatus();
  }
}

void UpdateStatus()
{
  sevseg.display(String(ammo)[0]);  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (!move) lcd.print("Waiting...");
  else lcd.print("P1 ");
  StatusOutput(move);
  lcd.setCursor(0, 1);
  if (!enemyMove) lcd.print("Waiting...");
  else if (!move) lcd.print("Ready!");
  else {
    lcd.print("P2 ");
    StatusOutput(enemyMove);
  }
}

bool GetInput()
{
  if (!move) {
    if (!digitalRead(SHOOT_BTN)) {
      if (ammo == 0) move = 5;
      else if (ammo < 5) move = 1;
      else move = 4;
      return true;
    }
    else if (!digitalRead(RELOAD_BTN)) { move = 2; return true; }
    else if (!digitalRead(SHIELD_BTN)) { move = 3;  return true; }
  }
  if (currentMillis - previousMillis > 3000 && !enemyMove) {
    enemyMove = random(1, 4);
    if (enemyMove == 1) {
      if (enemyAmmo == 0) enemyMove = 5;
      else if (enemyAmmo >= 5) enemyMove = 4;
    }
    return true;
  }
  return false;
}

void StatusOutput(int move)
{
  if (move == 1) lcd.print("SHOT");
  else if (move == 2) lcd.print("RELOADED");
  else if (move == 3) lcd.print("GUARDED");
  else if (move == 4) lcd.print("SHOT A 5X");
  else if (move == 5) lcd.print("FUMBLED");
}
