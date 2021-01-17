#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define LCD_SCL_PIN D3
#define LCD_SD4_PIN D4

#define ACTIVE_BUZZER_PIN D8



void setup()
{
  Serial.begin(9600);

  pinMode(ACTIVE_BUZZER_PIN, OUTPUT);
  Wire.begin(LCD_SD4_PIN, LCD_SCL_PIN);
  lcd.init();                    // initializing the LCD
  lcd.setCursor(0, 0);           //DP0 of Topline
  lcd.backlight();               // Enable or Turn On the backlight
  lcd.print("Salut FJ LEVEE");   // Start Printing
  lcd.setCursor(0, 1);           //DP0 of Topline
  lcd.print("You are great!!!"); // Start Printing
  Serial.print("D3: ");
  Serial.println(D3);
  Serial.print("D4: ");
  Serial.println(D4);
  Serial.println("OK Program Started");

}

void loop()
{
  //lcd.backlight();
  digitalWrite(ACTIVE_BUZZER_PIN, HIGH);
  delay(1000);
  //lcd.noBacklight();
  digitalWrite(ACTIVE_BUZZER_PIN, LOW);
  delay(1000);
}