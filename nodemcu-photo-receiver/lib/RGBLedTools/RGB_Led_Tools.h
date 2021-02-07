#ifndef RGB_Led_Tools_H
#define RGB_Led_Tools_H
#include <Arduino.h>
// These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include <ESP8266TimerInterrupt.h>

// Default Blink Frequency in HZ
#define DEFAULT_BLINK_FREQUENCY_HZ 2.0
#define RGB_MAX_VALUE 1023

// Define DEFAULT RGB DIOD LED Pins
#define DEFAULT_RGB_DIOD_RED_PIN D0
#define DEFAULT_RGB_DIOD_GREEN_PIN D1
#define DEFAULT_RGB_DIOD_BLUE_PIN D2




int convert_rgb_8bits_to_10bits_color(int c);



class RGBColor
{
  int _red_value = 0;
  int _green_value = 0;
  int _blue_value = 0;

public:
  RGBColor(int red_value = 0, int green_value = 0, int blue_value = 0){
    _red_value = red_value;
    _green_value = green_value;
    _blue_value = blue_value;
  }


  int get_red_value()
  {
  return _red_value;
  };
  int get_green_value()
  {
    return _green_value;
  };
  int get_blue_value()
  {
    return _blue_value;
  };
  void set_red_value(int red_value)
  {
    _red_value = red_value;
  };
  void set_green_value(int green_value)
  {
    _green_value = green_value;
  };
  void set_blue_value(int blue_value)
  {
    _blue_value = blue_value;
  };
  void set_8bits_colors(int red_8bits, int green_8bits, int blue_8bits){
    _red_value = convert_rgb_8bits_to_10bits_color(red_8bits);
    _green_value = convert_rgb_8bits_to_10bits_color(green_8bits);
    _blue_value = convert_rgb_8bits_to_10bits_color(blue_8bits);
  }
  void set_colors(int red_value, int green_value, int blue_value)
  {
    _red_value = red_value;
    _green_value = green_value;
    _blue_value = blue_value;
  }
};

class RGBLed
{
  RGBColor _regular_color;
  RGBColor _blink_color_1;
  RGBColor _blink_color_2;
  int blink_value = 0;
  int _red_pin;
  int _green_pin;
  int _blue_pin;

public :
  RGBLed(int red_pin , int green_pin , int blue_pin );
  //~RGBLed();

  void init_blinking_timer(timer_callback blinkMethod, float frequency);

  RGBColor* get_regular_color(){
    return &_regular_color;
  }
  RGBColor* get_blink_color_1()
  {
    return &_blink_color_1;
  }
  RGBColor* get_blink_color_2()
  {
    return &_blink_color_2;
  }

  void display_regular_color();
  void display_blink_color_1();
  void display_blink_color_2();
  void display_color(RGBColor* color);
  void display_color(int r, int g, int b);
  void display_colors_rotation();

  void blink();
  void start_blinking(timer_callback blinkMethod, float frequency);
  void stop_blinking();
};





#endif
