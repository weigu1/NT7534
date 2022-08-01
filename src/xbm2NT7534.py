#!/usr/bin/env python3
# file_change.py

from re import *
from sys import argv

def convertMatrix(data):
    '''converting D07 D06 D05 D04 D03 D02 D01 D00 to
                  D17 D16 D15 D14 D13 D12 D11 D10
                  D27 D26 D25 D24 D23 D22 D21 D20
                  D37 D36 D35 D34 D33 D32 D31 D30
                  D47 D46 D45 D44 D43 D42 D41 D40
                  D57 D56 D55 D54 D53 D52 D51 D50
                  D67 D66 D65 D64 D63 D62 D61 D60
                  D77 D76 D75 D74 D73 D72 D71 D70
                  

                  D00 D10 D20 D30 D40 D50 D60 D70
                  D01 D11 D21 D31 D41 D51 D61 D71
                  D02 D12 D22 D32 D42 D52 D62 D72
                  D03 D13 D23 D33 D43 D53 D63 D73 
                  D04 D14 D24 D34 D44 D54 D64 D74 
                  D05 D15 D25 D35 D45 D55 D65 D75 
                  D06 D16 D26 D36 D46 D56 D66 D76 
                  D07 D17 D27 D37 D47 D57 D67 D77   
       returning a comma seperated string  
                                                                            '''
    data2 = ((int(data[0],16)&0x01)<<7)|((int(data[1],16)&0x01)<<6)| \
       ((int(data[2],16)&0x01)<<5)|((int(data[3],16)&0x01)<<4)| \
       ((int(data[4],16)&0x01)<<3)|((int(data[5],16)&0x01)<<2)| \
       ((int(data[6],16)&0x01)<<1)|((int(data[7],16)&0x01)<<0), \
       ((int(data[0],16)&0x02)<<6)|((int(data[1],16)&0x02)<<5)| \
       ((int(data[2],16)&0x02)<<4)|((int(data[3],16)&0x02)<<3)| \
       ((int(data[4],16)&0x02)<<2)|((int(data[5],16)&0x02)<<1)| \
       ((int(data[6],16)&0x02)<<0)|((int(data[7],16)&0x02)>>1), \
       ((int(data[0],16)&0x04)<<5)|((int(data[1],16)&0x04)<<4)| \
       ((int(data[2],16)&0x04)<<3)|((int(data[3],16)&0x04)<<2)| \
       ((int(data[4],16)&0x04))<<1|((int(data[5],16)&0x04)<<0)| \
       ((int(data[6],16)&0x04)>>1)|((int(data[7],16)&0x04)>>2), \
       ((int(data[0],16)&0x08)<<4)|((int(data[1],16)&0x08)<<3)| \
       ((int(data[2],16)&0x08)<<2)|((int(data[3],16)&0x08)<<1)| \
       ((int(data[4],16)&0x08)<<0)|((int(data[5],16)&0x08)>>1)| \
       ((int(data[6],16)&0x08)>>2)|((int(data[7],16)&0x08)>>3), \
       ((int(data[0],16)&0x10)<<3)|((int(data[1],16)&0x10)<<2)| \
       ((int(data[2],16)&0x10)<<1)|((int(data[3],16)&0x10)<<0)| \
       ((int(data[4],16)&0x10)>>1)|((int(data[5],16)&0x10)>>2)| \
       ((int(data[6],16)&0x10)>>3)|((int(data[7],16)&0x10)>>4), \
       ((int(data[0],16)&0x20)<<2)|((int(data[1],16)&0x20)<<1)| \
       ((int(data[2],16)&0x20)<<0)|((int(data[3],16)&0x20)>>1)| \
       ((int(data[4],16)&0x20)>>2)|((int(data[5],16)&0x20)>>3)| \
       ((int(data[6],16)&0x20)>>4)|((int(data[7],16)&0x20)>>5), \
       ((int(data[0],16)&0x40)<<1)|((int(data[1],16)&0x40)<<0)| \
       ((int(data[2],16)&0x40)>>1)|((int(data[3],16)&0x40)>>2)| \
       ((int(data[4],16)&0x40)>>3)|((int(data[5],16)&0x40)>>4)| \
       ((int(data[6],16)&0x40)>>5)|((int(data[7],16)&0x40)>>6), \
       ((int(data[0],16)&0x80)<<0)|((int(data[1],16)&0x80)>>1)| \
       ((int(data[2],16)&0x80)>>2)|((int(data[3],16)&0x80)>>3)| \
       ((int(data[4],16)&0x80)>>4)|((int(data[5],16)&0x80)>>5)| \
       ((int(data[6],16)&0x80)>>6)|((int(data[7],16)&0x80)>>7)
    datastr = ''
    for i in range(8):
        hexdata2 = hex(data2[i])
        if data2[i]<16: 
           hexdata2 = hexdata2.replace('0x','0x0')
        datastr = datastr + hexdata2 + ', '
    datastr = datastr[:-1]
    datastr = datastr + '\n'
    return datastr

# check arguments
if len(argv) > 1:
    xbmfile = argv[1]
else:
    xbmfile = 'test.xbm'
if len(argv) > 2:
    cfileExt = argv[2]
else:
    cfileExt = '1'
hfile = 'logo' + cfileExt + '.h'

#open files
try:
    f1 = open(xbmfile)
except IOError:
    print('Cannot find file:', xbmfile)
    exit()
try:
    f2 = open(hfile, 'w')
except IOError:
    print('Cannot create file:', hfile)
    exit()
#get text from xbm file and filter
txt1 = f1.read()
txt2 = findall('0x..',txt1)
txt3 = '/* logo data for a 128x64 pixel display with NT7534 chip\n   ' \
        + hfile  + '\n*/\n\nconst extern PROGMEM uint8_t logo' \
        + cfileExt + '[] = {\n'
for j in range(0,897,128):    
    for k in range(16):    
        i = j + k  
        print (i)
        data = [txt2[i+0], txt2[i+16], txt2[i+32], txt2[i+48], \
                txt2[i+64], txt2[i+80], txt2[i+96], txt2[i+112]]
        txt3 = txt3 + '  ' + convertMatrix(data)
txt3 = txt3[:-2] + '};\n'


f2.write(txt3)


f1.close()
f2.close()
