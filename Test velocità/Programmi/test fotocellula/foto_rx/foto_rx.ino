// SimpleRx - the slave or the receiver

/*
  Test con un pulsante usato per iniziare la procedura di partenza e un segnale (LED) che 
  mi segnala la partenza e appena passo sulla fotocellula calcola il tempo fra spegnimento 
  LED e passaggio su fotocellula
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

#define buttonStart 2
#define led 3

const byte thisSlaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};

RF24 radio(CE_PIN, CSN_PIN);

bool dataReceived; // this must match dataToSend in the TX
bool newData = false;

int count = 0;  //Conteggio pressioni pulsante
long inTime = 0;
long cuTime = 0;
//===========

void setup() {

  Serial.begin(9600);
  pinMode(buttonStart, INPUT);
  pinMode(led, OUTPUT);
  Serial.println("SimpleRx Starting");
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();

  startProcedure();
}

//=============

void loop()
{
  getData();
  getTime(); 
}

void startProcedure()
{
  bool startFlag = true;
  digitalWrite(led, HIGH);
  while (startFlag) //cicla fino a quando non viene dato il via
  {
    if (digitalRead(buttonStart) == LOW)
    {
      int wait = random(500, 2000); //persona ora sul "pronti"
      delay(wait);  //attesa in millisecondi
      digitalWrite(led, LOW);  //segnalo con il led lo sparo
      startFlag = false;
    }
  }
  inTime = millis();
}

//==============

void getData() {
  if ( radio.available() )
  {
    radio.read( &dataReceived, sizeof(dataReceived) );
    newData = true;
    if (dataReceived)
      count++;
  }
}

//==============

void getTime()
{
  if (count == 1)
  {
    cuTime = millis();
    Serial.print("Tempo corrente: ");
    Serial.println(cuTime);
    Serial.print("Tempo iniziale: ");
    Serial.println(inTime);
    long rslt = (cuTime - inTime);
    // Serial.print("Tempo in millisecondi: ");
    // Serial.println(rslt);
    long mills = (rslt % 1000);
    long seconds = (rslt / 1000);
    if (seconds > 99)
      seconds = 99;
    Serial.print("Tempo: ");
    Serial.print(seconds);
    Serial.print(".");
    Serial.println(mills);
    count = 0;
  }
}
