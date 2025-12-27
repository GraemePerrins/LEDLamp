// Perlin noise rainbow pulse pattern for FastLED
// Uses 2D Perlin noise to generate random color patterns that pulse

// Global constants
const uint8_t _PAT4_BASE_VALUE = 128;      // Base brightness value
const uint8_t _PAT4_PULSE_AMPLITUDE = 127; // Pulse amplitude (128 + 127 = 255 max)
const uint8_t _PAT4_SATURATION = 255;      // Full saturation for vibrant colors
const uint16_t _PAT4_NOISE_SCALE = 50;     // Scale factor for Perlin noise

// Global variables
int _pat4NumLeds = 0;
int _pat4CyclesPerPattern = 0;
int _pat4CurrentCycle = 0;
uint16_t _pat4NoiseOffset = 0;             // Offset for noise function variation
uint8_t* _pat4BaseHues = nullptr;          // Array to store base hue for each LED
float _pat4PulsePhase = 0.0;               // Current phase of pulse (0.0 to 1.0)

// External LED array (defined in main sketch)
extern CRGB leds[];

void NoisePulseInit(int numLeds, int cyclesPerPattern) {
  _pat4NumLeds = numLeds;
  _pat4CyclesPerPattern = cyclesPerPattern;
  _pat4CurrentCycle = 0;
  _pat4NoiseOffset = random(1000, 9000);   // Random starting offset
  _pat4PulsePhase = 0.0;
  
  // Allocate memory for base hues
  if (_pat4BaseHues != nullptr) {
    delete[] _pat4BaseHues;
  }
  _pat4BaseHues = new uint8_t[numLeds];
  
  // Generate initial color pattern using Perlin noise
  for (int i = 0; i < _pat4NumLeds; i++) {
    // Use 2D Perlin noise with LED index as one dimension
    uint8_t noiseValue = inoise8(i * _PAT4_NOISE_SCALE, _pat4NoiseOffset);
    _pat4BaseHues[i] = noiseValue;
  }
}

void NoisePulseCycle() {
  // Check if we need to generate a new pattern
  if (_pat4CurrentCycle == 0) {
    // Generate new random offset for noise function
    _pat4NoiseOffset = random(1000, 9000);
    
    // Generate new color pattern
    for (int i = 0; i < _pat4NumLeds; i++) {
      uint8_t noiseValue = inoise8(i * _PAT4_NOISE_SCALE, _pat4NoiseOffset);
      _pat4BaseHues[i] = noiseValue;
    }
  }
  
  // Calculate pulse phase (0.0 to 1.0 through the cycle)
  _pat4PulsePhase = (float)_pat4CurrentCycle / (float)_pat4CyclesPerPattern;
  
  // Calculate brightness multiplier using sine wave for smooth pulsing
  // Phase 0.0 -> 0.5 = increasing, 0.5 -> 1.0 = decreasing
  float pulseValue = sin(_pat4PulsePhase * 2.0 * PI);
  
  // Convert to brightness adjustment (0 to PULSE_AMPLITUDE)
  int16_t brightnessAdjust = (int16_t)(pulseValue * _PAT4_PULSE_AMPLITUDE);
  
  // Apply colors to LEDs
  for (int i = 0; i < _pat4NumLeds; i++) {
    // Calculate adjusted brightness
    int16_t adjustedValue = _PAT4_BASE_VALUE + brightnessAdjust;
    
    // Constrain to valid range
    uint8_t finalValue = constrain(adjustedValue, 0, 255);
    
    // Set LED color with base hue and pulsing brightness
    leds[i] = CHSV(_pat4BaseHues[i], _PAT4_SATURATION, finalValue);
  }
  
  // Increment cycle counter
  _pat4CurrentCycle++;
  
  // Reset cycle counter when pattern duration is complete
  if (_pat4CurrentCycle >= _pat4CyclesPerPattern) {
    _pat4CurrentCycle = 0;
  }
  
  FastLED.show();
}