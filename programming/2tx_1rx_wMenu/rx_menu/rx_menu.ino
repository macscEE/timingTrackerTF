/*
   Ricevo pressioni dei pulsanti fra i vari TX, in RX calcolo i parziali dei tempi fra le pressioni e il tempo totale fra prima e ultima
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <LiquidCrystal_I2C.h>

#define startButton 2
#define CE_PIN   9
#define CSN_PIN 10

//For LCD
#define buttonScor 3
#define buttonOK 4

RF24 radio(CE_PIN, CSN_PIN);
RF24Network network(radio);
const uint16_t nodeRX = 0;
const uint16_t nodeTX1 = 1;
const uint16_t nodeTX2 = 2;

int dataReceived; // this must match dataToSend in the TX
bool startSend = true;  //invio quando voglio iniziare far attivare i TX

bool done = false;  //invio un int così dal numero so chi lo ha inviato
long inTime = 0;
long cuTime = 0;
long tempTime;

bool flag = true; //Debounce button LCD
int count = 0;  //Conta le pressioni del pulsante e quindi i vari programmi del menu

void setup()
{
  Serial.begin(9600);

  SPI.begin();
  radio.begin();
  network.begin(124, nodeRX);  //canale, indirizzo nodo
  radio.setDataRate(RF24_250KBPS);
  //inTime = millis();

  pinMode(startButton, INPUT);

  //For LCD
  pinMode(buttonScor, INPUT);
  pinMode(buttonOK, INPUT);
  lcd.begin(16, 2);
  lcd.backlight();  //accendo retroilluminazione
  // Print a message to the LCD.
  lcd.print("Benvenuto!");
  delay(1000);
  lcd.clear();
  lcd.print("Seleziona il");
  lcd.setCursor(1, 1);
  lcd.print("programma");
  delay(500);
  lcd.clear();
  while (flag)
  {
    if (digitalRead(buttonScor) == LOW)
    {
      count++;
      lcd.clear();  //Cambia il programma quindi cancello schermo per leggere
      if (count == 2)
        count = 0;
    }
    switch (count)
    { //Trova un modo di eseguire una sola funzione nel loop, tipo il frameBuffer del progetto di quinta
      case 0: oneTime();
        break;
      case 1: twoTime();
        break;
      default:
        break;
    }

    if (digitalRead(buttonOK) == LOW)
    {
      flag = false;
  }
}

//=============

void loop()
{
  while (!done)
  {
    if (digitalRead(startButton) == LOW)
    {
      done = true;
      inTime = millis();
      Serial.println("Partenza cronometro");
      sendData();
    }
  }
  radioReading();
  getTime();
}

//Funzioni di appoggio

void oneTime()
{
  lcd.setCursor(0, 0);
  lcd.print("Singola");
  lcd.setCursor(1, 0);
  lcd.print("fotocellula");
}

void twoTime()
{
  lcd.setCursor(0, 0);
  lcd.print("Doppia");
  lcd.setCursor(1, 0);
  lcd.print("fotocellula");
}


void sendData() {

  RF24NetworkHeader header1(nodeTX1); //(nodo destinatario)
  RF24NetworkHeader header2(nodeTX2); //(nodo destinatario)
  network.write(header1, &startSend, sizeof(startSend));
  network.write(header2, &startSend, sizeof(startSend));
}

void radioReading()
{
  network.update();
  while (network.available()) //Non vi entra se non vi sono dati
  {
    RF24NetworkHeader header;
    network.read(header, &dataReceived, sizeof(dataReceived));
  }
}

void getTime()
{
  switch (dataReceived)
  {
    case 1: t1();
      break;
    case 2: t2();
      break;
  }
}

String empty = String();  //Creo un oggetto stringa vuota
String s = String();  //di appoggio
void t1()
{
  tempTime = inTime;
  cuTime = millis();
  long rslt = (cuTime - tempTime);
  long mills = (rslt % 1000);
  long seconds = (rslt / 1000);
  s = empty + mills;  //Creo una stringa che mi serve in caso debba aggiungere uno "0" ai millesimi

  if (seconds > 99)
    seconds = 99;
  if (mills < 100)
  {
    s = "0" + s;
    if (mills < 10)
      s = "0" + s;  //Aggiungo un altro zero se è minore di 10, tipo "005"
  }

  Serial.print("Tempo 1° parz.: ");
  Serial.print(seconds);
  Serial.println("." + s);
  tempTime = cuTime;
  dataReceived = 0;
}

void t2()
{
  cuTime = millis();
  long rslt = (cuTime - tempTime);
  long mills = (rslt % 1000);
  long seconds = (rslt / 1000);
  s = empty + mills;

  if (seconds > 99)
    seconds = 99;
  if (mills < 100)
  {
    s = "0" + s;
    if (mills < 10)
      s = "0" + s;  //Aggiungo un altro zero se è minore di 10, tipo "005"
  }


  Serial.print("Tempo 2° parz.: ");
  Serial.print(seconds);
  Serial.println("." + s);

  long rsltTot = (cuTime - inTime);
  long millsTot = (rsltTot % 1000);
  long secondsTot = (rsltTot / 1000);
  s = empty + millsTot;

  if (secondsTot > 99)
    secondsTot = 99;
  if (millsTot < 100)
  {
    s = "0" + s;
    if (millsTot < 10)
      s = "0" + s;  //Aggiungo un altro zero se è minore di 10, tipo "005"
  }

  Serial.print("Totale: ");
  Serial.print(secondsTot);
  Serial.println("." + s);
  tempTime = cuTime;
  dataReceived = 0;
  done = false; //Faccio ripartire la procedura di partenza
}
