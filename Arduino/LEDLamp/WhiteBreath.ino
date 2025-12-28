// Pattern.ino - Breathing White Effect
// Uses FastLED library with HSV colors

#include <FastLED.h>

// Global constants (prefixed with _PAT7_)
const uint8_t _PAT7_MIN_BRIGHTNESS = 128;  // 50% intensity starting point
const uint8_t _PAT7_MAX_BRIGHTNESS = 255;  // 100% intensity peak

// Global variables (prefixed with _pat7)
int _pat7NumLeds = 0;
unsigned long _pat7CycleDurationMs = 0;
unsigned long _pat7HalfCycleDurationMs = 0;
unsigned long _pat7ElapsedMs = 0;
uint8_t _pat7CurrentBrightness = _PAT7_MIN_BRIGHTNESS;
bool _pat7Increasing = true;

extern CRGB leds[];  // Assumes leds array is defined in main sketch

/**
 * Initialize the pattern with the number of LEDs and cycle duration
 * @param numLeds Number of LEDs in the strip
 * @param cycleDurationSeconds Total duration for one complete cycle (grow + shrink) in seconds
 */
void WhiteBreathInit(int numLeds, int cycleDurationSeconds) {
  _pat7NumLeds = numLeds;
  _pat7CycleDurationMs = cycleDurationSeconds * 1000UL;
  _pat7HalfCycleDurationMs = _pat7CycleDurationMs / 2;
  _pat7ElapsedMs = 0;
  _pat7CurrentBrightness = _PAT7_MIN_BRIGHTNESS;
  _pat7Increasing = true;
}


/**
 * Advance the pattern by one cycle (called every 20ms)
 */
void WhiteBreathCycle() {
  // Increment elapsed time
  _pat7ElapsedMs += 20;
  
  // Calculate progress within the current half-cycle (0.0 to 1.0)
  float progress = (float)(_pat7ElapsedMs % _pat7HalfCycleDurationMs) / (float)_pat7HalfCycleDurationMs;
  
  // Determine if we're in the increasing or decreasing phase
  unsigned long cyclePosition = _pat7ElapsedMs % _pat7CycleDurationMs;
  _pat7Increasing = (cyclePosition < _pat7HalfCycleDurationMs);
  
  // Calculate brightness using smooth sine wave interpolation
  float brightnessRange = _PAT7_MAX_BRIGHTNESS - _PAT7_MIN_BRIGHTNESS;
  
  if (_pat7Increasing) {
    // Growing phase: use sine for smooth easing
    float sineProgress = sin(progress * PI / 2.0);  // 0 to PI/2 gives 0 to 1
    _pat7CurrentBrightness = _PAT7_MIN_BRIGHTNESS + (brightnessRange * sineProgress);
  } else {
    // Shrinking phase: use cosine for smooth easing
    float cosineProgress = cos(progress * PI / 2.0);  // 0 to PI/2 gives 1 to 0
    _pat7CurrentBrightness = _PAT7_MIN_BRIGHTNESS + (brightnessRange * cosineProgress);
  }
  
  // Create white color using HSV (Hue=0, Saturation=0 for white, Value=brightness)
  CHSV whiteColor(0, 0, _pat7CurrentBrightness);
  
  // Apply color to all LEDs
  for (int i = 0; i < _pat7NumLeds; i++) {
    leds[i] = whiteColor;
  }
  
  FastLED.show();
}
