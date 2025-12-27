// Center fade saturation pattern for FastLED
// Random color with max saturation in center, fading to edges

// Global constants
const uint8_t _PAT5_MAX_SATURATION = 255;
const uint8_t _PAT5_MIN_SATURATION = 0;
const uint8_t _PAT5_BRIGHTNESS = 255;

// Global variables
int _pat5NumLeds = 0;
int _pat5CyclesPerColor = 0;
int _pat5CenterLength = 0;
int _pat5CurrentCycle = 0;
uint8_t _pat5CurrentHue = 0;
int _pat5CenterStart = 0;
int _pat5CenterEnd = 0;

// External LED array (defined in main sketch)
extern CRGB leds[];

void RandomColourInit(int numLeds, int cyclesPerColor, int centerLength) {
  _pat5NumLeds = numLeds;
  _pat5CyclesPerColor = cyclesPerColor;
  _pat5CenterLength = centerLength;
  _pat5CurrentCycle = 0;
  
  // Calculate center position
  int center = _pat5NumLeds / 2;
  
  // Calculate center region bounds (L LEDs either side of middle)
  _pat5CenterStart = center - _pat5CenterLength;
  _pat5CenterEnd = center + _pat5CenterLength;
  
  // Constrain to valid range
  _pat5CenterStart = constrain(_pat5CenterStart, 0, _pat5NumLeds - 1);
  _pat5CenterEnd = constrain(_pat5CenterEnd, 0, _pat5NumLeds - 1);
  
  // Generate initial random hue
  _pat5CurrentHue = random(0, 256);
}

void RandomColourCycle() {
  // Check if we need to generate a new random color
  if (_pat5CurrentCycle == 0) {
    _pat5CurrentHue = random(0, 256);
  }
  
  // Set colors for all LEDs
  for (int i = 0; i < _pat5NumLeds; i++) {
    uint8_t saturation;
    
    // Check if LED is in the center region (max saturation)
    if (i >= _pat5CenterStart && i <= _pat5CenterEnd) {
      saturation = _PAT5_MAX_SATURATION;
    }
    // LED is before center region (fade from start to center)
    else if (i < _pat5CenterStart) {
      // Calculate distance from start to center start
      int totalDistance = _pat5CenterStart;
      int currentDistance = i;
      
      // Calculate saturation gradient (0 at start, max at center)
      if (totalDistance > 0) {
        saturation = map(currentDistance, 0, totalDistance, 
                        _PAT5_MIN_SATURATION, _PAT5_MAX_SATURATION);
      } else {
        saturation = _PAT5_MAX_SATURATION;
      }
    }
    // LED is after center region (fade from center to end)
    else {
      // Calculate distance from center end to strip end
      int totalDistance = (_pat5NumLeds - 1) - _pat5CenterEnd;
      int currentDistance = i - _pat5CenterEnd;
      
      // Calculate saturation gradient (max at center, 0 at end)
      if (totalDistance > 0) {
        saturation = map(currentDistance, 0, totalDistance, 
                        _PAT5_MAX_SATURATION, _PAT5_MIN_SATURATION);
      } else {
        saturation = _PAT5_MAX_SATURATION;
      }
    }
    
    // Set LED color with calculated saturation
    leds[i] = CHSV(_pat5CurrentHue, saturation, _PAT5_BRIGHTNESS);
  }
  
  // Increment cycle counter
  _pat5CurrentCycle++;
  
  // Reset cycle counter when it reaches the threshold
  if (_pat5CurrentCycle >= _pat5CyclesPerColor) {
    _pat5CurrentCycle = 0;
  }
  
  FastLED.show();
}