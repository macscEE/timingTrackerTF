/*
 * Menù di selezione del tipo di test, ogni selezione farà lampeggiare un led diverso
 */
 
#include <LiquidCrystal_I2C.h>

#define buttonScor 2
#define buttonOK 7

#define led1 3

bool flag = true;
int count = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() 
{
  pinMode(buttonScor, INPUT);
  pinMode(buttonOK, INPUT);
  pinMode(led1, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.backlight();  //accendo retroilluminazione
  // Print a message to the LCD.
  lcd.print("Benvenuto!");
  delay(1000);
  lcd.clear();
  lcd.print("Seleziona il");
  lcd.setCursor(1,1);
  lcd.print("programma");
  delay(500);
  lcd.clear();
  while(flag)
  {
    if(digitalRead(buttonScor) == LOW)
    {
      count++;
      lcd.clear();  //Cambia il programma quindi cancello schermo per leggere
      if (count == 2)
        count = 0;
    }
    switch (count)
    { //Trova un modo di eseguire una sola funzione nel loop, tipo il frameBuffer del progetto di quinta
      case 0: blinkSlow();
        break;
      case 1: blinkFast();
        break;
      default:
        break;
    }

    if(digitalRead(buttonOK) == LOW)
      flag = false;
  }
}

void loop() 
{
  if(count == 0)
     {
      digitalWrite(led1, HIGH);
      delay(500);
      digitalWrite(led1, LOW);
      delay(500);
     }
   else
    {
      digitalWrite(led1, HIGH);
      delay(150);
      digitalWrite(led1, LOW);
      delay(150);
    }
}

void blinkSlow()
{
  lcd.setCursor(0,0);
  lcd.print("Blink lento");
}

void blinkFast()
{
  lcd.setCursor(0,0);
  lcd.print("Blink veloce");
}
