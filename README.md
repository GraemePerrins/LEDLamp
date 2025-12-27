**Arduino Controlled 3D Printed LED Lamp**
 
Create a fantastic 3D printed led strip lamp that uses an Arduino Nano to create different coloured patterns on a 60 led strip using the FastLed library. See the project *FinishedLamp* folder for photos and a video of the completed lamp.

The lamp shell, base and led tower are all 3D printed and the FreeCad model files are included so you can easily modify them as needed. The default models will create a Cylinderical lamp with an external double helix pattern that is xx cm tall and approx 10 cm in diameter. 

**Printing**:

The lamp is printed in three parts:
* Lamp base which holds the circuit boards
* Base cover and LED tower which forms a central tower the Led strip is wrapped around
* Lamp shell which is the outer lamp cylinder shell with an external double helix pattern

**Parts List:**

The parts required to build a completed lamp are (all prices are in Australian dollars as at 2025):

* White PLA filament, approx $30 
* Arduino Nano 3.0 CH340 USB driver Mini / Type-C / Micro 16Mhz ATMEGA328P, $5
* LM2596 Adjustable DC to DC Step Down Buck Converter, $3.50
* 60 RGB Led Strip WS2811 DC12V, White PCB, 1m 60 IP65
* Prototype PCB board, $7 
* 6 x 6mm PCB spacers, $4.50 
* M3 bolts 20mm x 6, $3.50
* M3 nuts x 6, $2.95 
* Hook up wire various colours (black,red,blue etc), approx $4
* 4 x rubber adhesive feet, $2.50
* 1 x 1000uF 25VDC Low ESR Electrolytic Capacitor, $1.45
* 2.5mm Bulkhead Male DC Power Connector, $2.95
* White Miniature Pushbutton - SPST Momentary, $1.95
* 12v 1.5 to 2 amp DC power adapter, approx $25 to $30 
* 3 Pin 0.1in 90 Degree Locking Header and plug, $1.50
* DPDT Sub-Miniature Toggle Switch, $3.95 

> Notes: 
> * If you cannot find a white push button just get a red or black one and spray paint the button white 
> * If you use metal PCB spacers then just make sure to put electrical tape over the PCB board where you place the spacers to avoid short circuits
> * In my lamp i used a vertical 3 pin connector for the ELD strip but found it to be just a bit too high to fit the lamp base cover so I ended up soldering the LED strip wires direct to the 3 pin header. Using a 90 degree pin connector will solve that issue for you. 
> * Use a hot melt glue gun to fix the LED strip to the printed internal lamp tower. The led strip self adhesive will not stick to PLA plastic but hot glue works a treat.

**Build Instructions:** 

**Generating the 3D Print STL files:**

> 	If you are happy with the default lamp size *(20 cm tall, and 10 cm diameter) then you can use the project STL files as they are. If you make the LED tower taller then keep in mind you will likely need a longer LED strip. I found the 1m LED strip wrapped in a spiral around the central cylinder fits nicely for a 20cm tall lamp. The lamp cover top includes a number of slot holes to ensure heat is ventilated and will not build up inside the lamp cover.

1. The lamp is made from three 3D printed parts, the lamp base (which holds the circuit boards), the lamp base cover and LED tower which is where you glue the LED strip to, and the external lamp shell cover which needs to be semi-translucent and be a high quality STL file with a high mesh count.
2. The lamp base and LED tower are printed from the FreeCad files. You can adjust the dimensions as needed using the FreeCad files and then generate the STL files as needed. The included models and stl files include external holes for the switches, power connector, and usb-c and are spaced so the push button (to change patterns) faces forward and the usb, power and on/off switch are at the back. I used 
3. To create the patterned lamp cover I wrote a LLM prompt to get gemini AI to generate a python script to create the lamp cover STL file. The python script takes various parameters to control the generated lamp cover dimensions, pattern and mesh quaility and will create the lamp cover STL file (which will be quite large). The prompt file is inclued in the 3DModels folder as is the default generated lamp cover STL file I used. The generateLampCover.sh file shows the parameter values I used. 
> * Play around with generating various lamp cover STL files until you get one you like. Make sure you leave a pattern gap at the bottom to ensure it fits the lamp base and if you adjust the default cylinder dimensions you will need to adjust the other lamp FreeCad models to fit.
4. Once you have generated the lamp model STL files view them in your 3D print software to make sure they look correct and will fit together.

**3D Printing:**
	
1. 	If you are happy with the default lamp size *(20 cm tall, and 10 cm diameter) then you can use the project STL files as they are. If you make the LED tower taller then keep in mind you will likely need a longer LED strip. I found the 1m LED strip wrapped in a spiral around the central cylinder fits nicely for a 20cm tall lamp.
	
2. 	Printing the Lamp cover shell will take over 12 hours as the generated STL file is quite large and you want good quality for the print. For the lamp cover with normal white PLA filament I used print settings of:
	 	+ Line height 0.2mm
	 	+ Reduce print speed to approx
	 	+ Infil of %50
	 	+ Make sure to rotate the cover so the top cap is printed at the bottom as that way you avoid needing any supports
	
3. Printing the lamp base and base cover can be done with print settings of:
	 	+ Line height 0.2mm
	 	+ Reduce print speed to approx
	 	+ Infil of %50
	 	
4. Once the parts are printed make sure they fit together. You may need to sand the inside of the lamp cover and also sand or file the lamp base switch and power connector holes to ensure they fit the components you bought.

	
**Electronics:**

1. Solder the Arduino nano to the PCB board. 
2. Only need two data lines. One to the push button which is used to trigger   pattern changes. We will use Arduino PIN pull-up setting on this pin. The other data line controls the LED strip and needs a 220 Ohm resistor on the data line between the Arduino and the LED strip date pin. 
3. The circuit is very simple. Wire the 12V power connector to the PCB. Wire the capacitor across the + and - 12v lines. 
4. Connect the power switch between the +12v line.
5. Wire the 12v to the 3 Pin connector, and the DC to DC convertor. Adjust the DC convertor to output 5v for the Arduino. The converter can be adjusted via the trim pot. Turn counter clockwise to reduce voltage out. Make sure to adjust the DC convertor and test with a multimeter the output is 5v before continuning.
6. Wire the DC convertor 5v output to the Arduino VIN pin
7. Connect ground to the Arduino
8. Wire a data pin (say D5) from Ardunio via the 220 Ohm resistor to the 3 pin led connector.
9. Wire a data pin (say D2) from the Arduino to the push button. Note no external pull up resistor needed as we will enable the Arduino pull up on that pin via code.
10. Cut the LED strip connector from the start of the strip and wire to the 3 pin plug connector. Wire the LED strip wires to the correct 3 Pin plug to match the pin order you wired for the 12v, gnd and data line on the PCB.
11. Connect the power and connect the Arduino via USB to your PC and do a quick test that the components are all working ok and you can connect and run a basic FastLED example arduino code. (see coding below).
12. Once you have done a connected component test then assemble the PCB boards to the lamp base. Depending on the PCB boards you will need to drip holes in the board and lamp base bottom to place the PCB spacer bolts etc. 
13. See the photos in the FinishedLamp for a quick look how I placed and wired the PCB boards. 
 
**Coding:**

1. All the code is including under the Arduino/LEDLamp folder. The FastLed library is used to control the LED strip and run colour patterns on it. The different pattern code was generated using Claude AI and the code and associated prompt.txt files are all included.
2. See the readme.md file inside the Arduino folder for more information about the code.

**Operating the Lamp:**
 
* The lamp LED starts turned off and after a few seconds the first pattern will be displayed on the LEDs. The included code has eight different LED patterns.
* The lamp will cycle randomly through each pattern every 47 minutes. 
* The pattern sequence is also randomised so that the pattern order will vary after every 12 pattern cycles.
* Pushing the push button will trigger a pattern change.
* Each Led pattern has different behaviours as per each pattern code.

Enjoy your LED Lamp. :-)


  