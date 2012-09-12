/**
 * \file flex_lcd_16x1.c
 * \brief LCD driving for compiling with PICC for picdem2plus
 * \author Aurelien BOUIN
 * \version 1.0.0
 * \date 12 septembre 2012
 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2012  Aurelien BOUIN (a_bouin@yahoo.fr)                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  This program is free software; you can redistribute it and/or              *
 *  modify it under the terms of the GNU General Public License                *
 *  as published by the Free Software Foundation; either version 3             *
 *  of the License, or (at your option) any later version.                     *
 *                                                                             *
 *  This program is distributed in the hope that it will be useful,            *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 *  GNU General Public License for more details.                               *
 *                                                                             *
 *  You should have received a copy of the GNU General Public License          *
 *  along with this program; if not, write to the Free Software                *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// These are randomly assigned pins, used to test 
// that the driver can work with any arrangement 
// of i/o pins.  
#ifndef LCD_DB4
#define LCD_DB4   PIN_D0
#endif
#ifndef LCD_DB5
#define LCD_DB5   PIN_D1
#endif
#ifndef LCD_DB6
#define LCD_DB6   PIN_D2
#endif
#ifndef LCD_DB7
#define LCD_DB7   PIN_D3
#endif
#ifndef LCD_RS
#define LCD_RS    PIN_A3
#endif
#ifndef LCD_RW
#define LCD_RW    PIN_A2
#endif
#ifndef LCD_E
#define LCD_E     PIN_A1
#endif

/* 
#define LCD_DB4   PIN_D4 
#define LCD_DB5   PIN_D5 
#define LCD_DB6   PIN_D6 
#define LCD_DB7   PIN_D7 

#define LCD_RS    PIN_E0 
#define LCD_RW    PIN_E1 
#define LCD_E     PIN_E2 
*/ 

// If you want to use a 6-pin interface for your LCD, then 
// connect the R/W pin on the LCD to ground and comment 
// out the following line.  A 6-pin interface to the LCD 
// is used if only have a few free i/o pins available on 
// your PIC, and you want to use the smallest possible 
// number of pins for the LCD. 
#ifndef USE_LCD_RW
#define USE_LCD_RW   1
#endif

//======================================== 
// Use "2 lines" as the lcd type for the 16x1 LCD. 
// The LCD is the same as an 8x2 LCD, but with the 
// bottom line appended on the right side of the first line. 
#define LCD_TYPE  2         // 0=5x7, 1=5x10, 2=2 lines 
#define LCD_2ND_HALF_ADDRESS  0x40 

#define LCD_WIDTH  16 
#define LCD_HALF_WIDTH  (LCD_WIDTH/2) 

int8 const LCD_INIT_STRING[4] = 
{ 
 0x20 | (LCD_TYPE << 2), // Func set: 4-bit, 2 lines, 5x8 dots 
 0xc,                    // Display on 
 1,                      // Clear display 
 6                       // Increment cursor 
 }; 

int8 lcd_xcoord; 
                              

//------------------------------------- 
void lcd_send_nibble(int8 nibble) 
{ 
// Note:  !! converts an integer expression 
// to a boolean (1 or 0). 
 output_bit(LCD_DB4, !!(nibble & 1)); 
 output_bit(LCD_DB5, !!(nibble & 2));  
 output_bit(LCD_DB6, !!(nibble & 4));    
 output_bit(LCD_DB7, !!(nibble & 8));    

 delay_cycles(1); 
 output_high(LCD_E); 
 delay_us(2); 
 output_low(LCD_E); 
} 

//----------------------------------- 
// This sub-routine is only called by lcd_read_byte(). 
// It's not a stand-alone routine.  For example, the 
// R/W signal is set high by lcd_read_byte() before 
// this routine is called.      

#ifdef USE_LCD_RW 
int8 lcd_read_nibble(void) 
{ 
int8 retval; 
// Create bit variables so that we can easily set 
// individual bits in the retval variable. 
#bit retval_0 = retval.0 
#bit retval_1 = retval.1 
#bit retval_2 = retval.2 
#bit retval_3 = retval.3 

retval = 0; 
    
output_high(LCD_E); 
delay_us(1); 

retval_0 = input(LCD_DB4); 
retval_1 = input(LCD_DB5); 
retval_2 = input(LCD_DB6); 
retval_3 = input(LCD_DB7); 
  
output_low(LCD_E); 
delay_us(1);  
    
return(retval);    
}    
#endif 

//--------------------------------------- 
// Read a byte from the LCD and return it. 

#ifdef USE_LCD_RW 
int8 lcd_read_byte(void) 
{ 
int8 low; 
int8 high; 

output_high(LCD_RW); 
delay_cycles(1); 

high = lcd_read_nibble(); 

low = lcd_read_nibble(); 

return((high << 4) | low); 
} 
#endif 

//---------------------------------------- 
// Send a byte to the LCD. 
void lcd_send_byte(int8 address, int8 n) 
{ 
output_low(LCD_RS); 

#ifdef USE_LCD_RW 
while(bit_test(lcd_read_byte(), 7)) ; 
#else 
delay_us(60);  
#endif 

if(address) 
   output_high(LCD_RS); 
else 
   output_low(LCD_RS); 
      
 delay_cycles(1); 

#ifdef USE_LCD_RW 
output_low(LCD_RW); 
delay_cycles(1); 
#endif 

output_low(LCD_E); 

lcd_send_nibble(n >> 4); 
lcd_send_nibble(n & 0xf); 
} 

//---------------------------- 
void lcd_init(void) 
{ 
int8 i; 

output_low(LCD_RS); 

#ifdef USE_LCD_RW 
output_low(LCD_RW); 
#endif 

output_low(LCD_E); 

// Some LCDs require 15 ms minimum delay after 
// power-up.  Others require 30 ms.  I'm going 
// to set it to 35 ms, so it should work with 
// all of them. 
delay_ms(35);      

for(i=0 ;i < 3; i++) 
   { 
    lcd_send_nibble(0x03); 
    delay_ms(5); 
   } 

lcd_send_nibble(0x02); 

for(i=0; i < sizeof(LCD_INIT_STRING); i++) 
   { 
    lcd_send_byte(0, LCD_INIT_STRING[i]); 
    
    // If the R/W signal is not used, then 
    // the busy bit can't be polled.  One of 
    // the init commands takes longer than 
    // the hard-coded delay of 60 us, so in 
    // that case, lets just do a 5 ms delay 
    // after all four of them. 
    #ifndef USE_LCD_RW 
    delay_ms(5); 
    #endif 
   } 

lcd_xcoord = 1; 
} 

//---------------------------- 
// The x-coordinate can be 1 to 16. 
// The y coordinate is ignored.  
// This x,y interface is kept in order to be 
// consistent with other CCS LCD drivers. 
void lcd_gotoxy(int8 x, int8 y) 
{ 
int8 address; 

// Update the global x-coordinate variable with the 
// current x coordinate. 
lcd_xcoord = x; 

// Convert the x-coordinate from CCS format (1-16) to 
// the 0-15 format used by the LCD hardware. 
address = x - 1;  

// If the x-coordinate is within the 2nd half of the 
// LCD line, the address must be adjusted because 
// of the special architecture of the 8x2 LCD. 
if(address >= LCD_HALF_WIDTH)  
  {    
   address += (LCD_2ND_HALF_ADDRESS - LCD_HALF_WIDTH); 
  } 

lcd_send_byte(0, 0x80 | address); 
} 


//----------------------------- 
void lcd_putc(char c) 
{ 
 switch(c) 
   { 
    case '\f': 
      lcd_send_byte(0,1); 
      delay_ms(2); 
      lcd_xcoord = 1; 
      break; 
    
    case '\n': 
       lcd_gotoxy(1,1);  //  Goto start of line 1 
       break; 
    
    case '\b': 
       lcd_send_byte(0, 0x10); 
       lcd_xcoord--; 
       if(lcd_xcoord == LCD_HALF_WIDTH) 
          lcd_gotoxy(LCD_HALF_WIDTH, 1); 
       break; 
    
    default: 
       lcd_send_byte(1, c); 
       lcd_xcoord++; 
       if(lcd_xcoord == (LCD_HALF_WIDTH +1)) 
          lcd_gotoxy(LCD_HALF_WIDTH +1, 1); 
       break; 
   } 
} 

//------------------------------ 
#ifdef USE_LCD_RW 
char lcd_getc(int8 x, int8 y) 
{ 
char value; 

lcd_gotoxy(x,y); 

// Wait until busy flag is low. 
while(bit_test(lcd_read_byte(),7));  

output_high(LCD_RS); 
value = lcd_read_byte(); 
output_low(lcd_RS); 

return(value); 
} 
#endif 
