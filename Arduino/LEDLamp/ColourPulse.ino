// Rainbow pulse pattern for FastLED
// Pulses of color travel up and down the strip against a white background

// Global constants
const uint8_t _PAT3_WHITE_HUE = 0;
const uint8_t _PAT3_WHITE_SAT = 0;
const uint8_t _PAT3_WHITE_VAL = 255;

// Global variables
int _pat3NumLeds = 0;
int _pat3PulseLength = 0;
float _pat3PulseDuration = 0.0;
int _pat3Position = 0;
uint8_t _pat3CurrentHue = 0;
bool _pat3MovingUp = true;
unsigned long _pat3StepCount = 0;
unsigned long _pat3TotalSteps = 0;

// External LED array (defined in main sketch)
extern CRGB leds[];

void ColourPulseInit(int numLeds, int pulseLength, float pulseDuration) {
  _pat3NumLeds = numLeds;
  _pat3PulseLength = pulseLength;
  _pat3PulseDuration = pulseDuration;
  _pat3Position = 0;
  _pat3CurrentHue = 0;  // Start with red
  _pat3MovingUp = true;
  _pat3StepCount = 0;
  
  // Calculate total steps for a complete up and down cycle
  // Distance = 2 * numLeds (up and down)
  // Time per step = 20ms = 0.02s
  // Total steps = pulseDuration / 0.02
  _pat3TotalSteps = (unsigned long)(pulseDuration / 0.02);
}

void ColourPulseCycle() {
  // Set all LEDs to white background
  for (int i = 0; i < _pat3NumLeds; i++) {
    leds[i] = CHSV(_PAT3_WHITE_HUE, _PAT3_WHITE_SAT, _PAT3_WHITE_VAL);
  }
  
  // Calculate current position based on step count
  // Position ranges from 0 to (numLeds-1) and back
  float progress = (float)_pat3StepCount / (float)_pat3TotalSteps;
  
  if (progress < 0.5) {
    // Moving up (0 to numLeds-1)
    _pat3Position = (int)(progress * 2.0 * _pat3NumLeds);
    _pat3MovingUp = true;
  } else {
    // Moving down (numLeds-1 to 0)
    _pat3Position = (int)((1.0 - (progress - 0.5) * 2.0) * _pat3NumLeds);
    _pat3MovingUp = false;
  }
  
  // Constrain position to valid range
  _pat3Position = constrain(_pat3Position, 0, _pat3NumLeds - 1);
  
  // Draw the color pulse
  for (int i = 0; i < _pat3PulseLength; i++) {
    int ledIndex = _pat3Position - (_pat3PulseLength / 2) + i;
    
    // Wrap or skip if outside strip bounds
    if (ledIndex >= 0 && ledIndex < _pat3NumLeds) {
      leds[ledIndex] = CHSV(_pat3CurrentHue, 255, 255);
    }
  }
  
  // Increment step counter
  _pat3StepCount++;
  
  // Check if cycle is complete
  if (_pat3StepCount >= _pat3TotalSteps) {
    _pat3StepCount = 0;
    _pat3Position = 0;
    
    // Change to next hue in spectrum (red->violet = 0->192 in HSV)
    // Using increments of ~21 gives us 9 distinct colors across visible spectrum
    _pat3CurrentHue += 21;
    if (_pat3CurrentHue > 192) {
      _pat3CurrentHue = 0;  // Wrap back to red
    }
  }
  
  FastLED.show();
}