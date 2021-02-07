#include "RGB_Led_Tools.h"

// Init ESP8266 timer 0
ESP8266Timer blinkTimer;

int convert_rgb_8bits_to_10bits_color(int c)
{
  if (c == 255)
  {
    return RGB_MAX_VALUE;
  }
  else
  {
    return c * 4;
  }
}

RGBLed::RGBLed(int red_pin = DEFAULT_RGB_DIOD_RED_PIN, int green_pin = DEFAULT_RGB_DIOD_GREEN_PIN, int blue_pin = DEFAULT_RGB_DIOD_BLUE_PIN)
{
  Serial.println("RGB LEG Constructor");
  _red_pin = red_pin;
  _green_pin = green_pin;
  _blue_pin = blue_pin;
  // Set led pin mode to output.
  pinMode(_red_pin, OUTPUT);
  pinMode(_green_pin, OUTPUT);
  pinMode(_blue_pin, OUTPUT);
}

void RGBLed::display_regular_color()
{
  display_color(&_regular_color);
}
void RGBLed::display_blink_color_1()
{
  display_color(&_blink_color_1);
}
void RGBLed::display_blink_color_2()
{
  display_color(&_blink_color_2);
}
void RGBLed::display_color(RGBColor *color)
{
  analogWrite(_red_pin, RGB_MAX_VALUE - color->get_red_value());
  analogWrite(_green_pin, RGB_MAX_VALUE - color->get_green_value());
  analogWrite(_blue_pin, RGB_MAX_VALUE - color->get_blue_value());
}

void RGBLed::display_color(int r, int g, int b)
{
  analogWrite(_red_pin, RGB_MAX_VALUE - r);
  analogWrite(_green_pin, RGB_MAX_VALUE - g);
  analogWrite(_blue_pin, RGB_MAX_VALUE - b);
}

void RGBLed::display_colors_rotation()
{
  // RED
  display_color(RGB_MAX_VALUE, 0, 0);
  delay(200);
  // GREEN
  display_color(0, RGB_MAX_VALUE, 0);
  delay(200);
  // BLUE
  display_color(0, 0, RGB_MAX_VALUE);
  delay(200);
  // YELLOW
  display_color(RGB_MAX_VALUE, RGB_MAX_VALUE, 0);
  delay(200);
  // MAGENTA
  display_color(RGB_MAX_VALUE, 0, RGB_MAX_VALUE);
  delay(200);
  // CYAN
  display_color(0, RGB_MAX_VALUE, RGB_MAX_VALUE);
  delay(200);
  // WHITE
  display_color(RGB_MAX_VALUE, RGB_MAX_VALUE, RGB_MAX_VALUE);
  delay(200);
  // ORANGE
  display_color(RGB_MAX_VALUE, RGB_MAX_VALUE / 2, 0);
  delay(200);
  // BLACK
  display_color(0, 0, 0);
  delay(200);
}
void RGBLed::blink(){
  if (blink_value % 2 == 0)
  {
    display_blink_color_1();
  }
  else
  {
    display_blink_color_2();
  }
  blink_value++;
  if (blink_value == 2)
    blink_value = 0;
}

void RGBLed::init_blinking_timer(timer_callback blinkMethod, float frequency = DEFAULT_BLINK_FREQUENCY_HZ)
{
  Serial.println(F("RGBLed::init_blinking_timer"));
  if (blinkTimer.setFrequency(frequency, blinkMethod))
  {
    Serial.print(F("Starting blinkTimer OK, Frequency= "));
    Serial.print(frequency);
    Serial.println(F(" Hz"));
  }
  else
  {
    Serial.println(F("Can't set blinkTimer correctly with Frequency= "));
    Serial.print(frequency);
    Serial.println(F(" Hz. Select another freq. or interval"));
  }
}

void RGBLed::start_blinking(timer_callback blinkMethod, float frequency = DEFAULT_BLINK_FREQUENCY_HZ)
{
  Serial.println(F("RGBLed::start_blinking"));
  blink_value = 0;
  if (blinkTimer.setFrequency(frequency, blinkMethod))
  {
    Serial.print(F("Starting blinkTimer OK, Frequency= "));
    Serial.print(frequency);
    Serial.println(F(" Hz"));
  }
  else
  {
    Serial.println(F("Can't set blinkTimer correctly with Frequency= "));
    Serial.print(frequency);
    Serial.println(F(" Hz. Select another freq. or interval"));
  }
}
void RGBLed::stop_blinking(){
  Serial.println(F("RGBLed::stop_blinking"));
  blink_value = 0;
  blinkTimer.stopTimer();
}

