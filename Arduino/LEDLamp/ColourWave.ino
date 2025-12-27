#include <FastLED.h>
#include "ColourWave.h"

// External LED array (defined in main sketch)
extern CRGB leds[];
int _cw_num_leds = 0;

void ColourWaveInit(int numLeds) {
_cw_num_leds = numLeds;
}

void ColourWaveCycle() {
    static uint8_t hue = 0;

    for (int i = 0; i < _cw_num_leds; i++) {
        // Each LED gets a different hue based on position
        uint8_t ledHue = hue + (i * 10);
        leds[i] = CHSV(ledHue, 255, 255);
    }

    hue++;  // Shift colors
    FastLED.show();
}

 