#include <Arduino.h> 

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif





#define NEOPIXEL1        22 // On Trinket or Gemma, suggest changing this to 1
#define NEOPIXEL2        23 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 12 // Popular NeoPixel ring size
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

class Controls {

public:

    struct inputData{

        int emote = 0; // Play the designated emote sound and play a little light show. 

        int emoteState = 0;

        int lightShow = 0; // 0 = off, 1 = solid, 2 = slow blink, 3 = when emote is on

    }input; 

    Controls();
    void initalizeAll();
    
    
    void update(); // loop back to check to see if a command has been processed

    void debugLights(); // turn all the lights on to make sure that they work

private:


void blinkEmoteLights();


const int CoolDownTime = 150; // time in cycles before another random emote can be triggered 
int CoolDown = 0;


Adafruit_NeoPixel* neopixels[2] = {new Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL1, NEO_GRB + NEO_KHZ800), new Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL2, NEO_GRB + NEO_KHZ800)};
Adafruit_8x16matrix matrix[4] = {Adafruit_8x16matrix(),Adafruit_8x16matrix(),Adafruit_8x16matrix(),Adafruit_8x16matrix()};
TwoWire* i2cBus = new TwoWire(1);  // Bus Number zero



};