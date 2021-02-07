#include <Arduino.h>

#include <HT_hamming_encoder.h>
#include <Laser_transmitter.h>


#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include <ESP8266TimerInterrupt.h>

#define SEND_FREQUENCY_HZ 25000
//#define SEND_FREQUENCY_HZ 5000
volatile uint32_t lastMicros = 0;

#define LASER_LIGHT_PIN D1
#define LASER_TRIGGER_PIN D0
#define LASER_FIRE_PIN D8
uint16_t laser_msg = hamming_byte_encoder('0');
boolean laser_trigger_enabled = true;
LaserTransmitter laser;

// %ax Number of times that laser code is sent when trigger is activated
int shoot_code_number = 30;
int code_send_number = 0;

//=======================================================================
void ICACHE_RAM_ATTR TimerHandler()
{
  // transmit message, if any
  laser.transmit();

  #if (TIMER_INTERRUPT_DEBUG > 0)
    Serial.println("Delta us = " + String(micros() - lastMicros));
    lastMicros = micros();
  #endif
}

void setup()
{

  //Serial.begin(9600);
  Serial.begin(115200);
  pinMode(LASER_TRIGGER_PIN, INPUT);
  pinMode(LASER_FIRE_PIN, OUTPUT);

  // must be 500+ bits/second and less than laser slew rate
  laser.begin(TimerHandler, LASER_LIGHT_PIN, SEND_FREQUENCY_HZ);
  Serial.println(F("Laser Program started"));
}


void loop()
{
  int val = digitalRead(LASER_TRIGGER_PIN); //read the value of the digital interface 3 assigned to val
  if ((val == LOW) && (code_send_number == 0))
    digitalWrite(LASER_FIRE_PIN, HIGH);
  if ((val == LOW) && (code_send_number < shoot_code_number))
  {
    Serial.println("FIRE");
    //
    laser_trigger_enabled = false;
    laser.manchester_modulate(laser_msg);
    //
    delay(10);

    code_send_number++;
  }
  else if ((val == HIGH) && (!laser_trigger_enabled))
    {
      code_send_number = 0;
      laser_trigger_enabled = true;
      digitalWrite(LASER_FIRE_PIN, LOW);
  }
}