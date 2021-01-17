
// Define RGB DIOD LED Pins
#define RGB_DIOD_BLUE_PIN D0
#define RGB_DIOD_GREEN_PIN D1
#define RGB_DIOD_RED_PIN D2
#define RGB_MAX_VALUE 1023

int led_red_value=RGB_MAX_VALUE;
int led_green_value=RGB_MAX_VALUE;
int led_blue_value=RGB_MAX_VALUE;

void setup(){
   // Set led
  pinMode(RGB_DIOD_RED_PIN, OUTPUT);
  pinMode(RGB_DIOD_GREEN_PIN, OUTPUT);
  pinMode(RGB_DIOD_BLUE_PIN, OUTPUT);
  // WHITE
  display_led_color(led_red_value,led_green_value,led_blue_value);
}


void loop(){
  display_led_color(led_red_value,led_green_value,led_blue_value);
  delay(2000);
  display_led_color(0,0,0);
  delay(2000);
  
  display_led_colors();
  delay(2000);
}

void display_led_color(int r, int g, int b){
  analogWrite(RGB_DIOD_RED_PIN, RGB_MAX_VALUE - r);
  analogWrite(RGB_DIOD_GREEN_PIN, RGB_MAX_VALUE - g);
  analogWrite(RGB_DIOD_BLUE_PIN, RGB_MAX_VALUE - b);

}

void display_led_colors(){
  // RED
  display_led_color(RGB_MAX_VALUE,0,0);
  delay(2000);
  // GREEN
  display_led_color(0,RGB_MAX_VALUE,0);
  delay(2000);
  // BLUE
  display_led_color(0,0,RGB_MAX_VALUE);
  delay(2000);
  // YELLOW
  display_led_color(RGB_MAX_VALUE,RGB_MAX_VALUE,0);
  delay(2000);
  // MAGENTA
  display_led_color(RGB_MAX_VALUE,0,RGB_MAX_VALUE);
  delay(2000);
  // CYAN
  display_led_color(0,RGB_MAX_VALUE,RGB_MAX_VALUE);
  delay(2000);
  // WHITE
  display_led_color(RGB_MAX_VALUE,RGB_MAX_VALUE,RGB_MAX_VALUE);
  delay(2000);
  // ORANGE
  display_led_color(RGB_MAX_VALUE,RGB_MAX_VALUE/2,0);
  delay(2000);
  // BLACK
  display_led_color(0,0,0);
  delay(2000);
}
