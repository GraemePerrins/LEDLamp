**Arduino Controlled 3D Printed LED Lamp**
 
 **Code Structure:**
 
 The Arduino code is broken across several files:
*  LEDLamp.ino: Contains globals, setup, initialisation and the main loop
*  LEDPatterns.ino: Pattern controller, pattern array to pattern functions
*  xxxPattern.ino: Each separate pattern is in its own file. The patterns all have an initialise function and a pattern cycle function
* Most of the code should be self explanatory


***LEDLamp.ino:***
* An interrupt is used for handling the push button. The push button triggers a pattern change 
* A timer interrupt is used for controlling when to change the LED pattern being shown
* LEDStrip initialisation. Note that depending on what strip you get you may need to test it to set the correct FastLed colour order (COLOR_ORDER). There is a _testLedStripColorEncoding() to do that
* checkUpdateTime() uses millis() to control when the pattern cycle is called. The code defaults to 50 FPS. See updateIntervalMS.
* The pattern timer cycles the pattern shown every PATTERN_TIMER_SECS_COUNT_INTERVAL. Default is every 47 minutes.

***LEDPatterns.ino:***
* Controls the patterns and uses an indexed array to an array of Pattern function pointers
* Pattern indexes are randomised to implement random pattern orders
* Patterns are setup in initialisePatterns() 
An interrupt is used for handling the push button. The push 

***xxxPattern.ino:***
* Each pattern is in its own file. The patterns all have an initialise function to setup state variables and a PatternCycle function that is called every pattern cycle period (every 20 ms  = 50 times a second)
* The pattern code was generated using claude.ai and the prompt text files are included in the code folder.

**Adding new patterns:**
* The quickest and easiest way to create new patterns is to copy one of the text prompts and then modify it. Specify the pattern you would like and use https://claude.ai/new to generate the resulting code.
* I found that Claude.ai generated bug free correct code every time.
* Once you generate the code with Claude.ai, create a two new file tabs (one for a .h file and one for .ino file) in the Arduino IDE and paste the code into it. 
* Then add the new Pattern to the LEDPatterns.ino file initialisePatterns function.
* FastLED github site has lots of examples and recipes, see https://github.com/FastLED/FastLED/tree/master/cookbook/recipes 


Enjoy.
