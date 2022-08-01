/* NT7534 LCD library! (for GTK-281)

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
it is in the public domain.  */

#ifdef __AVR__
#include <avr/pgmspace.h>
#include <util/delay.h>
#endif

#include <stdlib.h>
#include "NT7534.h"
// create logo with gimp (128x64) and save as *.xbm. Convert with xbm2NT7534.py
#include "logo1.h" //creative-lab logo 

uint8_t is_reversed = 0;

// a handy reference to where the pages are on the screen
const uint8_t pagemap[] =  { 3, 2, 1, 0, 7, 6, 5, 4 };

// a 5x7 font table
const extern uint8_t PROGMEM font[];

void NT7534::write_data_port(uint8_t c) {
    digitalWrite(d7, (c & 0x80) >> 7);
    digitalWrite(d6, (c & 0x40) >> 6);
    digitalWrite(d5, (c & 0x20) >> 5);
    digitalWrite(d4, (c & 0x10) >> 4);
    digitalWrite(d3, (c & 0x08) >> 3);
    digitalWrite(d2, (c & 0x04) >> 2);
    digitalWrite(d1, (c & 0x02) >> 1);
    digitalWrite(d0, (c & 0x01) >> 0);
}

uint8_t NT7534::read_data_port() {
  uint8_t c = 0;
  c |= (digitalRead(d7)<<7);
  c |= (digitalRead(d6)<<6);
  c |= (digitalRead(d5)<<5);
  c |= (digitalRead(d4)<<4);
  c |= (digitalRead(d3)<<3);
  c |= (digitalRead(d2)<<2);
  c |= (digitalRead(d1)<<1);
  c |= (digitalRead(d0));
  return c;
}

void NT7534::set_datapins_to_input() {
  pinMode(d7,INPUT);
  pinMode(d6,INPUT);
  pinMode(d5,INPUT);
  pinMode(d4,INPUT);
  pinMode(d3,INPUT);
  pinMode(d2,INPUT);
  pinMode(d1,INPUT);
  pinMode(d0,INPUT);
}

void NT7534::set_datapins_to_output() {
  pinMode(d7,OUTPUT);
  pinMode(d6,OUTPUT);
  pinMode(d5,OUTPUT);
  pinMode(d4,OUTPUT);
  pinMode(d3,OUTPUT);
  pinMode(d2,OUTPUT);
  pinMode(d1,OUTPUT);
  pinMode(d0,OUTPUT);
}

void NT7534::write_command_byte(uint8_t c) {
  digitalWrite(di, LOW);	// write_command_byte
  write_data_port(c);
  digitalWrite(rw, LOW);	// write
  digitalWrite(en, HIGH);   // rising edge
  digitalWrite(en, LOW);    // needed!
}

void NT7534::write_data_byte(uint8_t c) {
  digitalWrite(di, HIGH);	// data
  write_data_port(c);
  digitalWrite(rw, LOW);	// write  
  digitalWrite(en, HIGH);
  digitalWrite(en, LOW);   //needed!
}

uint8_t NT7534::read_data_byte() {
  set_datapins_to_input();
  digitalWrite(di, HIGH);	// data
  digitalWrite(rw, HIGH);	// read
  digitalWrite(en, LOW);  
  digitalWrite(en, HIGH);
  delayMicroseconds(10);
  uint8_t c = read_data_port();
  digitalWrite(en, LOW);
  set_datapins_to_output();
  return c;
}

void NT7534::init(void) {
  pinMode(di, OUTPUT); // set pin directions
  pinMode(rw, OUTPUT);
  pinMode(en, OUTPUT);
  pinMode(rs, OUTPUT);
  digitalWrite(en, LOW);
  set_datapins_to_output();
  digitalWrite(rs, LOW); // hardware reset the display
  delayMicroseconds(100);
  digitalWrite(rs, HIGH);
  delayMicroseconds(100);
  write_command_byte(CMD_SET_BIAS_7);               // LCD bias select
  write_command_byte(CMD_SET_ADC_NORMAL);           // ADC select
  write_command_byte(CMD_SET_COM_NORMAL);           // SHL select
  write_command_byte(CMD_SET_DISP_START_LINE);      // Initial display line
  write_command_byte(CMD_SET_POWER_CONTROL | 0x4);  // turn on voltage converter
  delayMicroseconds(50);                                // (VC=1, VR=0, VF=0)
  write_command_byte(CMD_SET_POWER_CONTROL | 0x6);  // turn on voltage regulator
  delayMicroseconds(50);                                // (VC=1, VR=1, VF=0)
  write_command_byte(CMD_SET_POWER_CONTROL | 0x7);  // turn on voltage follower
  delayMicroseconds(10);                                // (VC=1, VR=1, VF=1)
  write_command_byte(CMD_SET_RESISTOR_RATIO | 0x6); // set lcd operating voltage
}

// Para:val (5 bit) from 0x01 (small) to 0x3F (large)
void NT7534::setbrightness(uint8_t val) {
  write_command_byte(CMD_SET_VOLUME_FIRST);
  write_command_byte(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

void NT7534::begin(uint8_t contrast) {
  init();
  write_command_byte(CMD_DISPLAY_ON);
  write_command_byte(CMD_SET_ALLPTS_NORMAL);
  setbrightness(contrast);
}

void NT7534::clear(void) {
  uint8_t p, c;
  for(p = 0; p < 8; p++) {
    write_command_byte(CMD_SET_PAGE | pagemap[p]);
    c=0;      // start at the beginning of the row
    write_command_byte(CMD_SET_COLUMN_LOWER | ((c+1) & 0x0f));
    write_command_byte(CMD_SET_COLUMN_UPPER | (((c+1) >> 4) & 0x0f));
    write_command_byte(CMD_RMW);
    for(c = 0; c <= LCDWIDTH; c++) {
      write_data_byte(0x00);    }
    write_command_byte(CMD_RMW_CLEAR);
  }
}

void NT7534::setall(void) {
  uint8_t p, c;
  for(p = 0; p < 8; p++) {
    write_command_byte(CMD_SET_PAGE | pagemap[p]);
    c=0;      // start at the beginning of the row
    write_command_byte(CMD_SET_COLUMN_LOWER | ((c+1) & 0x0f));
    write_command_byte(CMD_SET_COLUMN_UPPER | (((c+1) >> 4) & 0x0f));
    write_command_byte(CMD_RMW);
    for(c = 0; c <= LCDWIDTH; c++) {
      write_data_byte(0xFF);}
    write_command_byte(CMD_RMW_CLEAR);
  }
}

void NT7534::setpixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x > LCDWIDTH) || (y > LCDHEIGHT)) {
    return;
  }
  uint8_t c = getbyte(x, y/8);
  write_command_byte(CMD_SET_PAGE | pagemap[y/8]);
  write_command_byte(CMD_SET_COLUMN_LOWER | ((x+1) & 0xf));
  write_command_byte(CMD_SET_COLUMN_UPPER | (((x+1) >> 4) & 0xf));
  if (color) {
    write_data_byte(c|(0x80 >> (y%8)));
  }
  else {
    write_data_byte(c & ~(0x80 >> (y%8)));
  }
}

void NT7534::setbyte(uint8_t x, uint8_t p, uint8_t b) {
  write_command_byte(CMD_SET_PAGE | pagemap[p]);
  write_command_byte(CMD_SET_COLUMN_LOWER | ((x+1) & 0xf));
  write_command_byte(CMD_SET_COLUMN_UPPER | (((x+1) >> 4) & 0xf));
  write_data_byte(b);
}

bool NT7534::getpixel(uint8_t x, uint8_t y) {
  uint8_t c;
  write_command_byte(CMD_SET_PAGE | pagemap[y/8]);
  write_command_byte(CMD_SET_COLUMN_LOWER | ((x+1) & 0xf));
  write_command_byte(CMD_SET_COLUMN_UPPER | (((x+1) >> 4) & 0xf));
  c = read_data_byte(); //dummy read see datasheet
  c = read_data_byte();
  if ((c & ((0x80 >> (y%8))))==0) {
    return 0;
  }
  else return 1;
}

uint8_t NT7534::getbyte(uint8_t x, uint8_t p) {
  uint8_t c;
  write_command_byte(CMD_SET_PAGE | pagemap[p]);
  write_command_byte(CMD_SET_COLUMN_LOWER | ((x+1) & 0xf));
  write_command_byte(CMD_SET_COLUMN_UPPER | (((x+1) >> 4) & 0xf));
  c = read_data_byte(); //dummy read datasheet
  c = read_data_byte();
  return c;
}

void NT7534::displaylogo(uint8_t nr) {
  uint8_t p, c;
  for(p = 0; p <= 7; p++) {
    write_command_byte(CMD_SET_PAGE | p);
    c=0;     // start at the beginning of the row
    write_command_byte(CMD_SET_COLUMN_LOWER | (((c+1)) & 0x0f));
    write_command_byte(CMD_SET_COLUMN_UPPER | ((((c+1)) >> 4) & 0x0f));
    write_command_byte(CMD_RMW);
    for(c = 0; c <= LCDWIDTH; c++) {
      if (nr == 1) {
        write_data_byte(pgm_read_byte(&logo1[((LCDWIDTH)*pagemap[p])+c]));
      }
      #ifdef __LOGO2_H__
        if (nr == 2) {
          write_data_byte(pgm_read_byte(&logo2[((LCDWIDTH)*pagemap[p])+c]));
        }
      #endif // #ifdef __LOGO2_H__
      #ifdef __LOGO3_H__
        if (nr == 3) {
          write_data_byte(pgm_read_byte(&logo3[((LCDWIDTH)*pagemap[p])+c]));
        }
      #endif // #ifdef __LOGO3_H__
    }
    write_command_byte(CMD_RMW_CLEAR);
  }
}
    

void NT7534::drawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, 
		      uint8_t color) {  // bresenham's algorithm - thx wikpedia
  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }  
  uint8_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);
  int8_t err = dx / 2;
  int8_t ystep;
  if (y0 < y1) ystep = 1;
  else ystep = -1;
  for (; x0<=x1; x0++) {
    if (steep) {
      setpixel(y0, x0, color);
    }
    else {
      setpixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void NT7534::drawrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint8_t color) {
  for (uint8_t i=x; i<x+w; i++) {
    setpixel(i, y, color);
    setpixel(i, y+h-1, color);
  }
  for (uint8_t i=y; i<y+h; i++) {
    setpixel(x, i, color);
    setpixel(x+w-1, i, color);
  }
}
    
void NT7534::fillrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
		      uint8_t color) {
  for (uint8_t i=x; i<x+w; i++) {
    for (uint8_t j=y; j<y+h; j++) {
      setpixel(i, j, color);
    }
  }
}

void NT7534::drawcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {
  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;
  setpixel(x0, y0+r, color);
  setpixel(x0, y0-r, color);
  setpixel(x0+r, y0, color);
  setpixel(x0-r, y0, color);
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    setpixel(x0 + x, y0 + y, color);
    setpixel(x0 - x, y0 + y, color);
    setpixel(x0 + x, y0 - y, color);
    setpixel(x0 - x, y0 - y, color);
    setpixel(x0 + y, y0 + x, color);
    setpixel(x0 - y, y0 + x, color);
    setpixel(x0 + y, y0 - x, color);
    setpixel(x0 - y, y0 - x, color);
  }
}

void NT7534::fillcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {
  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;
  for (uint8_t i=y0-r; i<=y0+r; i++) {
    setpixel(x0, i, color);
  }
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    for (uint8_t i=y0-y; i<=y0+y; i++) {
      setpixel(x0+x, i, color);
      setpixel(x0-x, i, color);
    }
    for (uint8_t i=y0-x; i<=y0+x; i++) {
      setpixel(x0+y, i, color);
      setpixel(x0-y, i, color);
    }
  }
}

void  NT7534::drawchar(uint8_t x, uint8_t line, char c) {
  for (uint8_t i =0; i<5; i++ ) {
     setbyte(x, line, pgm_read_byte(font+(c*5)+i));
     x++;
  }
}
     
void  NT7534::clearchar(uint8_t x, uint8_t line, char c) {
  for (uint8_t i =0; i<5; i++ ) {
     setbyte(x, line, 0x00);
     x++;
  }
}

void NT7534::drawstring(uint8_t x, uint8_t line, const char *c) {
  while (c[0] != 0) {
    drawchar(x, line, c[0]);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT/8)) {
      return;        // ran out of space :(
    }
  }
}

void NT7534::clearstring(uint8_t x, uint8_t line, const char *c) {
  while (c[0] != 0) {
    clearchar(x, line, c[0]);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT/8)) {
      return;        // ran out of space :(
    }
  }
}

void NT7534::drawbitmap(uint8_t x, uint8_t y, const uint8_t *bitmap,
                        uint8_t w, uint8_t h, uint8_t color) {
  for (uint8_t j=0; j<h; j++) {
    for (uint8_t i=0; i<w; i++ ) {
      if (pgm_read_byte(bitmap + i + (j/8)*w) & (1<<(j%8))) {
        setpixel(x+i, y-1+j, color);
      }
    }
  }
}
