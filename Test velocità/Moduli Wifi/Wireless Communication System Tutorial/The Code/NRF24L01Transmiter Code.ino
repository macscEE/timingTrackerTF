#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
void setup()
{
    Serial.begin(9600);

    Mirf.cePin = 9;                
    Mirf.csnPin = 10;       
    Mirf.spi = &MirfHardwareSpi;
    Mirf.init();               
    Mirf.setRADDR((byte *)"Sen01");
    Mirf.payload = sizeof(unsigned int);
    Mirf.channel = 3;
    Mirf.config();
    Serial.println("I'm Sender...");
}
unsigned int adata = 0;
void loop()
{
    adata=123;
    byte data[Mirf.payload];
    data[0] = adata & 0xFF;                
    data[1] = adata >> 8;               
    Mirf.setTADDR((byte *)"Rec01");
    Mirf.send(data);
    while(Mirf.isSending()) {}
    delay(20);
}
