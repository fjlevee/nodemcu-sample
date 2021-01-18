#include <Arduino.h>
#include <HT_hamming_encoder.h>
#include <ESP8266_Light_modulator.h>
// Reception Freque cy in 
#define RECEIVE_FREQUENCY_HZ 25000

#define PHOTORECEIVER_PIN D1
LaserReceiver pdiode;
int last_received_code = 0;
long last_received_time = -1;

void ICACHE_RAM_ATTR TimerHandler()
{
  pdiode.receive();
}

//=======================================================================

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  delay(200);

  pdiode.begin(PHOTORECEIVER_PIN, RECEIVE_FREQUENCY_HZ, TimerHandler);

  Serial.print("PHOTORECEIVER_PIN D1: ");
  Serial.println(D1);
  Serial.print("LED_PIN 8: ");
  Serial.println(D8);
  Serial.println(F("Photo Receiver Program Started."));
} //end setup

void loop()
{
  //print and return most recently received byte, if any, only once
  //pdiode.printByte();
  
  uint8_t received_code = pdiode.getMsgCodeByte();
  if (received_code != 255)
  {
    Serial.print(millis());
    Serial.print(" - code: ");
    Serial.println((char)received_code);
  }
}
