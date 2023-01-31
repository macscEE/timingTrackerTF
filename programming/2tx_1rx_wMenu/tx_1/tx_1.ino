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

bool startSend = false; //Quando lo ricevo vuol dire che posso partire ad inviare dati
void setup()
{
  Serial.begin(9600);
  pinMode(button, INPUT);

  SPI.begin();
  radio.begin();
  network.begin(124, nodeTX1);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
}

//====================
bool flag = false;
void loop()
{
  network.update();
  while (network.available()) //Vado a leggere se ho l'ok di trasmettere
  {
    RF24NetworkHeader header1;
    network.read(header1, &startSend, sizeof(startSend));
  }
  if (startSend)
  {
    if (digitalRead(button) == LOW)
    {
      if (!flag)
      {
        Serial.print("Pulsante premuto:");
        sendData();
        startSend = false;  //Così per la prossima rilevazione deve comunque aspettare il via
      }
      flag = true;
    }
  }
  else
  {
    flag = false;  //Usato come antirimbalzo
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
