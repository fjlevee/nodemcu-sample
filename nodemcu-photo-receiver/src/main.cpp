#include <Arduino.h>
#include <HT_hamming_encoder.h>
#include <HT_light_modulator.h>

#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include <ESP8266TimerInterrupt.h>

volatile bool statusLed = false;
volatile uint32_t lastMicros = 0;
#define FREQ_HZ 25000

// Init ESP8266 timer 0
ESP8266Timer ITimer;


#define PHOTORECEIVER_PIN D1
#define LED_PIN D8
HT_PhotoReceiver pdiode;
int last_received_code = 0;
long last_received_time = -1;

//=======================================================================
void ICACHE_RAM_ATTR TimerHandler()
{
  pdiode.receive();
  /*
  digitalWrite(LED_PIN, statusLed); //Toggle LED Pin
  statusLed = !statusLed;
  #if (TIMER_INTERRUPT_DEBUG > 0)
    Serial.println("Delta us = " + String(micros() - lastMicros));
    lastMicros = micros();
  #endif*/
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  delay(200);
  
  pdiode.set_light_receive_pin(PHOTORECEIVER_PIN);
  //pdiode.set_speed(25000); // bits/sec, must match laser bit transfer rate
  pdiode.begin();

  Serial.print(F("\nStarting TimerInterruptTest on "));
  Serial.println(ARDUINO_BOARD);
  //Serial.println(ESP8266_TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));


  if (ITimer.setFrequency(FREQ_HZ, TimerHandler))
  {
    lastMicros = micros();
    Serial.print(F("Starting ITimer OK, Frequency= "));
    Serial.print(FREQ_HZ);
    Serial.println(F(" Hz"));
  }
  else{
    Serial.println(F("Can't set ITimer correctly with Frequency= "));
    Serial.print(FREQ_HZ);
    Serial.println(F(" Hz. Select another freq. or interval"));
  }
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
    //Serial.print("CODE: ");
    //Serial.println(received_code);
    Serial.print(millis());
    Serial.print(" - code: ");
    Serial.println((char)received_code);

  }
}
