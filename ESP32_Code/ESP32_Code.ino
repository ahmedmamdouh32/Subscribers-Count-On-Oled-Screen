#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Add this!
#include <Wire.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

long PreviousSubscribersCount = 0;
long CurrentSubscribersCount = 0;


long PreviousViewsCount = 0;
long CurrentViewsCount = 0;
const char* ssid = "MaMs";
const char* password = "shdoufa77777";

const char* serverName = "https://youtube.googleapis.com/youtube/v3/channels?part=statistics&forHandle=electronicshut&maxResults=0&key=AIzaSyBP_uQuLq1bJm-p_JEwAaIbnhnPtY871Ww";  // Change to your API

void Oled_init();
void oled_print(String Data);
void display_5_digits_centered(int number);
void display_number_size_6(int number);

void setup() {
  Oled_init();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  oled_print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    oled_print(".");
  }
  Serial.println("Connected!");
  oled_print("Connected!");

 
}


void loop() {
  makeGetRequest();

//  if (PreviousViewsCount != CurrentViewsCount) {
//    PreviousViewsCount = CurrentViewsCount;
//    display_5_digits_centered(CurrentViewsCount);
//  }
  display_5_digits_centered(CurrentViewsCount);
  delay(5000);
  display_5_digits_centered(CurrentSubscribersCount);
  delay(5000);

  
//  if (PreviousSubscribersCount != CurrentSubscribersCount) {
//    PreviousSubscribersCount = CurrentSubscribersCount;
//    display_5_digits_centered(CurrentSubscribersCount);
//  }
  
}





void makeGetRequest() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();  // Skip certificate validation (⚠️ only for testing)
    HTTPClient https;
    https.begin(client, serverName); // HTTPS URL
    int httpsResponseCode = https.GET();


    if (httpsResponseCode > 0) {
      String response = https.getString();
      Serial.println("Response:");
      Serial.println(response);

      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, response);
      if (!error) {
        const char* subscribers = doc["items"][0]["statistics"]["subscriberCount"];
        const char* views = doc["items"][0]["statistics"]["viewCount"];
        const char* videos = doc["items"][0]["statistics"]["videoCount"];

        CurrentSubscribersCount = atol(subscribers);
        CurrentViewsCount = atol(views);

        Serial.print("Subscribers: ");
        Serial.println(subscribers);
        Serial.print("Views: ");
        Serial.println(views);
        Serial.print("Videos: ");
        Serial.println(videos);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpsResponseCode);
      }
      https.end();
    }
    else {
      Serial.println("WiFi Disconnected");
//      WiFi.begin(ssid, password);
//      Serial.print("Connecting to WiFi..");
//      oled_print("Connecting to WiFi..");
//      while (WiFi.status() != WL_CONNECTED) {
//        delay(500);
//        Serial.print(".");
//        oled_print(".");
//      }
    }
  }

}

void Oled_init() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Address 0x3C for 128x64, SSD1306_SWITCHCAPVCC=generate display voltage from 3.3V internally
  display.clearDisplay(); //to remove the image dispalyed at begining
  display.setTextSize(6);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display(); // actually display all of the above
}


void display_number_size_6(int number) {
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 40);
  display.print(',');
  display.setTextSize(6);
  display.setCursor(20, 16);
  display.print(number);
  display.display();
}

void oled_print(String Data) {
  display.setTextSize(1);
  display.print(Data);
  display.display();
}

void display_5_digits_centered(int number) {
  display.clearDisplay();

  // Step 1: Format the number with comma
  String formatted;
  if (number >= 1000) {
    formatted += String(number / 1000);
    formatted += ',';

    int remainder = number % 1000;
    if (remainder < 100) formatted += '0';
    if (remainder < 10) formatted += '0';
    formatted += String(remainder);
  } else {
    formatted = String(number);
  }

  // Step 2: Estimate width
  int totalWidth = 0;
  for (int i = 0; i < formatted.length(); i++) {
    char c = formatted[i];
    if (c == ',') {
      totalWidth += 6 * 1; // comma at size 2
    } else {
      totalWidth += 6 * 4; // digits at size 4
    }
  }

  int startX = (128 - totalWidth) / 2;
  int y = (64 - 8 * 4) / 2; // vertically center size-4 text

  // Step 3: Draw character by character
  int x = startX;
  for (int i = 0; i < formatted.length(); i++) {
    char c = formatted[i];
    if (c == ',') {
      display.setTextSize(2);
      display.setCursor(x - 6, y + 16); // slight vertical adjust for smaller comma
      display.print(c);
      x += 6 * 1;
    } else {
      display.setTextSize(4);
      display.setCursor(x, y);
      display.print(c);
      x += 6 * 4;
    }
  }
  display.display();
}
