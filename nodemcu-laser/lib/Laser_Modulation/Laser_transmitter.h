/**   
 ******************************************************************************   
 * @file    Laser_transmitter.h
 * @author  Francois-Joseph LEVEE   
 * @date    16/01/2021
 * @brief   photo transceiver peripheral driver
 *
 *
 ******************************************************************************   
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 *	
 ******************************************************************************   
 */
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

#ifndef Laser_transmitter_H
#define Laser_transmitter_H
/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEFAULT_LASER_TRANSMITTER_PIN D0
#define DEFAULT_LASER_TRANSMITTER_FREQUENCY_HZ 1.0
/* Private typedef -----------------------------------------------------------*/
class LaserTransmitter
{
  // Init ESP8266 timer 0
  ESP8266Timer transmitterTimer;
  uint8_t tx_buffer[44];
  uint8_t tx_bitnum; // bit number to be transmitted
  int send_flag;     // light send flag
  int _laser_transmitter_pin;
  float _frequency;
  timer_callback _callback;
public:
  void begin(timer_callback callback, int laser_transmitter_pin = DEFAULT_LASER_TRANSMITTER_PIN, float frequency = DEFAULT_LASER_TRANSMITTER_FREQUENCY_HZ)
  {
    tx_bitnum = 0; // bit number to be transmitted
    send_flag = 0; // light send flag
    _laser_transmitter_pin = laser_transmitter_pin;
    _frequency = frequency;
    _callback = callback;
    //set light pin as output
    pinMode(_laser_transmitter_pin, OUTPUT);
    start_timer();
  }

  void start_timer()
  {
    // Start the interrupt timer
    Serial.print(F("\nStarting ITimer on "));
    Serial.println(ARDUINO_BOARD);
    //Serial.println(ESP8266_TIMER_INTERRUPT_VERSION);
    Serial.print(F("CPU Frequency = "));
    Serial.print(F_CPU / 1000000);
    Serial.println(F(" MHz"));
    if(transmitterTimer.setFrequency(_frequency, _callback))
    {
      Serial.print(F("Starting LaserTransmitter transmitterTimer OK, Frequency= "));
      Serial.print(_frequency);
      Serial.println(F(" Hz"));
    }
    else
    {
      Serial.println(F("Can't set LaserTransmitter transmitterTimer correctly with Frequency= "));
      Serial.print(_frequency);
      Serial.println(F(" Hz. Select another freq. or interval"));
    }
  }

  void stop_timer()
  {
    Serial.print(F("Stopping LaserTransmitter transmitterTimer"));
    transmitterTimer.stopTimer();
  }

  void manchester_modulate(uint16_t light_msg)
  {
    int i;
    uint8_t tmp;

    // first start bit
    for (i = 0; i < 4; i++)
    {
      tx_buffer[i] = i % 2;
    }

    for (i = 15; i >= 8; i--)
    {
      tmp = !!(light_msg & (1 << i)); // i-th bit value of tx_buffer
      tx_buffer[2 * (15 - i) + 4] = (tmp ^ 1);
      tx_buffer[2 * (15 - i) + 5] = (tmp ^ 0);
    }

    // first stop bit
    tx_buffer[20] = 1;
    tx_buffer[21] = 0;

    // second start bit
    for (i = 22; i < 26; i++)
    {
      tx_buffer[i] = i % 2;
    }

    for (i = 7; i >= 0; i--)
    {
      tmp = !!(light_msg & (1 << i)); // i-th bit value of tx_buffer
      tx_buffer[2 * (7 - i) + 26] = (tmp ^ 1);
      tx_buffer[2 * (7 - i) + 27] = (tmp ^ 0);
    }

    // second stop bit
    tx_buffer[42] = 1;
    tx_buffer[43] = 0;

    send_flag = 1;
  }

  void transmit()
  {
    //generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)

    // Generate wave based on manchester input
    if (send_flag)
    {
      digitalWrite(_laser_transmitter_pin, tx_buffer[tx_bitnum]);

      // shift to next bit in the send buffer
      if (tx_bitnum < 43)
      {
        tx_bitnum++; // next bit
      }
      else
      {
        tx_bitnum = 0; // reset to beginning of buffer
        send_flag = 0; // done sending
      }
    }
  }
};
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes -----------------------------------------------*/
#endif
