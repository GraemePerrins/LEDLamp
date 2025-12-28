// LED Lamp
//
//-------------------------------------------------------------
//
//
//  

#include <TimerOne.h>
#include <FastLED.h>
#include "LEDPatterns.h"


#define DEBUGLOG = 0;

const String LAMP_VERSION = "006";

// 47 * 60; // every 47 minutes change pattern
const unsigned int PATTERN_TIMER_SECS_COUNT_INTERVAL = 47 * 60; //47 * 60; // every 47 mins

// Push button 
const int PUSH_BTN_PIN = 3;
volatile unsigned int timerCounter = 0;
volatile bool changePatternFlag = false;
volatile bool btnPressedFlag = false;

// LED strip
const int LED_DATA_PIN = 6;
const int STRIP_NUM_LEDS = 60;
const int BRIGHTNESS = 160;

#define LED_TYPE WS2812B
#define COLOR_ORDER BRG



CRGB leds[STRIP_NUM_LEDS];

// Loop time control
unsigned int lastUpdate = 0;
const unsigned int updateIntervalMS = 20; // 20ms = 50 FPS



// This function is called automatically by the timer interrupt
void patternTimerCallback() {
  timerCounter++;
  if (timerCounter >= PATTERN_TIMER_SECS_COUNT_INTERVAL) 
  {
    timerCounter = 0;
    changePatternFlag = true;
  }
}

void initialisePatternTimer()
{
  // Initialize Timer1
  Timer1.initialize(1000000);  // Set timer period in microseconds (1 second = 1,000,000 µs) 
  Timer1.attachInterrupt(patternTimerCallback);
}


void initialiseSerial() {
#ifdef DEBUGLOG
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
    delay(250);
  }
#endif
}


// push button interrupt 
//
const unsigned long DEBOUNCE_TOLERANCE_MS = 60;
const unsigned long BTN_PRESS_MS = 350;



void buttonPressStateChanged() {
  static volatile bool firstPressedFlag = false;
  static volatile unsigned long btnFirstPressed = 0;

  const int pinLevel = digitalRead(PUSH_BTN_PIN);
  const unsigned long now = millis();

  if (pinLevel == HIGH) {
    if (firstPressedFlag == false) {
      firstPressedFlag = true;
      btnFirstPressed = now;
      return;
    }
  }
   
  if (pinLevel == LOW) {
    if (firstPressedFlag == true) {
      if ((now - btnFirstPressed) < DEBOUNCE_TOLERANCE_MS) {
        firstPressedFlag = false;
        return;
      }
      if ((now - btnFirstPressed) >= BTN_PRESS_MS) {
        btnPressedFlag = true;
      }
      firstPressedFlag = false;
      return;
    }
  }
}


void initialiseButtonSwitch() {
  btnPressedFlag = false;
  pinMode(PUSH_BTN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_BTN_PIN), buttonPressStateChanged, CHANGE);
}


void initialiseLEDStrip() {
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds,STRIP_NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);
  FastLED.show();
}


void setup() {
  // delay so when attached IDE doesnt trigger second reset of board
  delay(1000);
  initialiseSerial();
  initialiseLEDStrip();

  // _testLedStripColorEncoding();
  
  initialisePatterns(STRIP_NUM_LEDS);
  noInterrupts();
  initialisePatternTimer();
  initialiseButtonSwitch();
  interrupts();
  Serial.println("Initialised Version: " + LAMP_VERSION);
  delay(3000);
}


bool checkUpdateTime() 
{
  unsigned int nowMS = millis();
  if ((nowMS - lastUpdate) >= updateIntervalMS) {
    lastUpdate = nowMS;
    return true;
  }
  return false;
}


void _testLedStripColorEncoding() {
  Serial.println("show red");
  fill_solid(leds, STRIP_NUM_LEDS, CRGB::Red);  
  FastLED.show();
  delay(1000*3);
  fill_solid(leds, STRIP_NUM_LEDS, CRGB::Green);  
  Serial.println("show green");
  FastLED.show();
  delay(1000*3);
  fill_solid(leds, STRIP_NUM_LEDS, CRGB::Blue);  
  Serial.println("show blue");
  FastLED.show();
  delay(1000*3);
}



void loop() {
 
  if (btnPressedFlag == true) {
    Serial.println("button pattern change...");
    noInterrupts();
    changePatternFlag = true;
    btnPressedFlag = false;
    timerCounter = 0;
    interrupts();
  }

  if (changePatternFlag == true) {
    Serial.println("pattern change...");
    changePattern();
    changePatternFlag = false;
  }

  if (checkUpdateTime()) {
      patternCycle();
  }

}

