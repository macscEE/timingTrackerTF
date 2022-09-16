/*
   Ricevo pressioni dei pulsanti fra i vari TX, in RX calcolo i parziali dei tempi fra le pressioni e il tempo totale fra prima e ultima
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <LiquidCrystal_I2C.h>

#define startButton 2
#define CE_PIN 9
#define CSN_PIN 10

// For LCD
#define buttonSCOR 3
#define buttonOK 4

LiquidCrystal_I2C lcd(0x27, 16, 2);

RF24 radio(CE_PIN, CSN_PIN);
RF24Network network(radio);
const uint16_t nodeRX = 0;
const uint16_t nodeTX1 = 1;
const uint16_t nodeTX2 = 2;

int dataReceived;      // this must match dataToSend in the TX
bool startSend = true; // invio quando voglio iniziare far attivare i TX

bool done = false; // invio un int così dal numero so chi lo ha inviato
long inTime = 0;
long cuTime = 0;
//long tempTime;

bool flag = true;
int count = 0;    // Conta le pressioni del pulsante e quindi i vari programmi del menu
int foto;         // per selezionare che fotocellula usare nel caso se ne usi una sola

void setup()
{
  Serial.begin(9600);

  SPI.begin();
  radio.begin();
  network.begin(124, nodeRX); // canale, indirizzo nodo
  radio.setDataRate(RF24_250KBPS);
  // inTime = millis();

  pinMode(startButton, INPUT);

  // For LCD
  pinMode(buttonSCOR, INPUT);
  pinMode(buttonOK, INPUT);
  lcd.begin(16, 2);
  lcd.backlight(); // accendo retroilluminazione
  // Print a message to the LCD.
  lcd.print("Benvenuto!");
  delay(1500);
  lcd.clear();
  lcd.print("Seleziona il");
  lcd.setCursor(0, 1);
  lcd.print("programma");
  delay(1500);
  lcd.clear();
  while (flag)
  {
    if (digitalRead(buttonSCOR) == LOW)
    {
      count++;
      lcd.clear(); // Cambia il programma quindi cancello schermo per leggere
      if (count == 2)
        count = 0;
    }
    switch (count)
    {
      case 0:
        oneTime();
        break;
      case 1:
        twoTime();
        break;
    }

    if (digitalRead(buttonOK) == LOW)
    {
      if (count == 1)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Posizionare le");
        lcd.setCursor(0, 1);
        lcd.print("fotocellule");
        delay(3000);
        lcd.clear();
        while (digitalRead(buttonOK) == HIGH)
        {
          lcd.setCursor(0, 0);
          lcd.print("Premi OK");
          lcd.setCursor(0, 1);
          lcd.print("per avanzare");
        }
      }

      if (count == 0)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Selezionare");
        lcd.setCursor(0, 1);
        lcd.print("fotocellula");
        delay(1500);
        lcd.clear();
        foto = 0; // Variabile temporanea di conteggio
        while (digitalRead(buttonOK) == HIGH)
        {
          if (digitalRead(buttonSCOR) == LOW)
          {
            foto++;
            lcd.clear();
          }
          if (foto == 2)
            foto = 0;

          if (foto == 0)
          {
            lcd.setCursor(0, 0);
            lcd.print("Fotocellula");
            lcd.setCursor(0, 1);
            lcd.print("0");
          }

          if (foto == 1)
          {
            lcd.setCursor(0, 0);
            lcd.print("Fotocellula");
            lcd.setCursor(0, 1);
            lcd.print("1");
          }
        }
      }
      flag = false;
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Premere START");
  lcd.setCursor(0, 1);
  lcd.print("per iniziare");
}

//=============

void loop()
{
  while (!done)
  {
    if (digitalRead(startButton) == LOW)
    {
      inTime = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Acquisisco..");
      Serial.println("Partenza cronometro");
      sendData();
      done = true;
    }
  }
  radioReading();
  getTime();
}

// Funzioni di appoggio

void oneTime()
{
  lcd.setCursor(0, 0);
  lcd.print("Singola");
  lcd.setCursor(0, 1);
  lcd.print("fotocellula");
}

void twoTime()
{
  lcd.setCursor(0, 0);
  lcd.print("Doppia");
  lcd.setCursor(0, 1);
  lcd.print("fotocellula");
}

void sendData()
{

  RF24NetworkHeader header1(nodeTX1); //(nodo destinatario)
  RF24NetworkHeader header2(nodeTX2); //(nodo destinatario)
  if (count == 0) //Uso solo una fotocellula
  {
    if (foto == 0)
      network.write(header1, &startSend, sizeof(startSend));
    if (foto == 1)
      network.write(header2, &startSend, sizeof(startSend));
  }
  else  //Uso solo due fotocellule
  {
    network.write(header1, &startSend, sizeof(startSend));
    network.write(header2, &startSend, sizeof(startSend));
  }
}

void radioReading()
{
  network.update();
  while (network.available()) // Non vi entra se non vi sono dati
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

String empty = String(); // Creo un oggetto stringa vuota
String sd = String();    // Dove metto le cifre dopo la virgola
String si = String();    // cifre prima della virgola
void t1()
{
  if (count == 0)
  {
    if (foto == 0)
    {
      //tempTime = inTime;
      timeCalc(inTime);
      Serial.println("Tempo: " + si + "." + sd);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tempo: " + si + "." + sd);
      lcd.setCursor(0, 1);
      lcd.print("OK per reset");
      while (digitalRead(buttonOK) == HIGH)
      {
        //Aspetto che venga premuto il pulsante
      }
      dataReceived = 0;
      done = false; // Riazzero la procedura dato che devo prendere solo un tempo
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Premere START");
      lcd.setCursor(0, 1);
      lcd.print("per iniziare");
    }
  }
  else
  {
    //tempTime = inTime;
    timeCalc(inTime);
    Serial.println("Tempo 1° parz.: " + si + "." + sd);
    //tempTime = cuTime;
    dataReceived = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T1: " + si + "." + sd);
  }
}


void t2()
{
  if (count == 0)
  {
    if (foto == 1)
    {
      //tempTime = inTime;
      timeCalc(inTime);
      Serial.println("Tempo: " + si + "." + sd);
      lcd.clear();
      while (digitalRead(buttonOK) == HIGH)
      {
        lcd.setCursor(0, 0);
        lcd.print("Tempo: " + si + "." + sd);
        lcd.setCursor(0, 1);
        lcd.print("OK per reset");
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Premere START");
      lcd.setCursor(0, 1);
      lcd.print("per iniziare");
      dataReceived = 0;
      done = false; // Riazzero la procedura dato che devo prendere solo un tempo
    }
  }
  else
  {
    timeCalc(cuTime);
    Serial.println("Tempo 2° parz.: " + si + "." + sd);
    timeCalc(inTime);
    Serial.println("Totale: " + si + "." + sd);
    //tempTime = cuTime;
    dataReceived = 0;
    done = false; // Faccio ripartire la procedura di partenza
  }
}

void timeCalc(long tempTime)
{
  cuTime = millis();
  long rslt = (cuTime - tempTime);
  Serial.println(cuTime);
  Serial.println(tempTime);
  long mills = (rslt % 1000);
  long seconds = (rslt / 1000);
  sd = empty + mills; // Creo una stringa che mi serve in caso debba aggiungere uno "0" ai millesimi
  si = empty + seconds;

  if (seconds > 99)
    seconds = 99;
  if (seconds < 10)
    si = "0" + si;
  if (mills < 100)
  {
    sd = "0" + sd;
    if (mills < 10)
      sd = "0" + sd; // Aggiungo un altro zero se è minore di 10, tipo "005"
  }
}
