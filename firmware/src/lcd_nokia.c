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
#include "lcd_nokia.h"
#include "charset.h"
#include <util/delay.h>

#define MSBFIRST 0
#define LSBFIRST 1

void lcd_send(int pin, int value)
{
	if(value == 0)
	{
		LCD_PORT &= ~(1<<pin);
	}
	if(value == 1)
	{
		LCD_PORT |= (1<<pin);
	}
	//_delay_us(1);
}

void lcd_send_out(unsigned char dataPin, unsigned char clockPin, unsigned char bitOrder, unsigned char val)
{
	unsigned char i;
	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST) lcd_send(dataPin, !!(val & (1 << i)));
		else  lcd_send(dataPin, !!(val & (1 << (7 - i))));
		
		lcd_send(clockPin, HIGH);
		lcd_send(clockPin, LOW);
	}
}

void lcd_write(unsigned char dc, unsigned char data)
{
	PORTD |= _BV(1);	// SD CS=1 - SD Desabilitado
	
	lcd_send(LCD_DC_PIN, dc);
	lcd_send(LCD_SCE_PIN, LOW);
	lcd_send_out(LCD_SDIN_PIN, LCD_SCLK_PIN, MSBFIRST, data);
	lcd_send(LCD_SCE_PIN, HIGH);
}

// LCD init
void lcd_init()
{
	lcd_contrast = 0x2f;
	inverse = false;
	under = false;
	over = false;

	LCD_DDR = LCD_DDR | 1 << LCD_SCE_PIN;
	LCD_DDR = LCD_DDR | 1 << LCD_DC_PIN;
	LCD_DDR = LCD_DDR | 1 << LCD_SDIN_PIN;
	LCD_DDR = LCD_DDR | 1 << LCD_SCLK_PIN;
	lcd_config();
}

void lcd_config()
{
	PORTD |= _BV(1);	// SD CS=1 - SD Desabilitado
	lcd_write( LCD_CMD, 0x21 );  // LCD Extended Commands.
	lcd_write( LCD_CMD, lcd_contrast | (1<<7) );  // Set LCD Vop (Contrast). //B1
	lcd_write( LCD_CMD, 0x04 );  // Set Temp coefficent. //0x04
	lcd_write( LCD_CMD, 0x14 );  // LCD bias mode 1:48. //0x13
	lcd_write( LCD_CMD, 0x0C );  // LCD in normal mode. 0x0d for inverse
	lcd_write( LCD_C, 0x20);
	lcd_write( LCD_C, 0x0C);
}

void lcd_border(void)
{
	unsigned char  j;
	for(j=0; j<84; j++) // top
	{
		lcd_gotoxy(j,0);
		lcd_write(1,0x01);
	}
	for(j=0; j<84; j++) //Bottom
	{
		lcd_gotoxy(j,5);
		lcd_write(1,0x80);
	}

	for(j=0; j<6; j++) // Right
	{
		lcd_gotoxy(83,j);
		lcd_write(1,0xff);
	}
	for(j=0; j<6; j++) // Left
	{
		lcd_gotoxy(0,j);
		lcd_write(1,0xff);
	}

}

// clear lcd
void lcd_clear()
{
	for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
	{
		lcd_write(LCD_D, 0x00);
	}
}

// Goto X,Y
void lcd_gotoxy(unsigned char x, unsigned char y)
{
	lcd_write( 0, 0x80 | x);  // Column.
	lcd_write( 0, 0x40 | y);  // Row.
}

void lcd_icon(int icon)
{
	for (int index = 0; index < 12; index++)
	{
		unsigned char ascii = 0;
		if(icon!=255) ascii = (unsigned char)pgm_read_byte(&ICONS[icon][index]);
		if(inverse) ascii = 255-ascii;
		ascii = ascii | (1<<7)*(under==true) | (over==true);
		lcd_write(LCD_D, ascii);
	}
	lcd_write(LCD_D, 255*(inverse==true)  | (1<<7)*(under==true) | (over==true) );
}
// output a character on LCD
void lcd_char(unsigned char c)
{
	for (int index = 0; index < 5; index++)
	{
		unsigned char ascii = (unsigned char)pgm_read_byte(&ASCII[c - 0x20][index]);
		if(inverse) ascii = 255-ascii;
		ascii = ascii | (1<<7)*(under==true) | (over==true);
		lcd_write(LCD_D, ascii);
	}
	lcd_write(LCD_D, 255*(inverse==true) | (1<<7)*(under==true) | (over==true));
}
void lcd_put_s(char *str)
{
	register char c;

	while( (c = *(str++))) lcd_char(c);
}
void lcd_put_i(unsigned int value)
{
	char buffer[10];
	itoa(value,buffer,10);
	lcd_put_s(buffer);
}
void lcd_put_l(unsigned long int value)
{
	char buffer[10];
	ltoa(value,buffer,10);
	lcd_put_s(buffer);
}
void lcd_put_p(const prog_char *progmem_s)
{
	register char c;
	while ( (c = pgm_read_byte(progmem_s++)) )  lcd_char(c);
}
void lcd_inverse() {inverse=!inverse;}
void lcd_underline() {under=!under;}
void lcd_overline() {over=!over;}