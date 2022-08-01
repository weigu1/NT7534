/* NT7534 LCD library! (for GTK-281)
   Display test program
   more info on www.weigu.lu 
   
Copyright (C) 2016 Guy WEILER www.weigu.lu 
Enhancments by Bob Fisch (fisch.lu)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

some of this code was written by <cstone@pobox.com> originally and 
some of this code was written by Limor Fried, Adafruit Industries
it is in the public domain.*/

#include "NT7534.h"

// uncomment if the backlight is connected via a transistor to some pin
//#define BACKLIGHT 12

#ifdef ESP32
  NT7534 glcd(26, 27, 32, 33,                    // di  rw  en  rst
              16, 17, 18, 19, 21, 22, 23, 25 ) ; // d0 to d7

#elif defined ARDUINO_TEENSY2 // Teensy
  NT7534 glcd(5, 6, 9, 10,                  // di  rw  en  rst
              0, 1, 2, 3, 13, 14, 15, 4) ;  // d0 to d7

#elif defined(ARDUINO_ARCH_RP2040)  // not working with pico!
NT7534 glcd(14, 13 ,12, 2,               // di  rw  en  rst
            11, 10, 9, 8, 7, 6, 5, 4) ;   // d0 to d7
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  #ifdef BACKLIGHT
    pinMode(BACKLIGHT, OUTPUT);  // turn on backlight
  #endif // ifdef BACKLIGHT
  glcd.begin(0x19);  // initialize and set the contrast to 0x19 (0x01-0x3F)
}  

void loop() {
  #ifdef BACKLIGHT
    digitalWrite(BACKLIGHT, LOW);
  #endif // ifdef BACKLIGHT
  digitalWrite(LED_BUILTIN, HIGH);
  glcd.displaylogo(1);
  delay(2000);
  #ifdef BACKLIGHT
    digitalWrite(BACKLIGHT, HIGH);
  #endif // ifdef BACKLIGHT
  digitalWrite(LED_BUILTIN, LOW);
  delay(2000);
  glcd.clear();
  delay(100);
  glcd.setall();      //fill black
  delay(1000);
  glcd.clear();
  delay(100);
  testpixel();        // draw many pixel
  delay(1000);
  testdrawline();     // draw many lines
  delay(1000);
  testdrawrect();     // draw multiple rectangles  
  glcd.clear();
  testfillrect();     // draw multiple filled rectangles
  delay(1000);
  glcd.clear();
  testdrawcircle();   // draw mulitple circles
  delay(1000);
  glcd.clear();
  testfillcircle();   // multiple black circles
  delay(1000);
  glcd.clear();
  testdrawchar();     // draw the 128 characters in the font
  delay(2000);
  glcd.clear();
  testdrawstring();
  delay(1000);
  glcd.clear();
}

void testpixel() {
  for (uint8_t i=0; i<128; i+=4) {
    for (uint8_t j=0; j<64; j+=4) {
      delay(5);
      glcd.setpixel(i, j, BLACK);
    }
  }
  for (uint8_t i=0; i<128; i+=4) {
    for (uint8_t j=0; j<64; j+=4) {
      delay(5);
      glcd.setpixel(i, j, WHITE);
    }
  }
}

void testdrawline() {
  for (uint8_t i=0; i<128; i+=8) {
    glcd.drawline(0, 0, i, 63, BLACK);
  }
  for (uint8_t i=0; i<64; i+=8) {
    glcd.drawline(0, 0, 127, i, BLACK);
  }
  delay(1000);
  for (uint8_t i=0; i<128; i+=8) {
    glcd.drawline(i, 63, 0, 0, WHITE);
  }
  for (uint8_t i=0; i<64; i+=8) {
    glcd.drawline(127, i, 0, 0, WHITE);
  }
}

void testdrawrect(void) {
  for (uint8_t i=0; i<64; i+=2) glcd.drawrect(i, i, 128-i, 64-i, BLACK);
  for (uint8_t i=0; i<64; i+=2) glcd.drawrect(i, i, 128-i, 64-i, WHITE);
}
    
void testfillrect(void) {
  for (uint8_t i=0; i<64; i+=5) glcd.fillrect(i, i, 128-i, 64-i, i%2);
}
    
void testdrawcircle(void) {
  for (uint8_t i=0; i<63; i+=1) {
    glcd.drawcircle(63, 31, i, BLACK);
    delay(50);
  }
  for (uint8_t i=0; i<63; i+=2) {
    glcd.drawcircle(63, 31, i, WHITE);
  }
  delay(50);
}
    
void testfillcircle(void) {
  for (uint8_t i=0; i<128; i+=20) {
    glcd.fillcircle(i, 32, 10, BLACK);
    delay(50);}
  for (uint8_t i=0; i<128; i+=20) {
    glcd.fillcircle(i, 32, 10, WHITE);
    delay(50);
  }
}
    
void testdrawchar(void) {
  for (uint8_t i=0; i < 128; i++) {
    glcd.drawchar((i % 21) * 6, i/21, i);
  }
}

void testdrawstring(void) {
  uint8_t j=0;
  for (uint8_t i=0; i < 60; i+=10) {    
    glcd.drawstring(i, j, "CREATIVE-LAB");  
    glcd.drawstring(i+10, j+1, "by LAM!!");
    delay(700);  
    glcd.clearstring(i, j, "CREATIVE-LAB");
    glcd.clearstring(i+10, j+1, "by LAM!!");
    j++;
  }
}
