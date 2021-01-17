#include <Arduino.h>

#define LED_PIN_0 D0
#define LED_PIN_1 D1
#define LED_PIN_2 D2
#define LED_PIN_3 D3
#define LED_PIN_4 D4
#define LED_PIN_5 D5
#define LED_PIN_6 D6
#define LED_PIN_7 D7
#define LED_PIN_8 D8

#define DELAY_TIME 300
#define PINS_NUMBER 9

// the setup function runs once when you press reset or power the board
void setup()
{
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_PIN_0, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
  pinMode(LED_PIN_6, OUTPUT);
  pinMode(LED_PIN_7, OUTPUT);
  pinMode(LED_PIN_8, OUTPUT);
  Serial.println("OK Program Started");
}
int i = 0;

// the loop function runs over and over again forever
void loop()
{
  digitalWrite(LED_PIN_0, LOW);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);
  digitalWrite(LED_PIN_5, LOW);
  digitalWrite(LED_PIN_6, LOW);
  digitalWrite(LED_PIN_7, LOW);
  digitalWrite(LED_PIN_8, LOW);

  switch (i % PINS_NUMBER)
  {
  case 0:
    digitalWrite(LED_PIN_0, HIGH);
    break;
  case 1:
    digitalWrite(LED_PIN_1, HIGH);
    break;
  case 2:
    digitalWrite(LED_PIN_2, HIGH);
    break;
  case 3:
    digitalWrite(LED_PIN_3, HIGH);
    break;
  case 4:
    digitalWrite(LED_PIN_4, HIGH);
    break;
  case 5:
    digitalWrite(LED_PIN_5, HIGH);
    break;
  case 6:
    digitalWrite(LED_PIN_6, HIGH);
    break;
  case 7:
    digitalWrite(LED_PIN_7, HIGH);
    break;
  case 8:
    digitalWrite(LED_PIN_8, HIGH);
    break;
  }
  i++;
  if (i == PINS_NUMBER)
    i = 0;
  delay(DELAY_TIME);
}