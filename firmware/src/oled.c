
#include "oled.h"
#include "charset.h"
#include "logo.h"
#include "i2c.h"
#include <stdlib.h>
#include "string.h"

#define false 0
#define true  1

unsigned char ssd1306_contr;
unsigned char ssd1306_inv;
unsigned char ssd1306_under;
unsigned char ssd1306_over;

void  ssd1306_command(int c)
{
    int control = 0x00; // some use 0X00 other examples use 0X80. I tried both 
    i2c_start((SSD1306_ADDRESS<<1) | I2C_WRITE);
    i2c_write(control); // This is Command
    i2c_write(c);
    i2c_stop();
}

void  ssd1306_data(unsigned char c)
{
    i2c_start((SSD1306_ADDRESS<<1) | I2C_WRITE);
    i2c_write(0X40); // This byte is DATA
    i2c_write(c);
    i2c_stop();
}

void ssd1306_setColAddress(int x)
{
  ssd1306_command(SSD1306_COLUMNADDR); // 0x21 COMMAND
  ssd1306_command(x); // Column start address
  ssd1306_command(SSD1306_LCDWIDTH-1); // Column end address
}
void ssd1306_setPageAddress(int y)
{
  ssd1306_command(SSD1306_PAGEADDR); // 0x22 COMMAND
  ssd1306_command(y); // Start Page address
  ssd1306_command((SSD1306_LCDHEIGHT/8)-1);// End Page address
}
void ssd1306_transferBuffer(unsigned char* display_buffer, int siz)
{ 
      ssd1306_setColAddress(0);
      ssd1306_setPageAddress(0);       
      i2c_start((SSD1306_ADDRESS<<1) | I2C_WRITE);
      i2c_write(0X40); 
      for(int j=0;j<siz;j++)
      {
        i2c_write(display_buffer[j]);
      }
      i2c_stop();
}

void ssd1306_contrast(unsigned char value)
{
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(value);
    ssd1306_contr = value;
}

void ssd1306_init()
{
    i2c_init();
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE   
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80   
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3F); 
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset  
    ssd1306_command(SSD1306_SETSTARTLINE);// | 0x0);        // line #0   
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    ssd1306_command(0x14);                                  // using internal VCC 
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x00 horizontal addressing  
    ssd1306_command(SSD1306_SEGREMAP | 0x1);                // rotate screen 180 
    ssd1306_command(SSD1306_COMSCANDEC);                    // rotate screen 180  
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);  
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0xCF);  
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    ssd1306_command(0xF1);  
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);  
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4  
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
    ssd1306_command(SSD1306_DISPLAYON);                     //switch on OLED
    ssd1306_contr = 0xCF;
	lcd_contrast = ssd1306_contr;
    ssd1306_inv = 0;
    ssd1306_under = 0;
    ssd1306_over = 0;
}
void ssd1306_clear()
{
  ssd1306_setColAddress(0);
  ssd1306_setPageAddress(0);       
  i2c_start((SSD1306_ADDRESS<<1) | I2C_WRITE);
  i2c_write(0X40); 
  for(int j=0;j<1024;j++) i2c_write(0);
  i2c_stop(); 
}

void ssd1306_char(unsigned char c)
{
  i2c_start((SSD1306_ADDRESS<<1) | I2C_WRITE);
  i2c_write(0X40);
  
  for (int index = 0; index < 5; index++)
  {
    unsigned char ascii = (unsigned char)pgm_read_byte(&ASCII[c - 0x20][index]);
    if(ssd1306_inv) ascii = 255-ascii;
    ascii = ascii | (1<<7)*(ssd1306_under==true) | (ssd1306_over==true);
    
    i2c_write(ascii);
  }
  i2c_write(255*(ssd1306_inv==true) | (1<<7)*(ssd1306_under==true) | (ssd1306_over==true));
  i2c_stop();
}
void ssd1306_string(char *string)
{
  while (*string)
  {
    ssd1306_char(*string++);
  }
}

void ssd1306_screenUp()
{
    ssd1306_command(SSD1306_SEGREMAP | 0x1);                
    ssd1306_command(SSD1306_COMSCANDEC);                   
    ssd1306_clear();
}
void ssd1306_screenDown()
{
    ssd1306_command(SSD1306_SEGREMAP | 0x0);                
    ssd1306_command(SSD1306_COMSCANINC);
    ssd1306_clear();
}
void ssd1306_inverse() {ssd1306_inv=!ssd1306_inv;}
void ssd1306_underline() {ssd1306_under=!ssd1306_under;}
void ssd1306_overline() {ssd1306_over=!ssd1306_over;}
	
void lcd_init()
{
	ssd1306_init();
}	
void lcd_clear()
{
	ssd1306_clear();
}
void lcd_gotoxy(unsigned char x, unsigned char y)
{	
	ssd1306_setPageAddress(y);
	ssd1306_setColAddress(x);
}
void lcd_put_s(char *str)
{
	ssd1306_string(str);
}
void lcd_put_i(unsigned int value)
{
	char buffer[10];
	itoa(value,buffer,10);
	ssd1306_string(buffer);
}
void lcd_put_l(unsigned long int value)
{
	char buffer[10];
	ltoa(value,buffer,10);
	ssd1306_string(buffer);
}
void lcd_put_p(const prog_char *progmem_s)
{
	register char c;
	while ( (c = pgm_read_byte(progmem_s++)) )  ssd1306_char(c);
}
void lcd_icon(int icon)
{
	for (int index = 0; index < 12; index++)
	{
		unsigned char ascii = 0;
		if(icon!=255) ascii = (unsigned char)pgm_read_byte(&ICONS[icon][index]);
		if(ssd1306_inv) ascii = 255-ascii;
		ascii = ascii | (1<<7)*(ssd1306_under==true) | (ssd1306_over==true);
		ssd1306_data(ascii);
	}
	ssd1306_data(255*(ssd1306_inv==true)  | (1<<7)*(ssd1306_under==true) | (ssd1306_over==true) );
}
void lcd_inverse() {ssd1306_inverse();}
void lcd_underline() {ssd1306_underline();}
void lcd_overline() {ssd1306_overline();}
void logo()
{
	for(int i = 0;i<5;i++)
	{
		lcd_gotoxy(25,0+i);
		for(int j = 0; j<74; j++)
		{
			unsigned char D = (unsigned char)pgm_read_byte(&LOGO[i*74+j]);
			ssd1306_data(D);
		}
	}
}
