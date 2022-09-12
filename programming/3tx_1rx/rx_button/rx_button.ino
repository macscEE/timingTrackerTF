/*
   Ricevo pressioni dei pulsanti fra i vari TX, in RX calcolo i parziali dei tempi fra le pressioni e il tempo totale fra prima e ultima
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#define startButton 2
#define CE_PIN   9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
RF24Network network(radio);
const uint16_t nodeRX = 0;


int dataReceived; // this must match dataToSend in the TX
//invio un int così dal numero so chi lo ha inviato
bool done = false;

int count = 0;  //Conteggio pressioni pulsante
long inTime = 0;
long cuTime = 0;
long tempTime;
//===========

void setup()
{
  Serial.begin(9600);

  SPI.begin();
  radio.begin();
  network.begin(124, nodeRX);  //canale, indirizzo nodo
  radio.setDataRate(RF24_250KBPS);
  //inTime = millis();

  pinMode(startButton, INPUT);
}

//=============

void loop()
{
  while(!done)
  {
    if(digitalRead(startButton) == LOW)
    {
      done = true;
      inTime = millis();
      Serial.println("Partenza cronometro");
    }
  }
  radioReading();
  getTime();
}

//Funzioni di appoggio
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
    if(mills < 10)
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
    s = "0" + s;

  Serial.print("Tempo 2° parz.: ");
  Serial.print(seconds);
  Serial.println("." + s);

  long rsltTot = (cuTime - inTime);
  long millsTot = (rsltTot % 1000);
  long secondsTot = (rsltTot / 1000);
  s = empty + millsTot;

  if (millsTot < 100)
    s = "0" + s;
  if (secondsTot > 99)
    secondsTot = 99;

  Serial.print("Totale: ");
  Serial.print(secondsTot);
  Serial.println("." + s);
  tempTime = cuTime;
  dataReceived = 0;
  done = false; //Faccio ripartire la procedura di partenza
}
