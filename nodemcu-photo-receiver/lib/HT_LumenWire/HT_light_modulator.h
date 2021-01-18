/**   
 ******************************************************************************   
 * @file    Light_modulator.h
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
#ifndef Light_modulator_H
#define Light_modulator_H
/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEFAULT_LIGHT_SEND_PIN 6
#define DEFAULT_LIGHT_RECEIVE_PIN 7
/* Private typedef -----------------------------------------------------------*/
class HT_PhotoTransmitter
{

	uint8_t tx_buffer[44];
	uint8_t tx_bitnum; // bit number to be transmitted
	int send_flag;		 // light send flag
	int light_send_pin = DEFAULT_LIGHT_SEND_PIN;

public:
	void set_light_send_pin(int custom_light_send_pin)
	{
		light_send_pin = custom_light_send_pin;
	}
	void begin()
	{
		tx_bitnum = 0; // bit number to be transmitted
		send_flag = 0; // light send flag
		//set light pin as output
		pinMode(light_send_pin, OUTPUT);
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
			digitalWrite(light_send_pin, tx_buffer[tx_bitnum]);

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

class HT_PhotoReceiver
{
	uint8_t rx_buffer[44];
	uint8_t rx_bitnum; // number of bits received
	int recv_flag;		 // light receive flag
	uint16_t msg_raw;
	uint8_t msg_done;
	uint8_t can_print;
	int light_receive_pin = DEFAULT_LIGHT_RECEIVE_PIN;

public:
	void set_light_receive_pin(int custom_light_receive_pin)
	{
		light_receive_pin = custom_light_receive_pin;
	}

	void begin()
	{
		rx_bitnum = 0; // bit number to be transmitted
		recv_flag = 0; // light send flag
		can_print = 0;

		//set photodiode pin as input
		pinMode(light_receive_pin, INPUT);
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
			//tmp = !!(PIND & (1 << light_receive_pin));		// direct pin access is better (quicker) than digitalRead at high frequencies
			tmp = digitalRead(light_receive_pin);
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
			//rx_buffer[rx_bitnum] = !!(PIND & (1 << light_receive_pin));
			rx_buffer[rx_bitnum] = digitalRead(light_receive_pin);
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