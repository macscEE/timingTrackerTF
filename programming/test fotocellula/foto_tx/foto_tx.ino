/*
 * Test con un pulsante usato per iniziare la procedura di partenza e un segnale (LED) che 
   mi segnala la partenza e appena passo sulla fotocellula calcola il tempo fra spegnimento 
  LED e passaggio su fotocellula
 */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define fotocell 8
#define CE_PIN   9
#define CSN_PIN 10

const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

//Non cambia fra bool e boolean
bool dataToSend = false;

void setup() {

  Serial.begin(9600);

  Serial.println("SimpleTx Starting");
  pinMode(fotocell, INPUT);

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3, 5); // delay, count  ----> Sono i tentativi e il tempo fra di essi per comunicare con il RX, per ricevere il pacchetto di ACK
  radio.openWritingPipe(slaveAddress);
}

//====================
bool flag = false;
void loop()
{
  if (digitalRead(fotocell) == LOW)
  {
    if (!flag)
    {
      Serial.println("Passaggio oggetto");
      dataToSend = true;
      send();
      flag = true;
    }
  }
  else
  {
    flag = false;  //Usato come antirimbalzo
  }
}

//====================

void send() {

  bool rslt;
  rslt = radio.write( &dataToSend, sizeof(dataToSend) );
  // Always use sizeof() as it gives the size as the number of bytes.
  // For example if dataToSend was an int sizeof() would correctly return 2

  Serial.print("Data Sent ");
  Serial.print(dataToSend);
  if (rslt) //Verifica che il ritorno della funzione "write" sia true, ovvero ACK è stato ricevuto
  {
    Serial.println("  Acknowledge received");
  }
  else
  {
    Serial.println("  Tx failed");
  }
}
