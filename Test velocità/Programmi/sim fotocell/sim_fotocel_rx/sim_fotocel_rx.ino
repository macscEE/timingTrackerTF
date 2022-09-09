// SimpleRx - the slave or the receiver

/*
 * Ricevo il segnale dei due pulsanti dal TX, calcolo il tempo che intercorre fra le
 * due pressioni
 * 
 */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

bool dataReceived; // this must match dataToSend in the TX
bool newData = false;

int count = 0;  //Conteggio pressioni pulsante
long inTime = 0;
long cuTime = 0;
//===========

void setup() {

    Serial.begin(9600);

    Serial.println("SimpleRx Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);
    radio.startListening();
}

//=============

void loop() 
{
    getData();
    getTime(); 
}

//==============

void getData() {
    if ( radio.available() ) 
    {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;
        if(dataReceived)
        {
          if(count == 0)
          inTime = millis();
          count++;
        }
    }
}

//==============

void getTime()
{
      if(count == 2)
    {
      cuTime = millis();
      long rslt = (cuTime - inTime);
     // Serial.print("Tempo in millisecondi: ");
     // Serial.println(rslt);
      long mills = (rslt%1000);
      long seconds = (rslt/1000);
      if(seconds > 99)
        seconds = 99;
      Serial.print("Tempo: ");
      Serial.print(seconds);
      Serial.print(".");
      Serial.println(mills);
      count = 0;
    }
}
