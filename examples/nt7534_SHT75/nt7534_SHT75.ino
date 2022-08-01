/* NT7534 LCD library! (for GTK-281)
   Display temperature data with SHT75 from Sensirion  
   This Version is for Teensy 2.0
   You need the SHT75 library from spease:
   https://github.com/spease/Sensirion
   more info on www.weigu.lu 
   
Copyright (C) 2016 Guy WEILER www.weigu.lu 

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
#include "Sensirion.h"

const uint8_t TEENSY_LED =     11;   
const uint8_t BACKLIGHT_LED =  12;   
const uint8_t dataPin  =       16;
const uint8_t clockPin =       17;
const unsigned long pixdrawtime_ms = 300000;              //5 minutes = 300.000ms
const uint8_t meastime_ms = 1000;                         //once per second
uint8_t counter = 0, i, itemp;
uint8_t tempArray[120];
float temperature, humidity, dewpoint;
char  tempStr[4], humStr[2], dewpStr[2];
unsigned long time1, time2, tdelta;


NT7534 glcd(5, 6, 9, 10,                  // di  rw  en  rst
            0, 1, 2, 3, 13, 14, 15, 4) ;  // d0 to d7

Sensirion tempSensor = Sensirion(dataPin, clockPin);

void setup() {
  //Serial.begin(115200); //for dubugging
  //Serial.println("debugging enabled :)");
  pinMode(TEENSY_LED, OUTPUT); 
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, LOW);
  digitalWrite(TEENSY_LED, HIGH);
  glcd.begin(0x19);  // initialize and set the contrast to 0x19 (0x01-0x3F)
  glcd.clear();
  digitalWrite(BACKLIGHT_LED, HIGH);                      // turn on backlight
  digitalWrite(TEENSY_LED, LOW);
  glcd.drawstring(0, 0, "T:");
  glcd.drawstring(51, 0, "H:");
  glcd.drawstring(90, 0, "DP:");
  glcd.drawstring(36, 0, "C");
  glcd.drawstring(75, 0, "%");
  glcd.drawstring(120, 0, "C");
  glcd.drawline(4, 10, 4, 63, BLACK);                     // y axes
  for (i=60; i>9; i-=2) glcd.setpixel(3, i, BLACK);
  for (i=62; i>9; i-=10) glcd.drawline(1, i, 4, i, BLACK);
  glcd.drawline(124, 10, 124, 63, BLACK);
  for (i=60; i>9; i-=2) glcd.setpixel(125, i, BLACK);
  for (i=62; i>9; i-=10) glcd.drawline(125, i, 127, i, BLACK);
  glcd.drawline(4, 62, 127, 62, BLACK);                   // x axes
  for (i=4; i<125; i+=12) glcd.setpixel(i, 63, BLACK);
  for (i=4; i<125; i+=2) glcd.setpixel(i, 52, BLACK);}    // 20C
  

void loop() {
  time1 = millis();
  tdelta = 0;
  while (tdelta < pixdrawtime_ms) {                       //do this all the time
    tempSensor.measure(&temperature, &humidity, &dewpoint);
    dtostrf(temperature, 2, 1, tempStr);
    glcd.drawstring(12, 0, tempStr);  
    dtostrf(humidity, 2, 0, humStr);
    glcd.drawstring(63, 0, humStr);  
    dtostrf(dewpoint, 2, 0, dewpStr);
    glcd.drawstring(108, 0, dewpStr);  
    delay(meastime_ms);
    time2 = millis();
    tdelta = time2-time1;}
  itemp = (62-(int)((temperature-15.0)*2.0));
  if (itemp<10) itemp = 10;                               //clip max temp
  if (itemp>62) itemp = 62;                               //clip min temp
  tempArray[counter] = itemp;
  glcd.setpixel(counter + 5, itemp, BLACK);
  counter++;
  if (counter==120) {
    while(1) {
      time1 = millis();
      tdelta = 0;
      while (tdelta < pixdrawtime_ms) {
        tempSensor.measure(&temperature, &humidity, &dewpoint);
        dtostrf(temperature, 2, 1, tempStr);
        glcd.drawstring(12, 0, tempStr);  
        dtostrf(humidity, 2, 0, humStr);
        glcd.drawstring(63, 0, humStr);  
        dtostrf(dewpoint, 2, 0, dewpStr);
        glcd.drawstring(108, 0, dewpStr);  
        delay(meastime_ms);
        time2 = millis();
        tdelta = time2-time1;}
      for (i=0; i<119; i++) glcd.setpixel(i+5, tempArray[i], WHITE);
      itemp = 62-(int)((temperature-15.0)*2.0);
      if (itemp<10) itemp = 10;                           //clip max temp
      if (itemp>62) itemp = 62;                           //clip min temp
      tempArray[120] = itemp;
      for (i=0; i<120; i++) tempArray[i] = tempArray[i+1];
      for (i=0; i<120; i++) glcd.setpixel(i+5, tempArray[i], BLACK);}}}
