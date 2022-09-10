/* SimpleTx - the master or the transmitter
  Simulo la fotocellula con un pulsante, appena lo premo invia "true" altrimenti non invia nulla
*/
#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

#define button 2
#define CE_PIN   9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
RF24Network network(radio);
const uint16_t nodeTX1 = 1;
const uint16_t nodeRX = 0;
  
const int dataToSend = 1;

void setup() 
{
  Serial.begin(9600);
  pinMode(button, INPUT);

  SPI.begin();
  radio.begin();
  network.begin(124, nodeTX1);
  radio.setDataRate(RF24_250KBPS);  
  radio.startListening();
}

//====================
bool flag = false;
void loop()
{
  network.update();
  if (digitalRead(button) == LOW)
  {
    if (!flag)
    {
      Serial.print("Pulsante premuto:");
      sendData();
      flag = true;
    }
  }
  else
  {
    flag = false;  //Debounce
  }
}

//====================

void sendData() {

  bool rslt;
  RF24NetworkHeader header(nodeRX); //(nodo destinatario)
  rslt = network.write(header, &dataToSend, sizeof(dataToSend));
  
  if (rslt) //Verifica che il ritorno della funzione "write" sia true, ovvero ACK è stato ricevuto
  {
    Serial.println("  Acknowledge received");
  }
  else
  {
    Serial.println("  Tx failed");
  }
}
