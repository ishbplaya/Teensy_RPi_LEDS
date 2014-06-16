/*  OctoWS2811 BasicTest.ino - Basic RGB LED Test
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
    Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.

  This test is useful for checking if your LED strips work, and which
  color config (WS2811_RGB, WS2811_GRB, etc) they require.
*/

#include <OctoWS2811.h>
#include <Adafruit_NeoPixel.h>

const int ledsPerStrip = 20;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
  Serial.begin(9600); //USB is always 12 Mbit/sec
  leds.begin();
  leds.show();
}

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
#define PURPLE 0x551A8B
//Local Parameters
String str;

void loop() {
  Serial.println("Waiting for data to come by... =(");
  //Local Parameters
  //String str;
  char charBuf[50];
  String colorName; //seperated by ';'
  String stripNuminString; //seperated by '/'
  String pixelStartinString; //seperated by '-'
  String pixelEndinString; //seperated by '.'
  int stripNum;
  int pixelsStart;
  int pixelsEnd;
  //delay(1000); //do not print TOO fast!
  if(Serial.available() > 0) //Typical serial data = RED;02/000-020.\n
  {
    str = Serial.readStringUntil('\n'); //end of the string is 
    str.toCharArray(charBuf, 50);
    Serial.println(charBuf);
    int i = 0;
    boolean gotColorName = false;
    boolean gotStripNum = false;
    boolean gotPixelStart = false;
    boolean gotPixelEnd = false;
    for (i; i < 50; i++)
    {
      if(charBuf[i] == ';' && !gotColorName)
        gotColorName = true;
      else if (charBuf[i] == '/' && !gotStripNum)
        gotStripNum = true;
      else if (charBuf[i] == '-' && !gotPixelStart)
        gotPixelStart = true;
      else if (charBuf[i] == '.' && !gotPixelEnd)
        gotPixelEnd = true;
      else if (!gotColorName && !gotStripNum && !gotPixelStart && !gotPixelEnd)
        colorName += charBuf[i];
      else if (gotColorName && !gotStripNum && !gotPixelStart && !gotPixelEnd)
        stripNuminString += charBuf[i];
      else if (gotColorName && gotStripNum && !gotPixelStart && !gotPixelEnd)
        pixelStartinString += charBuf[i];
      else if (gotColorName && gotStripNum && gotPixelStart && !gotPixelEnd)
        pixelEndinString += charBuf[i];
    }
    stripNum = stripNuminString.toInt(); //convert from String to Int
    pixelsStart = pixelStartinString.toInt();
    pixelsEnd = pixelEndinString.toInt();
    
    Serial.println("str is : " + str + "|| colorName is : " + colorName+
              "|| stripNuminString is : " + stripNuminString
            + "|| pixelsStart is : " + pixelStartinString + 
            "|| pixelsEnd is: " + pixelEndinString +"\n");
  }
  
  int microsec = 2000000 / leds.numPixels();  // change them all in 2 seconds

  // uncomment for voltage controlled speed
  // millisec = analogRead(A9) / 40;
  str = colorName;
  if(str == "come on!") {
    colorWipe(RED, microsec);
    colorWipe(GREEN, microsec);
    colorWipe(BLUE, microsec);
    colorWipe(PURPLE, microsec);
    
    theaterChase(RED, microsec);
    theaterChase(GREEN, microsec);
    theaterChase(BLUE, microsec);
    theaterChase(PURPLE, microsec);
    
    rainbow(20);
    rainbowCycle(20);
    theaterChaseRainbow(50);
  }
  else if(str == "RED") {
    //colorWipe(RED, microsec);
    setBomb(RED, microsec, stripNum, pixelsStart, pixelsEnd);
  }
  else if(str == "GREEN") {
    //colorWipe(GREEN, microsec);
    setBomb(GREEN, microsec, stripNum, pixelsStart, pixelsEnd);
  }
  else if (str == "BLUE") {
    //colorWipe(BLUE, microsec);
    setBomb(BLUE, microsec, stripNum, pixelsStart, pixelsEnd);
  }
  else if (str == "YELLOW") {
    //colorWipe(YELLOW, microsec);
    setBomb(YELLOW, microsec, stripNum, pixelsStart, pixelsEnd);
  }
  else if (str == "PINK") {
    //colorWipe(PINK, microsec);
    setBomb(PINK, microsec, stripNum, pixelsStart, pixelsEnd);
  }
  else if (str == "ORANGE") {
    //colorWipe(ORANGE, microsec);
    setBomb(ORANGE, microsec, stripNum, pixelsStart, pixelsEnd);
  }
  else if (str == "WHITE") {
    //colorWipe(WHITE, microsec);
    setBomb(WHITE, microsec, stripNum, pixelsStart, pixelsEnd);
  }
  else {
    //colorWipe(PURPLE, microsec);
    setBomb(PURPLE, microsec, stripNum, pixelsStart, pixelsEnd);
  }
  delay(1000);
}

void colorWipe(int color, int wait)
{
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
}

void setBomb(int color, int wait, int strip, int pixelStart, int pixelEnd)
{
  for (int i=pixelStart; i < pixelEnd; i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
}

//from NEOPIXEL LIBRARY
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<leds.numPixels(); i++) {
      leds.setPixel(i, 255);
    }
    leds.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout 
//from NEOPIXEL LIBRARY
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< leds.numPixels(); i++) {
      leds.setPixel(i,(i * 256 / leds.numPixels()) + j);
    }
    leds.show();
    delay(wait);
  }
}

//Theatre-style crawling lights from NEOPIXEL LIBRARY
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < leds.numPixels(); i=i+3) {
        leds.setPixel(i+q, c);    //turn every third pixel on
      }
      leds.show();
     
      delay(wait);
     
      for (int i=0; i < leds.numPixels(); i=i+3) {
        leds.setPixel(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect from NEOPIXEL LIBRARY
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < leds.numPixels(); i=i+3) {
          leds.setPixel(i+q, (i+j) % 255);    //turn every third pixel on
        }
        leds.show();
       
        delay(wait);
       
        for (int i=0; i < leds.numPixels(); i=i+3) {
          leds.setPixel(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

