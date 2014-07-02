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
#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
#define PURPLE 0x551A8B
#define BLANK  0x000000
#define AQUA   0x00FFFF

//Global Parameters
String str;
char charBuf[100];
const int ledsPerStrip = 20;
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

void setup() {
  Serial.begin(9600); //USB is always 12 Mbit/sec
  leds.begin();
  leds.show();
}

void loop() {
  Serial.println("Waiting for data to come by... =(");

  if(Serial.available() > 1)
  {
    str = Serial.readString();
    str.toCharArray(charBuf, 100);
    char *p = charBuf;
    char *str;
    while ((str = strtok_r(p, ";", &p)) != NULL) {// delimiter is the semicolon
      String messageId = getValue(str,',',0);
      int microsec = 100000 / leds.numPixels();  // change them all in 0.1 seconds
      if(messageId == "C")
      {
        colorWipe(BLANK, microsec);
      }
      else if (messageId == "S")
      {
        leds.show();
      }
      else if (messageId == "P")
      {
        int colorName = getValue(str,',',1).toInt();
        int pixelId = getValue(str,',',2).toInt();
        leds.setPixel(pixelId, colorName);
        
        Serial.println("Color :" + colorName);
        Serial.println("Pixel :" + pixelId);
      }
      else if (messageId == "R")
      {
        int colorName = getValue(str,',',1).toInt();
        int pixelsStart = getValue(str,',',2).toInt();
        int pixelsEnd = getValue(str,',',3).toInt();
        setRange(colorName, microsec,  pixelsStart, pixelsEnd);
        
        Serial.println("Color :" + colorName);
        Serial.println("Start Pixel :" + pixelsStart);
        Serial.println("End Pixel :" + pixelsEnd);
      }
      //delay(1000);
    }
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void colorWipe(int color, int wait)
{
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
    leds.show();
    delayMicroseconds(wait);
  }
}

void setRange(int color, int wait, int pixelStart, int pixelEnd)
{
  for (int i=0; i < leds.numPixels(); i++) {
    if(i > pixelStart && i < pixelEnd) {
         leds.setPixel(i, color); 
    }
    //leds.show();
    delayMicroseconds(wait);
  }
}

int rainbowColors[180];
void rainbow_setup(uint8_t wait) {
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  for (int i=0; i<180; i++) {
    int hue = i * 2;
    int saturation = 100;
    int lightness = 50;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
  }
  digitalWrite(1, LOW);
  leds.begin();
}

void rainbow(int phaseShift, int cycleTime)
{
  int color, x, y, offset, wait;

  wait = cycleTime * 1000 / ledsPerStrip;
  for (color=0; color < 180; color++) {
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStrip; x++) {
      for (y=0; y < 8; y++) {
        int index = (color + x + y*phaseShift/2) % 180;
        leds.setPixel(x + y*ledsPerStrip, rainbowColors[index]);
      }
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  }
}

// Convert HSL (Hue, Saturation, Lightness) to RGB (Red, Green, Blue)
//
//   hue:        0 to 359 - position on the color wheel, 0=red, 60=orange,
//                            120=yellow, 180=green, 240=blue, 300=violet
//
//   saturation: 0 to 100 - how bright or dull the color, 100=full, 0=gray
//
//   lightness:  0 to 100 - how light the color is, 100=white, 50=color, 0=black
//
int makeColor(unsigned int hue, unsigned int saturation, unsigned int lightness)
{
	unsigned int red, green, blue;
	unsigned int var1, var2;

	if (hue > 359) hue = hue % 360;
	if (saturation > 100) saturation = 100;
	if (lightness > 100) lightness = 100;

	// algorithm from: http://www.easyrgb.com/index.php?X=MATH&H=19#text19
	if (saturation == 0) {
		red = green = blue = lightness * 255 / 100;
	} else {
		if (lightness < 50) {
			var2 = lightness * (100 + saturation);
		} else {
			var2 = ((lightness + saturation) * 100) - (saturation * lightness);
		}
		var1 = lightness * 200 - var2;
		red = h2rgb(var1, var2, (hue < 240) ? hue + 120 : hue - 240) * 255 / 600000;
		green = h2rgb(var1, var2, hue) * 255 / 600000;
		blue = h2rgb(var1, var2, (hue >= 120) ? hue - 120 : hue + 240) * 255 / 600000;
	}
	return (red << 16) | (green << 8) | blue;
}

unsigned int h2rgb(unsigned int v1, unsigned int v2, unsigned int hue)
{
	if (hue < 60) return v1 * 60 + (v2 - v1) * hue;
	if (hue < 180) return v2 * 60;
	if (hue < 240) return v1 * 60 + (v2 - v1) * (240 - hue);
	return v1 * 60;
}

// alternate code:
// http://forum.pjrc.com/threads/16469-looking-for-ideas-on-generating-RGB-colors-from-accelerometer-gyroscope?p=37170&viewfull=1#post37170

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
          leds.setPixel(i+q, (i+j));    //turn every third pixel on
        }
        leds.show();
       
        delay(wait);
       
        for (int i=0; i < leds.numPixels(); i=i+3) {
          leds.setPixel(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

