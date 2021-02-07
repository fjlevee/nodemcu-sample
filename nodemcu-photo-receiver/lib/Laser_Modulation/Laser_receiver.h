/**   
 ******************************************************************************   
 * @file    Laser_receiver.h
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

#ifndef Laser_receiver_H
#define Laser_receiver_H
/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEFAULT_LASER_RECEIVER_PIN D1
#define DEFAULT_LASER_RECEIVER_FREQUENCY_HZ 1.0
#define DEFAULT_LASER_RECEIVER_TOUCH_DETECTION_THRESHOLD 3
class LaserReceiver
{
  // Init ESP8266 timer 0
  ESP8266Timer receiverTimer;
  uint8_t rx_buffer[44];
  uint8_t rx_bitnum; // number of bits received
  int recv_flag;     // light receive flag
  uint16_t msg_raw;
  uint8_t msg_done = 255;
  int _laser_receiver_pin;
  float _frequency;
  timer_callback _callback;
  int _touch_detection_threshold = DEFAULT_LASER_RECEIVER_TOUCH_DETECTION_THRESHOLD;
  uint8_t _last_code_received = 255;
  int _last_code_received_number = 0;
  uint8_t _touch_code_detected = 255;

public:
  void begin(timer_callback callback, int laser_receiver_pin = DEFAULT_LASER_RECEIVER_PIN, float frequency = DEFAULT_LASER_RECEIVER_FREQUENCY_HZ )
  {
    _laser_receiver_pin = laser_receiver_pin;
    _frequency = frequency;
    _callback = callback;
    //set photodiode pin as input
    pinMode(_laser_receiver_pin, INPUT);

    // Print info of CPU and board
    Serial.print(F("\nLaser Receiver created on "));
    Serial.println(ARDUINO_BOARD);
    //Serial.println(ESP8266_TIMER_INTERRUPT_VERSION);
    Serial.print(F("CPU Frequency = "));
    Serial.print(F_CPU / 1000000);
    Serial.println(F(" MHz"));

    start_timer();
  }

  void set_touch_detection_threshold(int touch_detection_threshold){
    _touch_detection_threshold = touch_detection_threshold;
    Serial.print("set_touch_detection_threshold to ");
    Serial.println(_touch_detection_threshold);
  }

  void start_timer()
  {
    rx_bitnum = 0; // bit number to be transmitted
    recv_flag = 0; // light send flag
    _last_code_received = 255;
    _last_code_received_number = 0;
    _touch_code_detected = 255;
    if (receiverTimer.setFrequency(_frequency, _callback))
    {
      Serial.print(F("Starting LaserReceiver receiverTimer OK, Frequency= "));
      Serial.print(_frequency);
      Serial.println(F(" Hz"));
    }
    else
    {
      Serial.println(F("Can't set LaserReceiver receiverTimer correctly with Frequency= "));
      Serial.print(_frequency);
      Serial.println(F(" Hz. Select another freq. or interval"));
    }
  }

  void stop_timer()
  {
    Serial.print(F("Stopping LaserReceiver receiverTimer"));
    receiverTimer.stopTimer();
  }

  uint16_t manchester_demodulate()
  {
    uint8_t i, check1, check2;
    uint16_t out = 0;

    /* first frame */
    for (i = 4; i < 20; i += 2)
    {
      check1 = rx_buffer[i];
      check2 = rx_buffer[i + 1];
      if (!check1 && check2)
        out |= (1 << (15 - ((i - 4) / 2)));
    }

    /* second frame */
    for (i = 26; i < 42; i += 2)
    {
      check1 = rx_buffer[i];
      check2 = rx_buffer[i + 1];
      if (!check1 && check2)
        out |= (1 << (7 - ((i - 26) / 2)));
    }
    return (out);
  }

  void receive()
  {
    if (_touch_code_detected == 255){
      int tmp;

      if (!recv_flag)
      {
        //tmp = !!(PIND & (1 << laser_receive_pin));		// direct pin access is better (quicker) than digitalRead at high frequencies
        tmp = digitalRead(_laser_receiver_pin);
        switch (rx_bitnum)
        {
        case 0:
          if (tmp == 1)
            rx_bitnum = 1;
          break;
        case 1:
          if (tmp == 0)
            rx_bitnum = 2;
          else
            rx_bitnum = 0;
          break;
        case 2:
          if (tmp == 1)
            rx_bitnum = 3;
          else
            rx_bitnum = 0;
          break;
        case 3:
          recv_flag = 1;
          for (rx_bitnum = 0; rx_bitnum < 4; rx_bitnum++)
          {
            rx_buffer[rx_bitnum] = rx_bitnum % 2;
          }
          rx_bitnum = 4;
          rx_buffer[rx_bitnum] = tmp;
          rx_bitnum++;
          break;
        }
      }
      else if (rx_bitnum < 44)
      {
        //rx_buffer[rx_bitnum] = !!(PIND & (1 << _laser_receiver_pin));
        rx_buffer[rx_bitnum] = digitalRead(_laser_receiver_pin);
        rx_bitnum++;
      }
      else
      {
        rx_bitnum = 0;
        recv_flag = 0;

        // Read data from the photodiode
        msg_raw = manchester_demodulate();
        msg_done = hamming_byte_decoder((msg_raw >> 8), (0xFF & msg_raw));

        if (_last_code_received == msg_done){
          _last_code_received_number ++;
        }else{
          _last_code_received_number = 1;
        }
        _last_code_received = msg_done;
        Serial.print((char)_last_code_received);
        Serial.print(" received ");
        Serial.print(_last_code_received_number);
        Serial.println(" time(s)");

        if (_last_code_received_number == _touch_detection_threshold)
        {
          _touch_code_detected = _last_code_received;
          // Stop Timer
          receiverTimer.stopTimer();
          Serial.print("Touch Detected: ");
          Serial.println((char) _touch_code_detected);
        }
        msg_done = 255;
      }
    }
  }

  uint8_t get_touch_code_detected()
  {
    uint8_t result = _touch_code_detected;
    _touch_code_detected = 255;
    return (result);
  }
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes -----------------------------------------------*/
#endif