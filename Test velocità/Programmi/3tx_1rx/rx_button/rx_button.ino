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
const uint16_t nodeRX = 00;


int dataReceived; // this must match dataToSend in the TX
//invio un int così dal numero so chi lo ha inviato
bool newData = false;

int count = 0;  //Conteggio pressioni pulsante
long inTime = 0;
long cuTime = 0;
//===========

void setup()
{
  Serial.begin(9600);
  Serial.println("SimpleRx Starting");

  SPI.begin();
  radio.begin();
  network.begin(90, nodeRX);  //canale, indirizzo nodo
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
      case 1: Serial.println("Inviato da TX1");
        break;
      case 2: Serial.println("Inviato da TX2");
        break;
      case 3: Serial.println("Inviato da TX3");
        break;
      default: Serial.println("Errore");
        break;
    }
    //getTime();
  }
}

//==============
long tempMill, tempSec, tempTime;
void getTime()
{
  if (count == 0)
  {
    cuTime = millis();
    long rslt = (cuTime - inTime);
    long mills = (rslt % 1000);
    long seconds = (rslt / 1000);
    if (seconds > 99)
      seconds = 99;
    Serial.print("Tempo 1° parz.: ");
    Serial.print(seconds);
    Serial.print(".");
    Serial.println(mills);
    tempMill = mills;
    tempSec = seconds;
    tempTime = cuTime;
    count++;
  }

  if(count == 1)
  {
    cuTime = millis();
    long rslt = (tempTime - cuTime);
    long mills = (rslt % 1000);
    long seconds = (rslt / 1000);
    if (seconds > 99)
      seconds = 99;
    Serial.print("Tempo 2° parz.: ");
    Serial.print(seconds);
    Serial.print(".");
    Serial.println(mills);
    tempMill = mills;
    tempSec = seconds;
    tempTime = cuTime;
    count++;
  }

  if(count == 2)
  {
    cuTime = millis();
    long rslt = (tempTime - cuTime);
    long mills = (rslt % 1000);
    long seconds = (rslt / 1000);
    if (seconds > 99)
      seconds = 99;
    Serial.print("Tempo 3° parz.: ");
    Serial.print(seconds);
    Serial.print(".");
    Serial.println(mills);
    tempMill = mills;
    tempSec = seconds;
    tempTime = cuTime;
    count = 0;
  }

}
