#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

WiFiMulti wifiMulti;

const uint64_t kInterruptIntervalMinutes = 30;
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;

const size_t kCapacity = JSON_ARRAY_SIZE(5) + 6*JSON_OBJECT_SIZE(2) + 630;
DynamicJsonDocument doc(kCapacity);

const String kHeadlineUrl = "http://news-headline-server.glitch.me/publications/CAAqBwgKMOLs8Aowhd27Ag/headlines";
const uint16_t kTimeOut = 60000;
const String kHeadlineSpace = "       ";
String headlineStr = "";

int32_t cursorX = 0;

void IRAM_ATTR onTimer() {
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

void extractHeadlines(String payload) {
  deserializeJson(doc, payload);
  JsonArray headlines = doc["headlines"];
  headlineStr = "";
  for (int i=0; i<headlines.size(); ++i) {
    String headline = headlines[i]["headline"];
    headlineStr += headline;
    if (i < headlines.size() - 1) {
      headlineStr += kHeadlineSpace;
    }
  }
}

void getHeadlines() {
  if ((wifiMulti.run() == WL_CONNECTED)) { // check for wifi connection
    
    HTTPClient httpClient;
    httpClient.begin(kHeadlineUrl);
    httpClient.setTimeout(kTimeOut);
    int httpCode = httpClient.GET();
    if (httpCode > 0) {

      if (httpCode == HTTP_CODE_OK) {
        String payload = httpClient.getString();
        extractHeadlines(payload);
      } else {
        M5.Lcd.println("http code: " + httpCode);
      }
      
    } else {
      M5.Lcd.println("failed to http connect to webpage");
      M5.Lcd.println(httpClient.errorToString(httpCode).c_str());
    }
    
  } else {
    M5.Lcd.println("failed to connect to wifi");
  }
}

void setup() {
  // Initialize the M5StickC object
  M5.begin();

  // Set up screen
  M5.Axp.ScreenBreath(8);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(2);

  cursorX = M5.Lcd.width();

  // Set up wifi
  wifiMulti.addAP("your_wifi_id", "your_wifi_password");
  getHeadlines();

  // Set up timer
  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, kInterruptIntervalMinutes * 60 * 1000000, true);
  timerAlarmEnable(timer);
}

void loop() {
  // Timer has fired
  if (cursorX == M5.Lcd.width() && xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    getHeadlines();
  }
  
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.drawString(headlineStr, cursorX, (M5.Lcd.height() - M5.Lcd.fontHeight()) / 2);
  cursorX -= 7;
  if (cursorX <= -M5.Lcd.textWidth(headlineStr)) {
    cursorX = M5.Lcd.width();
  }
  delay(100);
}
