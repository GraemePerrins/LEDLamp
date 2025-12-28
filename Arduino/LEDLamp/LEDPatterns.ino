#include "LEDPatterns.h"


const int LED_PATTERNS_TABLE_SIZE = 9;
const int PATTERN_RANDOMISE_CYCLE = LED_PATTERNS_TABLE_SIZE; // Randomise pattern sequence after every 12 pattern changes

int patternIndexTable[LED_PATTERNS_TABLE_SIZE];
const int PATTERNINDEXSIZE = sizeof(patternIndexTable) / sizeof(patternIndexTable[0]);
unsigned int currentPatternIndex = 0;
int patternCycleCount = 0;

typedef void (*PatternActionFuncPtr)();
PatternActionFuncPtr patternFunctions[LED_PATTERNS_TABLE_SIZE];


void initialisePatternIndexTable() {
  for (int i = 0; i < LED_PATTERNS_TABLE_SIZE; i++)
  {
    patternIndexTable[i] = i;
  }
  currentPatternIndex = 0;
  patternCycleCount = 0;
}


// Function to swap two integers
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher-Yates shuffle algorithm
void shuffle(int arr[], int n) {
    // Seed the random number generator
  randomSeed(analogRead(0));

    for (int i = n - 1; i > 0; i--) {
        // Pick a random index from 0 to i
        int j = rand() % (i + 1);

        // Swap arr[i] with the element at random index j
        swap(&arr[i], &arr[j]);
    }
}

void randomShufflePatternIndexes()
{
    shuffle(patternIndexTable, PATTERNINDEXSIZE);
    currentPatternIndex = 0;
}

void clearLEDStrip() {
  FastLED.clear(true);
}
 
void changePattern()
{
  if (patternCycleCount++ >= PATTERN_RANDOMISE_CYCLE) {
      randomShufflePatternIndexes();
      patternCycleCount = 0;
  }

  do {
    currentPatternIndex = (currentPatternIndex + 1) % PATTERNINDEXSIZE;  
  } while(patternFunctions[patternIndexTable[currentPatternIndex]] == NULL);

  Serial.println("Pattern index:" +String(currentPatternIndex));
  clearLEDStrip();
  FastLED.show();
}


void initialisePatterns(int numLeds) {

  for (int i = 0; i < LED_PATTERNS_TABLE_SIZE; i++)
    patternFunctions[i] = NULL;

  initialisePatternIndexTable();
  
  PinkPurpleCyclePatternInit(numLeds);
  FirePatternInit(numLeds,40,90);
  ColourWaveInit(numLeds);
  BlueSkyInit(numLeds,120);
  ColourPulseInit(numLeds, 10, 5);
  NoisePulseInit(numLeds, 260);
  RandomColourInit(numLeds, 3000, 26);
  LavaLampInit(numLeds);
  WhiteBreathInit(numLeds, 60);

  patternFunctions[0] = &PinkPurpleCycle;
  patternFunctions[1] = &FirePatternCycle;
  patternFunctions[2] = &ColourWaveCycle;
  patternFunctions[3] = &BlueSkyCycle;
  patternFunctions[4] = &ColourPulseCycle; 
  patternFunctions[5] = &NoisePulseCycle; 
  patternFunctions[6] = &RandomColourCycle;  
  patternFunctions[7] = &LavaLampCycle;  
  patternFunctions[8] = &WhiteBreathCycle;  
 
  randomShufflePatternIndexes();
  changePattern();
}

void patternCycle() {
  if (patternFunctions[patternIndexTable[currentPatternIndex]] != NULL) {
    patternFunctions[patternIndexTable[currentPatternIndex]]();
  }
}


