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

#ifndef CONFIG_H_
#define CONFIG_H_

#define nop() __asm__ __volatile__ ("nop")
#define clear_bit(a,z) (a &= ~_BV(z))
#define set_bit(a,z) (a |= _BV(z))

/*
This is the original SDISK II version from Nishida that was updated
to receive a 16x2 LCD from Victor Trucco and Fabio Belavenuto

DO NOT CHANGE THIS unless you really know what you are doing
*/
#ifdef _SDISK_ORIGINAL_

	#define _LCD_

	// LCD pins
	#define LCD_PORT PORTC
	#define LCD_PORTD DDRC
	#define LCD_ENABLE_PIN 5
	#define LCD_INSTRUCTION_PIN 4

	// SPI (SD SLOT) pins
	#define SPI_MISO  0
	#define SPI_CLOCK 5
	#define SPI_MOSI  4
	#define SPI_CS    1
	#define SD_EJECT  3
	#define SPI_DDR   DDRD
	#define SPI_PIN   PIND
	#define SPI_PORT  PORTD
	#define _CLK_DI_CS	0b00110010
	#define _CLKNDI_CS	0b00100010
	#define NCLK_DI_CS	0b00010010
	#define NCLKNDI_CS	0b00000010
	#define _CLK_DINCS	0b00110000
	#define _CLKNDINCS	0b00100000
	#define NCLK_DINCS	0b00010000
	#define NCLKNDINCS	0b00000000

	// SD LED
	#define SD_LED_PORT PORTB
	#define SD_LED_PORTD DDRB
	#define SD_LED 4

	// BOTOES
	#define ENTER_PORT       PIND
	#define DOWN_PORT        PIND
	#define UP_PORT          PINB
	#define ENTER_PORTD      PORTD
	#define DOWN_PORTD       PORTD
	#define UP_PORTD         PORTB
	#define ENTER_BIT        DDD6
	#define DOWN_BIT         DDD7
	#define UP_BIT           DDB5

	// PINOS PARA A DISK II - APPLE
	#define DISKII_PIN       PINC
	#define DISKII_ENABLE    DDC0


#endif

/*
This is the original SDISK II version from Nishida that was modified
by Alexandre Suaide to receive a NOKIA 5110 Graphical LCD display.
It is still based on the original hardware, just replaced the 16x2 LCD
by the Nokia LCD

DO NOT CHANGE THIS unless you really know what you are doing
*/
#ifdef _SDISK_ORIGINAL_MOD_NOKIA_

	#define _LCD_NOKIA_

	// LCD pins
	#define LCD_PORT         PORTC
	#define LCD_DDR          DDRC
	#define LCD_SCE_PIN      5
	#define LCD_RESET_PIN    VCC
	#define LCD_DC_PIN       3
	#define LCD_SDIN_PIN     1
	#define LCD_SCLK_PIN     4

	// SPI (SD SLOT) pins
	#define SPI_MISO  0
	#define SPI_CLOCK 5
	#define SPI_MOSI  4
	#define SPI_CS    1
	#define SD_EJECT  3
	#define SPI_DDR   DDRD
	#define SPI_PIN   PIND
	#define SPI_PORT  PORTD
	#define _CLK_DI_CS	0b00110010
	#define _CLKNDI_CS	0b00100010
	#define NCLK_DI_CS	0b00010010
	#define NCLKNDI_CS	0b00000010
	#define _CLK_DINCS	0b00110000
	#define _CLKNDINCS	0b00100000
	#define NCLK_DINCS	0b00010000
	#define NCLKNDINCS	0b00000000

	// SD LED
	#define SD_LED_PORT PORTB
	#define SD_LED_PORTD DDRB
	#define SD_LED 4

	// BOTOES
	#define ENTER_PORT       PIND
	#define DOWN_PORT        PIND
	#define UP_PORT          PINB
	#define ENTER_PORTD      PORTD
	#define DOWN_PORTD       PORTD
	#define UP_PORTD         PORTB
	#define ENTER_BIT        DDD6
	#define DOWN_BIT         DDD7
	#define UP_BIT           DDB5

	// PINOS PARA A DISK II - APPLE
	#define DISKII_PIN       PINC
	#define DISKII_ENABLE    DDC0


#endif

/*
This is the second version of SDISK II built by Victor Trucco
with the Nokia 5110 graphical LCD display with USB connection
for firmware update.

DO NOT CHANGE THIS unless you really know what you are doing
*/
#ifdef _SDISK_NOKIA_

	#define _LCD_NOKIA_

	// LCD pins
	#define LCD_PORT         PORTC
	#define LCD_DDR          DDRC
	#define LCD_SCE_PIN      5
	#define LCD_RESET_PIN    VCC
	#define LCD_DC_PIN       4
	#define LCD_SDIN_PIN     3
	#define LCD_SCLK_PIN     1

	// SPI (SD SLOT) pins
	#define SPI_MISO  0
	#define SPI_CLOCK 5
	#define SPI_MOSI  3 //DI
	#define SPI_CS    4
	#define SPI_SW	  1
	#define SD_EJECT  SPI_SW
	#define SPI_DDR   DDRD
	#define SPI_PIN   PIND
	#define SPI_PORT  PORTD
	#define _CLK_DI_CS	0b00111000
	#define _CLKNDI_CS	0b00110000
	#define NCLK_DI_CS	0b00011000
	#define NCLKNDI_CS	0b00010000
	#define _CLK_DINCS	0b00101000
	#define _CLKNDINCS	0b00100000
	#define NCLK_DINCS	0b00001000
	#define NCLKNDINCS	0b00000000

	// SD LED
	#define SD_LED_PORT PORTB
	#define SD_LED_PORTD DDRB
	#define SD_LED 5

	// BOTOES
	#define ENTER_PORT       PIND
	#define DOWN_PORT        PIND
	#define UP_PORT          PINB
	#define ENTER_PORTD      PORTD
	#define DOWN_PORTD       PORTD
	#define UP_PORTD         PORTB
	#define ENTER_BIT        DDD6
	#define DOWN_BIT         DDD7
	#define UP_BIT           DDB4

	// PINOS PARA A DISK II - APPLE
	#define DISKII_PIN       PINC
	#define DISKII_ENABLE    DDC0
#endif

#endif /* CONFIG_H_ */
