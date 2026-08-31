#include "controls.h"


Controls::Controls() {

}


void Controls::initalizeAll() {

    if(i2cBus->setPins(5,18)) Serial.println("I2C Pins set OK");
  else Serial.println("I2C Pins setting failed");

  i2cBus->begin();  

  matrix[0].begin(0x70,i2cBus);
  matrix[1].begin(0x71,i2cBus); 
  matrix[2].begin(0x72,i2cBus);
  matrix[3].begin(0x73, i2cBus);

  for(Adafruit_NeoPixel* neopixels : neopixels){
    neopixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  }

}


void Controls::update() {
   switch(input.lightShow){
    case 0:
        // turn off lights
        for(Adafruit_NeoPixel* pixels : neopixels){
            pixels->clear();
            pixels->show();
        }
        for(int m=0; m<4; m++){
            matrix[m].clear();
            matrix[m].writeDisplay();
        }
        break;

    case 1:
        // solid on
        for(int m=0; m<4; m++){
            matrix[m].clear();
            matrix[m].fillRect(0, 0, 8, 16, LED_ON);
            matrix[m].writeDisplay();
        } 
        for(Adafruit_NeoPixel* pixels : neopixels){
            pixels->clear();
            for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
                pixels->setPixelColor(i, pixels->Color(0, 150, 0));
                pixels->show();   // Send the updated pixel colors to the hardware.
            }
     }
        break;
    case 2: // Blinking
       blinkEmoteLights();
        break;
        case 3:
        break;
   }

}

void Controls::blinkEmoteLights(){
    CoolDown++;
        if(CoolDown >= CoolDownTime){
            CoolDown = 0;
            for(int m=0; m<4; m++){
                matrix[m].clear();
                if(random(0,10) > 5){
                    matrix[m].fillRect(0, 0, 8, 16, LED_ON);
                }
                else{
                    matrix[m].clear();
                }

                matrix[m].writeDisplay();
            } 
            for(Adafruit_NeoPixel* pixels : neopixels){
                pixels->clear();
                if(random(0,10) > 5){
                    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
                        
                        pixels->setPixelColor(i, pixels->Color(0, 150, 0));
                    }
                }
                else{
                    pixels->clear();
                }
                    
                pixels->show();   // Send the updated pixel colors to the hardware.
                
            }
        }
}

void Controls::debugLights(){
      for(Adafruit_NeoPixel* pixels : neopixels){
    pixels->clear(); // Set all pixel colors to 'off'

    // The first NeoPixel in a strand is #0, second is 1, all the way up
    // to the count of pixels minus one.
    for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels->setPixelColor(i, pixels->Color(0, 150, 0));

    pixels->show();   // Send the updated pixel colors to the hardware.
    }
    delay(DELAYVAL); // Pause before next pass through loop
  }


  for(int m=0; m<4; m++){
    matrix[m].clear();
    matrix[m].fillRect(0, 0, 8, 16, LED_ON);
    matrix[m].writeDisplay();
  } 
  delay(2000);
  for(int m=0; m<4; m++){
    matrix[m].clear();
    matrix[m].writeDisplay();
  }
  delay(2000);
}
