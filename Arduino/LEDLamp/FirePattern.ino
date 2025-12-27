#include <FastLED.h>
#include "FirePattern.h"

const int _FP_NUM_LEDS = 60;

// External LED array (defined in main sketch)
extern CRGB leds[];
byte _fireHeat[_FP_NUM_LEDS];

uint8_t _cooling;
uint8_t _sparking;


void FirePatternInit(int numLeds,uint8_t cooling, uint8_t sparking) {
  _cooling = cooling;
  _sparking = sparking;
}


void customFire(uint8_t cooling, uint8_t sparking) {
    // cooling: 20-100 (higher = faster cooling)
    // sparking: 50-200 (higher = more sparks)

    // Cool down
    for (int i = 0; i < _FP_NUM_LEDS; i++) {
        _fireHeat[i] = qsub8(_fireHeat[i], random8(0, ((cooling * 10) / _FP_NUM_LEDS) + 2));
    }

    // Heat drift upward
    for (int k = _FP_NUM_LEDS - 1; k >= 2; k--) {
        _fireHeat[k] = (_fireHeat[k - 1] + _fireHeat[k - 2] + _fireHeat[k - 2]) / 3;
    }

    // Ignite sparks
    if (random8() < sparking) {
        int y = random8(7);
        _fireHeat[y] = qadd8(_fireHeat[y], random8(160, 255));
    }

    // Map to colors
    for (int j = 0; j < _FP_NUM_LEDS; j++) {
        leds[j] = ColorFromPalette(HeatColors_p, scale8(_fireHeat[j], 240)); //LavaColors_p HeatColors_p
    }
}


void FirePatternCycle() {
  customFire(_cooling, _sparking);
  FastLED.show();
}
 