#include <Arduino.h>

#include <HT_hamming_encoder.h>
#include <HT_light_modulator.h>
#include <Ticker.h>

#define LASER_LIGHT_PIN D1
#define LASER_TRIGGER_PIN D0
#define LASER_FIRE_PIN D8
uint16_t laser_msg = hamming_byte_encoder('E');
boolean laser_trigger_enabled = true;
HT_PhotoTransmitter laser;

Ticker laserTicker;

//tlaserTicker ticks
void transmit()
{

  // transmit message, if any
  laser.transmit();
  //Serial.println("transmit");
}
void setup()
{

  Serial.begin(9600);
  pinMode(LASER_TRIGGER_PIN, INPUT);
  pinMode(LASER_FIRE_PIN, OUTPUT);

  // put your setup code here, to run once:
  laser.set_light_send_pin(LASER_LIGHT_PIN);
  // must be 500+ bits/second and less than laser slew rate
  laser.begin();

  laserTicker.attach_ms(0.04, transmit);
  Serial.println("Laser started");
}


void loop()
{
  int val = digitalRead(LASER_TRIGGER_PIN); //read the value of the digital interface 3 assigned to val
  if ((val == LOW) && (laser_trigger_enabled))
  {
    digitalWrite(LASER_FIRE_PIN, HIGH);
    laser_trigger_enabled = false;
    laser.manchester_modulate(laser_msg);
    delay(10);
    digitalWrite(LASER_LIGHT_PIN, HIGH);
    delay(100);
    digitalWrite(LASER_LIGHT_PIN, LOW);

    Serial.println("Fire\n");

    digitalWrite(LASER_FIRE_PIN, LOW);
  }
  else if (val == HIGH)
  {
    laser_trigger_enabled = true;
  }
}