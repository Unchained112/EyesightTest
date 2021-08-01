/* 
 * File:   lcd.h
 * Author: 12591
 *
 * Created on July 31, 2021, 6:39 PM
 */

#ifndef LCD_H
#define	LCD_H

#include <xc.h>

#define sclk PORTEbits.RE0
#define sid PORTEbits.RE1 // sda
#define rs PORTEbits.RE2
#define reset PORTEbits.RE3
#define cs1 PORTEbits.RE4


#define uchar unsigned char 
#define uint unsigned int
#define ulong unsigned long

int read_lcd_dir();

//void initial_lcd();
//void disp_rand(char size);
//void clear_screen();
//void display();


#endif	/* LCD_H */

