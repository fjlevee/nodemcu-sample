#include <Arduino.h>
#include <RGB_Led_Tools.h>

// Define RGB DIOD LED Pins
#define RGB_DIOD_RED_PIN D2
#define RGB_DIOD_GREEN_PIN D3
#define RGB_DIOD_BLUE_PIN D4


RGBLed rgbLed(RGB_DIOD_RED_PIN, RGB_DIOD_GREEN_PIN, RGB_DIOD_BLUE_PIN);

void ICACHE_RAM_ATTR BlinkFunction()
{
  //Serial.println("BlinkFunction: ");
  //Serial.println(millis());
  rgbLed.blink();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("");
  Serial.println("Starting Program");

  Serial.println(F("End setup"));
}


void modify_leg_with_pointer(RGBLed* led){
  // Set Regular color To magenta
  led->get_regular_color()->set_8bits_colors(255,0,255);
  Serial.println("modify_leg_with_pointer display_regular_color (MAGENTA)");
  led->display_regular_color();
  delay(2000);
  // Set Regular color To cyan
  led->get_regular_color()->set_8bits_colors(0, 255, 255);
}

void loop()
{
  Serial.println("loop--------------------------------------");
  delay(2000);


  
  
  /*
  Serial.println("display_colors_rotation");
  rgbLed.display_colors_rotation();
  delay(2000);*/
  
  Serial.println("modify_leg_with_pointer");
  modify_leg_with_pointer(&rgbLed);

  // Color should be cyan
  Serial.println("display_regular_color (CYAN)");
  rgbLed.display_regular_color();
  delay(2000);

  /*
  // Try the blink
  Serial.println("Starting blinking led");
  rgbLed.get_blink_color_1()->set_8bits_colors(255,0,0);
  rgbLed.get_blink_color_2()->set_8bits_colors(0, 0, 255);

  //rgbLed.start_blinking(BlinkFunction,200);
  //delay(10000);

  //Serial.println("Stopping blinking led");
  //rgbLed.stop_blinking();
  ///delay(10000);
  */

  rgbLed.get_blink_color_1()->set_8bits_colors(255, 0, 0);
  rgbLed.get_blink_color_2()->set_8bits_colors(0, 0, 255);
  
  Serial.println("Start blinking");
  rgbLed.start_blinking(BlinkFunction, 8);
  delay(5000);
  Serial.println("Stop blinking");
  rgbLed.stop_blinking();
  delay(5000);
  Serial.println("Start linking");
  rgbLed.start_blinking(BlinkFunction, 8);
  delay(5000);
  Serial.println("Stop linking");
  rgbLed.stop_blinking();
}

