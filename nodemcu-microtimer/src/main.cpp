#include <Arduino.h>

#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include <ESP8266TimerInterrupt.h>

#define BUILTIN_LED 2 // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED

volatile bool statusLed = false;
volatile uint32_t lastMicros = 0;
// Frequency en Hz
#define FREQ_HZ 1

// Init ESP8266 timer 0
ESP8266Timer ITimer;

//#define LED BUILTIN_LED
#define LED_PIN D8

    //=======================================================================
void ICACHE_RAM_ATTR TimerHandler()
{
  
  digitalWrite(LED_PIN, statusLed); //Toggle LED Pin
  statusLed = !statusLed;

  #if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Delta us = " + String(micros() - lastMicros));
      lastMicros = micros();
  #endif
}
//=======================================================================
//                               Setup
//=======================================================================
void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial);

  delay(200);

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
  else
  {
    Serial.println(F("Can't set ITimer correctly with Frequency= "));
    Serial.print(FREQ_HZ);
    Serial.println(F(" Hz. Select another freq. or interval"));
  }
  Serial.println(F("Micro Timer Program Started."));
}
//=======================================================================
//                MAIN LOOP
//=======================================================================
void loop()
{
}