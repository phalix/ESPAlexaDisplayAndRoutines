/**
Thanks to https://thingpulse.com for the great OLED Library
*/

// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h"
// or #include "SH1106Wire.h", legacy include: `#include "SH1106.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"

// Include the UI lib
#include "OLEDDisplayUi.h"


#include <WiFi.h>
#include <HTTPClient.h>

// Use the corresponding display class:

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

// Initialize the OLED display using Wire library
SSD1306 display (0x3c, 5, 4);
// SH1106Wire display(0x3c, SDA, SCL);

OLEDDisplayUi ui     ( &display );


const char* ssid = "EnterSSID";
const char* password = "EnterWIFIPW";
void initWiFi() {
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to WiFi ..");
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    display.clear();
    display.drawString(0, 0, "Connecting to WiFi .."+String(millis()));
    display.drawString(0, 10, String(millis()));
    display.display();
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  
}
int httpResponseCode = 0;

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  //display->drawString(128, 0, String(millis()));
  display->drawString(128, 0, String(WiFi.RSSI()));
  display->drawString(128, 10, String(httpResponseCode));
  
  
  
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the center of the text
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "Fort Knox");  
  
  
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the center of the text
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "Guten Morgen");
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the center of the text
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "Lampen An");

}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the center of the text
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "Lampen Aus");
}

void drawFrame5(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, drawFrame4/*, drawFrame5*/ };

// how many frames are there?
int frameCount = 4;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

const uint8_t activeSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B01000010,
    B01000010,
    B00100100,
    B00011000
};

const uint8_t inactiveSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000
};

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  
  
  pinMode(36, INPUT_PULLDOWN);
  pinMode(34, INPUT_PULLDOWN);
  pinMode(33, INPUT_PULLDOWN);
  pinMode(26, INPUT_PULLDOWN);

  
  
  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(60);

  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  //display.flipScreenVertically();
  ui.disableAutoTransition();
  initWiFi();
  
}

int state = 0;

void loop() {
  
  
  int remainingTimeBudget = ui.update();
  
  if (remainingTimeBudget > 0) {
    Serial.println(WiFi.status());
    int buttonState36 = digitalRead(36);
    int buttonState34 = digitalRead(34);
    if(buttonState36>0){
      ui.nextFrame();
    }else if(buttonState34>0){
      ui.previousFrame();
    }
    int a = ui.getUiState()->currentFrame;
    int buttonState33 = digitalRead(33);
    if(buttonState33>0){
      httpResponseCode = 0;
      if(WiFi.status() == WL_CONNECTED){
        HTTPClient http;
        String serverPath;
        switch(a){
          case 0:
          serverPath = "EnterAGetRequest";
          break;
          case 1:
          serverPath = "EnterAGetRequest";
          break;
          case 2:
          serverPath = "EnterAGetRequest";
          break;
          case 3:
          serverPath = "EnterAGetRequest";
          break;
              
        }
        
        http.begin(serverPath.c_str());
        httpResponseCode = http.GET(); 
      }else{
        WiFi.reconnect();  
      }
       
    }
    
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
  }
}
