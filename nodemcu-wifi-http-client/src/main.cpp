

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Program_config.h"

ESP8266WiFiMulti WiFiMulti;
WiFiClient client;
DynamicJsonDocument doc(256);
String id = "";
// Define RGB DIOD LED Pins
#define RGB_DIOD_BLUE_PIN D0
#define RGB_DIOD_GREEN_PIN D1
#define RGB_DIOD_RED_PIN D2
#define RGB_MAX_VALUE 1023

int led_red_value = RGB_MAX_VALUE;
int led_green_value = RGB_MAX_VALUE;
int led_blue_value = RGB_MAX_VALUE;

void display_led_color(int r, int g, int b)
{
  analogWrite(RGB_DIOD_RED_PIN, RGB_MAX_VALUE - r);
  analogWrite(RGB_DIOD_GREEN_PIN, RGB_MAX_VALUE - g);
  analogWrite(RGB_DIOD_BLUE_PIN, RGB_MAX_VALUE - b);
}
void initializeWiFi(){
  Serial.println("");
  Serial.println("");
  Serial.print("Init Wifi with mac address: ");
  Serial.println(WiFi.macAddress());

  for (uint8_t t = 4; t > 0; t--)
  {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void send_register()
{
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED))
  {

    HTTPClient http;

    Serial.print("[HTTP] send_register begin ...\n");
    http.begin(client, SERVER_API); //HTTP
    http.addHeader("Content-Type", "application/json");
    http.addHeader(API_AUTH_HEADER, API_AUTH_TOKEN);
    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.POST("{\"mac\":\"" + WiFi.macAddress() + "\"}");

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        String payload = http.getString();
        Serial.print("Payload received: ");
        Serial.println(payload);
        deserializeJson(doc, payload);
        JsonObject obj = doc.as<JsonObject>();
        // You can use a String to get an element of a JsonObject
        // No duplication is done.
        id = obj["id"].as<String>();
        Serial.print("Id: ");
        Serial.println(id);
        JsonObject led = obj["color"];
        led_red_value = (int)led[String("r")] * 4;
        led_green_value = (int)led[String("g")] * 4;
        led_blue_value = (int)led[String("b")] * 4;
        Serial.printf("R=%d, G=%d, B=%d\n", led_red_value, led_green_value, led_blue_value);
        display_led_color(led_red_value, led_green_value, led_blue_value);
      }
      else
      {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }
    else
    {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
}

void setup()
{

  Serial.begin(115200);
  //Serial.begin(9600);
  // Serial.setDebugOutput(true);
  initializeWiFi();
  send_register();
}




void loop()
{
  
}