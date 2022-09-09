/*
 * Test della fotocellula collegata ad arduino, semplice verifica sul serial monitor del passaggio di un oggetto
 */

#define flag 2

void setup() 
{
  Serial.begin(9600);
  pinMode(flag, INPUT);
}

void loop() 
{
  if(digitalRead(flag) == LOW)
    Serial.println("Oggetto passato");
}
