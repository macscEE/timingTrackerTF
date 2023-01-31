/*
   Ricevo pressioni dei pulsanti fra i vari TX, in RX calcolo i parziali dei tempi fra le pressioni e il tempo totale fra prima e ultima
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <LiquidCrystal_I2C.h>

//For NRF24L01
#define CE_PIN 9
#define CSN_PIN 10

// For LCD
#define buttonSCOR 3
#define buttonOK 4

#define startButton 2
#define soundStart 5

LiquidCrystal_I2C lcd(0x27, 16, 2);

RF24 radio(CE_PIN, CSN_PIN);  //Vari nodi del sistema di trasmissione
RF24Network network(radio);
const uint16_t nodeRX = 0;
const uint16_t nodeTX1 = 1;
const uint16_t nodeTX2 = 2;

bool debounce = false;  //Uso come antirimbalzo
int dataReceived; // invio un int così dal numero so che fotocellula ha inviato
bool startSend = true; // invio quando voglio iniziare far attivare i TX

bool done = false;
long inTime = 0;
long cuTime = 0;

bool flag = true;
int count = 0;    // Conta le pressioni del pulsante e quindi i vari programmi del menu
int foto;         // per selezionare che fotocellula usare nel caso se ne usi una sola

int t1i, t1d, t2i, t2d, ti, td; //Var. per memorizzare i tempi anche come stringhe
String t1si, t1sd, t2si, t2sd, tsi, tsd;

void setup()
{
  Serial.begin(9600);

  SPI.begin();
  radio.begin();
  network.begin(124, nodeRX); // canale, indirizzo nodo
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);

  pinMode(startButton, INPUT);
  pinMode(soundStart, OUTPUT);

  // For menu LCD
  pinMode(buttonSCOR, INPUT);
  pinMode(buttonOK, INPUT);

  lcd.begin(16, 2);
  lcd.backlight(); // accendo retroilluminazione
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
      if (!debounce)
      {
        count++;
        lcd.clear(); // Cambia il programma quindi cancello schermo per leggere
        if (count == 2)
          count = 0;
        debounce = true;
      }
    }
    else
    {
      debounce = false;
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
        lcd.setCursor(0, 0);
        lcd.print("Premi OK");
        lcd.setCursor(0, 1);
        lcd.print("per avanzare");

        while (digitalRead(buttonOK) == HIGH)
        {
          //Attendo
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
            if (!debounce)
            {
              foto++;
              lcd.clear();
              debounce = true;
            }
          }
          else
          {
            debounce = false;
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

//===========================================================================================

void loop()
{
  while (!done)
  {
    if (digitalRead(startButton) == LOW)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pronti..");

      digitalWrite(soundStart, HIGH);
      delay(800);
      digitalWrite(soundStart, LOW);

      int wait = random(3000, 5000); //persona ora sul "pronti"
      delay(wait);  //attesa in millisecondi
      digitalWrite(soundStart, HIGH);  //segnalo con il led lo sparo

      inTime = millis();

      delay(600);
      digitalWrite(soundStart, LOW);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Acquisisco..");
      Serial.println("Partenza cronometro");

      sendData(); //Faccio trasmettere le fotocellule
      done = true;
    }
  }
  radioReading();
  getTime();
}
//===========================================================================================
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
      timeCalc();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tempo: " + tsi + "." + tsd);
      lcd.setCursor(0, 1);
      lcd.print("OK per reset");

      while (digitalRead(buttonOK) == HIGH)
      {
        //Aspetto che venga premuto il pulsante
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
    timeCalc();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T1: " + si + "." + sd);

    dataReceived = 0;
  }
}


void t2()
{
  if (count == 0)
  {
    if (foto == 1)
    {
      timeCalc();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tempo: " + tsi + "." + tsd);
      lcd.setCursor(0, 1);
      lcd.print("OK per reset");

      while (digitalRead(buttonOK) == HIGH)
      {
        //Aspetto di ricevere conferma per avanzare
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
    timeCalc();

    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print("Totale: " + tsi + "." + tsd);
    lcd.setCursor(0, 1);
    lcd.print("SCOR per T1, T2");

    calcT2();

    int temp = -1; //variabile di conteggio temporaneo
    
    while (digitalRead(buttonOK) == HIGH)
    {
      if (digitalRead(buttonSCOR) == LOW)
      {
        if (!debounce)
        {
          temp++;
          lcd.clear();
          if (temp == 2)
            temp = 0;
          debounce = true;
        }
      }
      else
      {
        debounce = false;
      }

      if (temp == 0)
      {
        lcd.setCursor(0, 0);
        lcd.print("T1: " + t1si + "." + t1sd);
        lcd.setCursor(0, 1);
        lcd.print("T2: " + t2si + "." + t2sd);
      }

      if (temp == 1)
      {
        lcd.setCursor(0, 0);
        lcd.print("Totale: " + tsi + "." + tsd);
        lcd.setCursor(0, 1);
        lcd.print("OK per reset");
      }
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Premere START");
    lcd.setCursor(0, 1);
    lcd.print("per iniziare");

    dataReceived = 0;
    done = false; // Faccio ripartire la procedura di partenza
  }
}

void timeCalc()
{
  cuTime = millis();
  long rslt = (cuTime - inTime);

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

  if (count == 1)
  {
    if (dataReceived == 1) //T1
    {
      t1i = seconds; //Interi
      t1d = mills;

      t1si = si;  //Stringhe
      t1sd = sd;
    }
    else  //dataReceived == 2 || Totale, quindi ho T1 + T2
    {
      ti = seconds; //Interi
      td = mills;

      tsi = si; //Stringhe
      tsd = sd;
    }
  }
  else
  {
    tsi = si; //Stringhe
    tsd = sd;

  }
}

void calcT2()
{
  //Calcolo i secondi
  t2i = ti - t1i;

  if (td < t1d)
  {
    t2d = (1000 + td) - t1d;  //Prendo in prestito un secondo = 1000 millisecondi
    t2i--;
  }
  else
  {
    //Calcolo i decimi
    t2d = td - t1d;
  }

  t2si = t2i + empty; //Trasformo in stringa per scrivere su display
  t2sd = t2d + empty;

  if (t2i > 99)
    t2i = 99;
  if (t2i < 10)
    t2si = "0" + t2si;
  if (t2d < 100)
  {
    t2sd = "0" + t2sd;
    if (t2d < 10)
      t2sd = "0" + t2sd; // Aggiungo un altro zero se è minore di 10, tipo "005"
  }  
}
