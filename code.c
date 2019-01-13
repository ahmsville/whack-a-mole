#include <LiquidCrystal.h>

/*The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 8
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)
*/

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 8, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int moleSwitch[4] = {A1, A2, A3, A4}; //whack detection switch pins
int moles[4] = {6, 9, 10, 11};  //mole mechanics control pins

int score = 0, level = 1;
int power = 255;  //controls the amount of power given to the coils that controls the moles (PWM)
int molesperlev = 15; //number of moles per level
int molecount, previousmole = 0, randmole;

int whackdetwin = 1000; //sets the maximum time allowed to Whack one mole in milliseconds, decreases by 25% anytime the level changes
int timeframe, whackdetector = 0;

void setup() {
  for (int i = 0; i < 4; i++) { // configure appropriate input and output pins
    pinMode(moleSwitch[i], INPUT);
    pinMode(moles[i], OUTPUT);
  }
  lcd.begin(16, 2);
  lcd.print("Get Ready to");
  lcd.setCursor(0, 1);
  lcd.print("Whack Some Moles");
  delay(2000);
  lcd.clear();
  lcd.print("BEGIN");
  delay(400);
  lcd.clear();
  lcd.print("MolesWhacked: ");
  lcd.setCursor(13, 0);
  lcd.print(score);
  lcd.setCursor(10, 1);
  lcd.print("LEVEL");
  lcd.setCursor(15, 1);
  lcd.print(level);
}

void loop() {
  
  randmole = rand() % 4; // selects a mole at random
  if (randmole != previousmole) {  // runs if newly selected mole is not the same as the previously active mole
    
    // level selector
    if (molecount < molesperlev) { // level 1
      timeframe = whackdetwin;
      level = 1;
    } else if ((molecount >= molesperlev) && (molecount < molesperlev * 2)) { // level 2
      timeframe = 0.75 * whackdetwin;
      level = 2;
    } else if ((molecount >= molesperlev * 2) && (molecount < molesperlev * 3)) { // level 3
      timeframe = 0.5 * whackdetwin;
      level = 3;
    }
    else if ((molecount >= molesperlev * 3) && (molecount <= molesperlev * 4)) { // level 4
      timeframe = 0.25 * whackdetwin;
      level = 4;
    } else if (molecount > molesperlev * 4) { // Game End
     
      lcd.clear();
      if (score <= 1.2 * molesperlev) { // if you whacked less than 30% of the moles
        lcd.print("You Kinda Suck");
        lcd.setCursor(0, 1);
        lcd.print(score);
        lcd.setCursor(3, 1);
        lcd.print("Moles Whacked");
      } else if (score < 3.2 * molesperlev && score > 1.2 * molesperlev) { // if you whacked 30 - 80% of the moles
        lcd.print("Not Bad");
        lcd.setCursor(0, 1);
        lcd.print(score);
        lcd.setCursor(3, 1);
        lcd.print("Moles Whacked");
      } else if (score >= 3.2 * molesperlev) { // if you whacked more than 70% of the moles
        lcd.print("Awesome Whaking");
        lcd.setCursor(0, 1);
        lcd.print(score);
        lcd.setCursor(3, 1);
        lcd.print("Moles Whacked");
      }
      while (1) { // turn of all solenoids
        for (int j = 0; j < 4; j++) {
          analogWrite(moles[j], 0);
        }
      }
    }
    // game starts from here
    analogWrite(moles[randmole], power); // activate the selected mole
    for (int i = 0; i < timeframe; i++) {
      if (digitalRead(moleSwitch[randmole]) == 1) {
        whackdetector = 1;
        analogWrite(moles[randmole], 0);
        delay(1);
      } else {
        delay(1);
      }
    }
    analogWrite(moles[randmole], 0);  // deactivate activated mole if it didnt get whacked
    if (whackdetector == 1) { //increment the score if a mole was whacked within the given timeframe
      score += 1;
      whackdetector = 0;   //reset wackdetector to 0
    } else {
      lcd.setCursor(0, 1);
      lcd.print("missed");
      delay(200);
    }
    lcd.clear();      // update game status
    lcd.print("MolesWhacked: ");
    lcd.setCursor(13, 0);
    lcd.print(score);
    lcd.setCursor(10, 1);
    lcd.print("LEVEL");
    lcd.setCursor(15, 1);
    lcd.print(level);
    molecount++;   //increment molecount for the next cycle
    previousmole = randmole;   //indicate previously active mole for the next cycle
  }
}
