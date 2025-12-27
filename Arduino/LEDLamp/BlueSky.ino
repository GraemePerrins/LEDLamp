#include <FastLED.h>
#include "ColourWave.h"

// External LED array (defined in main sketch)
extern CRGB leds[];
// Pattern state variables
int _pat2NumLedsGlobal = 0;
float _pat2Progress = 0.0;  // 0.0 to 1.0 for complete transition
float _pat2IncrementPerCall = 0.0;

// Timing constants
const float _PAT2_CALL_INTERVAL_MS = 20.0;  // Called every 20ms
const float _PAT2_DEFAULT_TRANSITION_TIME_MS = 120000.0;  // 120 seconds default

// Color spectrum HSV values
// Black: HSV(any, any, 0)
// Dark Deep Blue: HSV(170, 255, 80)
// Sky Blue: HSV(150, 180, 255)
const uint8_t _PAT2_DEEP_BLUE_HUE = 170;
const uint8_t _PAT2_SKY_BLUE_HUE = 150;

void BlueSkyInit(int numLeds, float transitionTimeSeconds = 120.0) {
  _pat2NumLedsGlobal = numLeds;
  _pat2Progress = 0.0;
  
  // Calculate increment per call based on transition time
  float transitionTimeMs = transitionTimeSeconds * 1000.0;
  _pat2IncrementPerCall = _PAT2_CALL_INTERVAL_MS / transitionTimeMs;
}

void BlueSkyCycle() {
  // Update progress
  _pat2Progress += _pat2IncrementPerCall;
  
  // Loop back to start after reaching 2.0 (1.0 transition + 1.0 hold)
  if (_pat2Progress >= 2.0) {
    _pat2Progress = 0.0;
  }
  
  // Calculate current color based on progress
  uint8_t hue, sat, val;
  
  if (_pat2Progress < 1.0) {
    // Transition phase: 0.0 to 1.0
    if (_pat2Progress < 0.5) {
      // First half: Black to Dark Deep Blue (0.0 to 0.5)
      float t = _pat2Progress * 2.0;  // Map to 0.0 - 1.0
      
      hue = _PAT2_DEEP_BLUE_HUE;
      sat = 255;
      val = (uint8_t)(80.0 * t);  // 0 to 80
    } else {
      // Second half: Dark Deep Blue to Sky Blue (0.5 to 1.0)
      float t = (_pat2Progress - 0.5) * 2.0;  // Map to 0.0 - 1.0
      
      // Interpolate hue from deep blue to sky blue
      hue = _PAT2_DEEP_BLUE_HUE + (uint8_t)((float)(_PAT2_SKY_BLUE_HUE - _PAT2_DEEP_BLUE_HUE) * t);
      
      // Interpolate saturation from 255 to 180
      sat = 255 - (uint8_t)(75.0 * t);  // 255 to 180
      
      // Interpolate value from 80 to 255
      val = 80 + (uint8_t)(175.0 * t);  // 80 to 255
    }
  } else {
    // Hold phase: 1.0 to 2.0 - stay at sky blue
    hue = _PAT2_SKY_BLUE_HUE;
    sat = 180;
    val = 255;
  }
  
  // Apply the same color to all LEDs
  for (int i = 0; i < _pat2NumLedsGlobal; i++) {
    leds[i] = CHSV(hue, sat, val);
  }
  
  FastLED.show();
}