/*------------------------------------
SDISK II LCD Firmware
2010.11.11 by Koichi Nishida
2012.01.26 by Fábio Belavenuto
2015.07.02 by Alexandre Suaide
-------------------------------------
*/

/*
2015.07.02 by Alexandre Suaide
Added support for SDHC cards and subdirectories
Removed DSK to NIC conversion
FAT16 and FAT32 disks should have at least 64 blocks per cluster
*/

/*
2012.01.26 by Fábio Belavenuto
Added support for image exchange using a button added in the Brazilian version by Victor Trucco
Added support for a 16x2 LCD
*/

/*
This is a part of the firmware for DISK II emulator by Nishida Radio.
Copyright (C) 2010 Koichi NISHIDA
email to Koichi NISHIDA: tulip-house@msf.biglobe.ne.jp
This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#ifndef __LCD_NOKIA__
#define __LCD_NOKIA__

#define false 0
#define true  1
#define LOW 0
#define HIGH 1
#define MIN_CONTRAST 30
#define MAX_CONTRAST 60

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "string.h"

unsigned char lcd_contrast;
unsigned char inverse;
unsigned char under;
unsigned char over;

void lcd_write(unsigned char dc, unsigned char data);
void lcd_icon(int icon);
void lcd_char(unsigned char c);
void lcd_init();
void lcd_config();
void lcd_border(void);
void lcd_clear();
void lcd_char(unsigned char c);
void lcd_clear();
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_put_s(char *str);
void lcd_put_i(unsigned int value);
void lcd_put_l(unsigned long int value);
void lcd_put_p(const prog_char *progmem_s);
void lcd_icon(int icon);
void lcd_inverse();
void lcd_underline();
void lcd_overline();

/* Definições para o LCD */

#define LCD_C     LOW
#define LCD_D     HIGH

#define LCD_X     84
#define LCD_Y     48
#define LCD_CMD   0

#define LCD_ENABLE  				LCD_PORT |= _BV(LCD_SCE_PIN)
#define LCD_DISABLE 				LCD_PORT &=~_BV(LCD_SCE_PIN)
#define LCD_INSTRUCTION				LCD_PORT &=~_BV(LCD_SCLK_PIN)
#define LCD_DATA					LCD_PORT |= _BV(LCD_SCLK_PIN)

#endif