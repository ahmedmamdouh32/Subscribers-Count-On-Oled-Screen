#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Add this!
#include <Wire.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

const unsigned char epd_bitmap_eye[] PROGMEM = {
  0x00, 0x1f, 0xf8, 0x00, 0x00, 0xff, 0xff, 0x00, 0x07, 0xf0, 0x0f, 0xe0, 0x0f, 0xe0, 0x07, 0xf0,
  0x1f, 0xc0, 0x03, 0xf8, 0x3f, 0xc3, 0xc3, 0xfc, 0x7f, 0x87, 0xe1, 0xfe, 0xff, 0x87, 0xe1, 0xff,
  0xff, 0x87, 0xe1, 0xff, 0x7f, 0x87, 0xe1, 0xfe, 0x3f, 0xc3, 0xc3, 0xfc, 0x1f, 0xc0, 0x03, 0xf8,
  0x0f, 0xe0, 0x07, 0xf0, 0x07, 0xf0, 0x0f, 0xe0, 0x00, 0xff, 0xff, 0x00, 0x00, 0x1f, 0xf8, 0x00
};

const unsigned char epd_bitmap_user [] PROGMEM = {
  0x07, 0xe0, 0x0f, 0xf0, 0x0c, 0x30, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0e, 0x70,
  0x07, 0xe0, 0x1f, 0xf8, 0x38, 0x1c, 0x60, 0x06, 0x60, 0x06, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03
};



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
  display.clearDisplay();


}


void loop() {
  makeGetRequest();

  //  if (PreviousViewsCount != CurrentViewsCount) {
  //    PreviousViewsCount = CurrentViewsCount;
  //    display_5_digits_centered(CurrentViewsCount);
  //  }
  printEye();
  display_5_digits_centered(CurrentViewsCount);
  delay(5000);
  display.clearDisplay();
  printUser();
  display_5_digits_centered(CurrentSubscribersCount);
  delay(5000);
  display.clearDisplay();


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
  //display.clearDisplay();

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
  //  int y = (64 - 8 * 4) / 2; // vertically center size-4 text
  int y = 26 ;
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



void writeEye(unsigned char x, unsigned char y) {
  display.drawBitmap(x, y, epd_bitmap_eye, 32, 16, WHITE);
}
void writeUser(unsigned char x, unsigned char y) {
  display.drawBitmap(x, y, epd_bitmap_user, 16, 16, WHITE);
//  display.drawLine(x, y + 14, x + 15, y + 14, WHITE);
  display.drawLine(x, y + 15, x + 15, y + 15, WHITE);
}
void printEye(){
  writeEye(48,0);
}
void printUser(){
  writeUser(56,0);
}
