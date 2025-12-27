// Lava lamp pattern for FastLED
// Simulates organic lava lamp movement with rainbow colors

// Global constants
const uint8_t _PAT6_SATURATION = 255;        // Full saturation
const uint8_t _PAT6_BRIGHTNESS = 255;        // Full brightness
const uint16_t _PAT6_SPEED = 4;              // Animation speed
const uint16_t _PAT6_SCALE = 50;             // Perlin noise scale for movement
const uint16_t _PAT6_HUE_SCALE = 30;         // Perlin noise scale for color
const uint8_t _PAT6_BLOB_COUNT = 4;          // Number of lava "blobs"

// Global variables
int _pat6NumLeds = 0;
uint16_t _pat6TimeOffset = 0;                // Time-based animation offset
uint16_t _pat6HueOffset = 0;                 // Offset for hue variation
uint8_t _pat6BaseHue = 0;                    // Base hue that slowly rotates

// External LED array (defined in main sketch)
extern CRGB leds[];

void LavaLampInit(int numLeds) {
  _pat6NumLeds = numLeds;
  _pat6TimeOffset = 0;
  _pat6HueOffset = random(1000, 5000);
  _pat6BaseHue = 0;
}

void LavaLampCycle() {
  // Slowly rotate the base hue for rainbow effect
  _pat6BaseHue += 1;
  
  // Update time offset for animation
  _pat6TimeOffset += _PAT6_SPEED;
  
  // Generate lava lamp effect for each LED
  for (int i = 0; i < _pat6NumLeds; i++) {
    // Use 3D Perlin noise for organic movement
    // x-axis: LED position along strip
    // y-axis: Time for animation
    // z-axis: Additional dimension for complexity
    
    // Calculate brightness using layered noise for blob-like effect
    uint8_t brightness1 = inoise8(i * _PAT6_SCALE, _pat6TimeOffset, 0);
    uint8_t brightness2 = inoise8(i * _PAT6_SCALE + 5000, _pat6TimeOffset + 3000, 1000);
    
    // Combine noise layers for more interesting patterns
    uint16_t combinedBrightness = (brightness1 + brightness2) / 2;
    
    // Apply sine wave modulation for smoother blobs
    uint8_t modulation = sin8(i * 255 / _pat6NumLeds + _pat6TimeOffset / 4);
    combinedBrightness = (combinedBrightness * modulation) / 255;
    
    // Scale brightness to create distinct blobs
    uint8_t finalBrightness;
    if (combinedBrightness > 140) {
      finalBrightness = map(combinedBrightness, 140, 255, 180, 255);
    } else if (combinedBrightness > 80) {
      finalBrightness = map(combinedBrightness, 80, 140, 60, 180);
    } else {
      finalBrightness = map(combinedBrightness, 0, 80, 10, 60);
    }
    
    // Calculate hue using noise for color variation within rainbow
    uint8_t hueNoise = inoise8(i * _PAT6_HUE_SCALE, _pat6HueOffset, _pat6TimeOffset / 8);
    
    // Blend base rotating hue with noise-based variation
    uint8_t finalHue = _pat6BaseHue + (hueNoise / 4);
    
    // Apply color with varying saturation based on brightness for depth
    uint8_t saturation = map(finalBrightness, 10, 255, 200, 255);
    
    leds[i] = CHSV(finalHue, saturation, finalBrightness);
  }
  
  // Apply blur for smoother lava lamp effect
  blur1d(leds, _pat6NumLeds, 64);
  
  FastLED.show();
}