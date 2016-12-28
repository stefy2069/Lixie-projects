/* -------------------------------------------------
   Data.Sparkfun.com Streamer
   using ESP8266 and Lixie Displays!

   by Connor Nishijima - 12/22/2016
   -------------------------------------------------

   To use your Lixie Displays / ESP8266 as a live
   streamer for a data.sparkfun.com stream, you'll
   need a few things:

   - WIFI_SSID
   - WIFI_PASSWORD
   - SPARKFUN_PUBLIC_KEY
   - SPARKFUN_DATA_FIELD

  SPARKFUN_PUBLIC_KEY is your 20-char public key for
  your Data.Soarkfun.com stream

  SPARKFUN_DATA_FIELD is the field of data on your
  stream you'd like to display

   -------------------------------------------------
*/

#include "Lixie.h" // Include Lixie Library
Lixie lix;         // Set class nickname for faster coding

#include <ESP8266WiFi.h>        // ESP8266 WIFI Lib
#include <ESP8266WiFiMulti.h>   // WifiMulti Lib for connection handling
#include <ESP8266HTTPClient.h>  // HTTPClient for web requests
#include <ArduinoJson.h>        // JSON Parser
ESP8266WiFiMulti WiFiMulti;

// This should be the index page of your Wordpress site!
char* WIFI_SSID = "";
char* WIFI_PASS = "";
String SPARKFUN_PUBLIC_KEY = "zDA9M8dQlahOqo4bx5Dd";
String SPARKFUN_DATA_FIELD = "luminosity";

void setup() {
  Serial.begin(115200);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS); // Your WIFI credentials
  lix.begin(); // Initialize LEDs

  // This sets all lights to yellow while we're connecting to WIFI
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    lix.color_on_rgb(255, 255, 0);
    lix.write_int(9999);
    delay(100);
  }

  // Green on connection success
  lix.color_on_rgb(0, 255, 0);
  lix.write_int(9999);
  delay(500);

  // Reset colors to default
  lix.color_on_rgb(255, 255, 255);
  lix.write_int(9999);
}

void loop() {
  checkSF(SPARKFUN_DATA_FIELD);
  delay(5000);
}

// FUNCTIONS ----------------------------------------

// Check latest data.sparkfun.com log
void checkSF(String data_field) {
  StaticJsonBuffer<400> jsonBuffer;      // JSON Parser setup
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin("http://data.sparkfun.com/output/" + SPARKFUN_PUBLIC_KEY + ".json?offset=0&limit=1");
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        JsonArray& sf_data = jsonBuffer.parseArray(payload);
        if (!sf_data.success()) {
          Serial.println("Parsing failed");
          return;
        }
        int field = sf_data[0][data_field];
        if(field < 0){
          field = 0;
        }
        lix.write_int(field);
        Serial.print("DATA: ");
        Serial.println(field);
      }
    }
    http.end();
  }
}