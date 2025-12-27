#include <FastLED.h>
#include "PinkPurpleCycle.h"

// External LED array (defined in main sketch)
extern CRGB leds[];

// Pattern state variables
float _pat1Offset = 0.0;
float _pat1Direction = 1.0;
int _pat1NumLedsGlobal = 0;

// Timing constants
const float _PAT1_CYCLE_TIME_MS = 30000.0;  // 20 seconds for full oscillation
const float _PAT1_CALL_INTERVAL_MS = 20.0;  // Called every 20ms
const float _PAT1_INCREMENT_PER_CALL = (60.0 / _PAT1_CYCLE_TIME_MS) * _PAT1_CALL_INTERVAL_MS;

// Color spectrum HSV values
// Deep Pink: HSV(330, 255, 200)
// Dark Purple: HSV(270, 255, 180)
// Blue: HSV(160, 255, 200)
const uint8_t _PAT1_COLOR_START_HUE = 330;  // Deep pink
const uint8_t _PAT1_COLOR_MID_HUE = 270;    // Dark purple
const uint8_t _PAT1_COLOR_END_HUE = 160;    // Blue

void PinkPurpleCyclePatternInit(int numLeds) {
  _pat1NumLedsGlobal = numLeds;
  _pat1Offset = 0.0;
  _pat1Direction = 1.0;
}

void PinkPurpleCycle() {
  // Update offset position
  _pat1Offset += _PAT1_INCREMENT_PER_CALL * _pat1Direction;
  
  // Reverse direction at boundaries
  if (_pat1Offset >= 60.0) {
    _pat1Offset = 60.0;
    _pat1Direction = -1.0;
  } else if (_pat1Offset <= 0.0) {
    _pat1Offset = 0.0;
    _pat1Direction = 1.0;
  }
  
  // Apply colors to each LED
  for (int i = 0; i < _pat1NumLedsGlobal; i++) {
    // Calculate position in the color gradient (0.0 to 60.0)
    float gradientPos = i + _pat1Offset;
    
    // Wrap the gradient position to create the oscillating effect
    // Map to 0.0 - 1.0 range for color interpolation
    float colorProgress = gradientPos / 60.0;
    
    // Keep within bounds
    if (colorProgress > 1.0) colorProgress = 1.0;
    if (colorProgress < 0.0) colorProgress = 0.0;
    
    uint8_t hue, sat, val;
    
    // Interpolate through the three colors
    if (colorProgress < 0.5) {
      // Deep pink to dark purple (first half)
      float t = colorProgress * 2.0;  // 0.0 to 1.0
      hue = _PAT1_COLOR_START_HUE + (_PAT1_COLOR_MID_HUE - _PAT1_COLOR_START_HUE + 256) % 256 * t;
      sat = 255;
      val = 200 - 20 * t;  // 200 to 180
    } else {
      // Dark purple to blue (second half)
      float t = (colorProgress - 0.5) * 2.0;  // 0.0 to 1.0
      hue = _PAT1_COLOR_MID_HUE + (_PAT1_COLOR_END_HUE - _PAT1_COLOR_MID_HUE + 256) % 256 * t;
      sat = 255;
      val = 180 + 20 * t;  // 180 to 200
    }
    
    leds[i] = CHSV(hue, sat, val);
  }
  
  FastLED.show();
}