#ifndef LP_LEDPATTERN_H
#define LP_LEDPATTERN_H


#include "PinkPurpleCycle.h"
#include "FirePattern.h"
#include "ColourWave.h"
#include "BlueSky.h"
#include "NoisePulse.h"
#include "RandomColour.h"
#include "LavaLamp.h"


const int LED_PATTERNS_TABLE_SIZE = 16;

void initialisePatterns(int numLeds);
void randomShufflePatternIndexes();
void changePattern();
void patternCycle();

#endif
