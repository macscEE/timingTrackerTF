/*
   Ricevo pressioni dei pulsanti fra i vari TX, in RX calcolo i parziali dei tempi fra le pressioni e il tempo totale fra prima e ultima
*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#define CE_PIN   9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);
RF24Network network(radio);
const uint16_t nodeRX = 0;


int dataReceived; // this must match dataToSend in the TX
//invio un int così dal numero so chi lo ha inviato
bool newData = false;

int count = 0;  //Conteggio pressioni pulsante
long inTime = 0;
long cuTime = 0;
long tempTime;
//===========

void setup()
{
  Serial.begin(9600);
  Serial.println("SimpleRx Starting");

  SPI.begin();
  radio.begin();
  network.begin(124, nodeRX);  //canale, indirizzo nodo
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  inTime = millis();

}

//=============

void loop()
{
  network.update();
  while (network.available()) //Non vi entra se non vi sono dati
  {
    RF24NetworkHeader header;
    network.read(header, &dataReceived, sizeof(dataReceived));
    switch (dataReceived)
    {
      case 1: Serial.println("From TX1");
        count = 1;
        break;
      case 2: Serial.println("From TX2");
        count = 2;
        break;
      default: Serial.println("Error");
        break;
    }
  }
  getTime();
}

//Funzioni di appoggio

void getTime()
{
  switch (count)
  {
    case 1: t1();
      break;
    case 2: t2();
      break;
  }
}
 
String s; 
String s2;
void t1()
{
  tempTime = inTime;
  cuTime = millis();
  long rslt = (cuTime - tempTime);
  long mills = (rslt % 1000);
  long seconds = (rslt / 1000);
  if (seconds > 99)
    seconds = 99;
  if (mills < 100)
    s = "0" + mills;
    
  Serial.print("Tempo 1° parz.: ");
  Serial.print(seconds);
  Serial.print(".");
  Serial.println(s);
  tempTime = cuTime;
  count = 0;
}

void t2()
{
  cuTime = millis();
  long rslt = (cuTime - tempTime);
  long mills = (rslt % 1000);
  long seconds = (rslt / 1000);
  if (mills < 100)
    s = "0" + mills;
  if (seconds > 99)
    seconds = 99;

  Serial.print("Tempo 2° parz.: ");
  Serial.print(seconds);
  Serial.print(".");
  Serial.println(s);

  long rsltTot = (cuTime - inTime);
  long millsTot = (rsltTot % 1000);
  long secondsTot = (rsltTot / 1000);
  if (millsTot < 100)
    s = "0" + millsTot;
  if (secondsTot > 99)
    secondsTot = 99;

  Serial.print("Totale: ");
  Serial.print(secondsTot);
  Serial.print(".");
  Serial.println(s);
  tempTime = cuTime;
  count = 0;
}
