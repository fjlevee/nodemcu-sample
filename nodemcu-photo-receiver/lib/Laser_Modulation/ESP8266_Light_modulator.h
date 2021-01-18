/**   
 ******************************************************************************   
 * @file    ESP8266_Light_modulator.h
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

#ifndef ESP8266_Light_modulator_H
#define ESP8266_Light_modulator_H
/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEFAULT_LASER_SEND_PIN D0
#define DEFAULT_LASER_RECEIVE_PIN D1
#define DEFAULT_FREQUENCY_HZ 1.0
/* Private typedef -----------------------------------------------------------*/
class LaserTransmitter
{
	// Init ESP8266 timer 0
	ESP8266Timer ITimer;
	uint8_t tx_buffer[44];
	uint8_t tx_bitnum; // bit number to be transmitted
	int send_flag;		 // light send flag
	int laser_send_pin = DEFAULT_LASER_SEND_PIN;
	float frequency = DEFAULT_FREQUENCY_HZ;

public:
	void begin(int custom_laser_send_pin, float custom_frequency, timer_callback callback)
	{
		tx_bitnum = 0; // bit number to be transmitted
		send_flag = 0; // light send flag
		laser_send_pin = custom_laser_send_pin;
		frequency = custom_frequency;
		//set light pin as output
		pinMode(laser_send_pin, OUTPUT);

		// Start the interrupt timer
		Serial.print(F("\nStarting ITimer on "));
		Serial.println(ARDUINO_BOARD);
		//Serial.println(ESP8266_TIMER_INTERRUPT_VERSION);
		Serial.print(F("CPU Frequency = "));
		Serial.print(F_CPU / 1000000);
		Serial.println(F(" MHz"));
		if (ITimer.setFrequency(frequency, callback))
		{
			Serial.print(F("Starting LaserTransmitter ITimer OK, Frequency= "));
			Serial.print(frequency);
			Serial.println(F(" Hz"));
		}
		else
		{
			Serial.println(F("Can't set LaserTransmitter ITimer correctly with Frequency= "));
			Serial.print(frequency);
			Serial.println(F(" Hz. Select another freq. or interval"));
		}
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
			digitalWrite(laser_send_pin, tx_buffer[tx_bitnum]);

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

class LaserReceiver
{
	// Init ESP8266 timer 0
	ESP8266Timer ITimer;
	uint8_t rx_buffer[44];
	uint8_t rx_bitnum; // number of bits received
	int recv_flag;		 // light receive flag
	uint16_t msg_raw;
	uint8_t msg_done;
	uint8_t can_print;
	int laser_receive_pin = DEFAULT_LASER_RECEIVE_PIN;
	float frequency = DEFAULT_FREQUENCY_HZ;

public:
	void begin(int custom_laser_receive_pin, float custom_frequency, timer_callback callback)
	{
		rx_bitnum = 0; // bit number to be transmitted
		recv_flag = 0; // light send flag
		can_print = 0;
		laser_receive_pin = custom_laser_receive_pin;
		frequency = custom_frequency;
		//set photodiode pin as input
		pinMode(laser_receive_pin, INPUT);

		// Start the interrupt timer
		Serial.print(F("\nStarting ITimer on "));
		Serial.println(ARDUINO_BOARD);
		//Serial.println(ESP8266_TIMER_INTERRUPT_VERSION);
		Serial.print(F("CPU Frequency = "));
		Serial.print(F_CPU / 1000000);
		Serial.println(F(" MHz"));
		if (ITimer.setFrequency(frequency, callback))
		{
			Serial.print(F("Starting LaserReceiver ITimer OK, Frequency= "));
			Serial.print(frequency);
			Serial.println(F(" Hz"));
		}
		else
		{
			Serial.println(F("Can't set LaserReceiver ITimer correctly with Frequency= "));
			Serial.print(frequency);
			Serial.println(F(" Hz. Select another freq. or interval"));
		}
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
		int tmp;

		if (!recv_flag)
		{
			//tmp = !!(PIND & (1 << laser_receive_pin));		// direct pin access is better (quicker) than digitalRead at high frequencies
			tmp = digitalRead(laser_receive_pin);
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
			//rx_buffer[rx_bitnum] = !!(PIND & (1 << laser_receive_pin));
			rx_buffer[rx_bitnum] = digitalRead(laser_receive_pin);
			rx_bitnum++;
		}
		else
		{
			rx_bitnum = 0;
			recv_flag = 0;

			// Read data from the photodiode
			msg_raw = manchester_demodulate();
			msg_done = hamming_byte_decoder((msg_raw >> 8), (0xFF & msg_raw));

			can_print = 1;
		}
	}

	uint8_t printByte()
	{
		if (can_print)
		{
			Serial.print((char)msg_done);
			can_print = 0;
		}
		return (msg_done);
	}
	uint8_t getMsgCodeByte()
	{
		uint8_t result = msg_done;
		msg_done = 255;
		return (result);
	}
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External function prototypes -----------------------------------------------*/
#endif