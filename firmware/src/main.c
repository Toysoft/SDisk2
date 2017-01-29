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

#include "main.h"

#define BUF_NUM 5
unsigned char writeData[BUF_NUM][350];
unsigned char sectors[BUF_NUM], tracks[BUF_NUM];
unsigned char buffNum;
unsigned char *writePtr;

#define MAXFILES 200
int *files_id;
int nfiles  = 0;
int selected_file_id = 0;
int id_of_config_file = -1;

#define FAT_NIC_ELEMS 10
#define NIC_FILE_SIZE 280
unsigned long fatNic[FAT_NIC_ELEMS];

// DISK II status
unsigned char       ph_track;					// 0 - 139
unsigned char       sector;	    				// 0 - 15
unsigned short      bitbyte;					// 0 - (8*512-1)
unsigned char       prepare;
unsigned char       readPulse;
unsigned char       inited;
unsigned char       magState;
unsigned char       protect;
unsigned char       formatting;
const unsigned char volume = 0xfe;

unsigned char  trk;
unsigned short long_sector;
unsigned short long_cluster;
unsigned long  ft;
unsigned char  old_trk;

/*Error codes
1 - Can not put SD Card on idle state
2 - Can not make SD Card initialization
3 - Can not init FAT
4 - Partition type not supported (only FAT16 and FAT32)
5 - Cluster size smaller than minimum to keep buffer
*/


/*                               1234567890123456 */
#ifdef _LCD_
PROGMEM const char SPLASH1[]  = "SDISK2 LCD v.4.1";
PROGMEM const char SPLASH2[]  = "  Apple ][ BR   ";
PROGMEM const char NIC[]      = "NIC: ";
PROGMEM const char EMP[]      = "                ";
PROGMEM const char SPE0[]     = "Delay: ";
PROGMEM const char SPE1[]     = "Select delay:  ";
PROGMEM const char TRAK[]     = "TR: ";
PROGMEM const char MSG7[]     = "Select NIC image";
PROGMEM const char MSGC[]     = "Building list...";
PROGMEM const char MSG8[]     = " Nothing here!  ";
PROGMEM const char MSGD[]     = "Sorting files...";
#define TRX 10
#define TRY 1
#endif

#ifdef _LCD_NOKIA_
/*                               12345678901234 */
PROGMEM const char SPLASH1[]  = "SDisk ][";
PROGMEM const char SPLASH2[]  = "Apple ][ - BR";
PROGMEM const char VERSION[]  = "v. 4.1 (2017)";
PROGMEM const char NIC[]      = " NIC          ";
PROGMEM const char SETUP[]    = " SETUP        ";
PROGMEM const char EMP[]      = "              ";
PROGMEM const char SET1[]     = " Set SD delay ";
PROGMEM const char SET2[]     = " Set contrast ";
PROGMEM const char VALUE[]    = " Value: ";
PROGMEM const char CONT[]     = " LCD CONTRAST ";
PROGMEM const char DLAY[]     = " SD DELAY     ";
PROGMEM const char TRAK[]     = "Track: ";
PROGMEM const char MSG7[]     = " SELECT NIC   ";
PROGMEM const char MSGC[]     = " Building list";
PROGMEM const char MSG8[]     = " Nothing here!";
PROGMEM const char MSGD[]     = " Sorting files";

#define TRX 12
#define TRY 3
#endif

PROGMEM const char ERR[]      = "E R R O R";
PROGMEM const char ERM1[]     = "SD can't idle ";
PROGMEM const char ERM2[]     = "SD can't init ";
PROGMEM const char ERM3[]     = "Boot sector   ";
PROGMEM const char ERM4[]     = "Not FAT16/32  ";
PROGMEM const char ERM5[]     = "Cluster < 32kB";
PROGMEM const char ERM6[]     = "Missing MBR   ";
PROGMEM const char ERM7[]     = "SD Block ERROR";
PROGMEM const char ERMC[]     = "Code: ";
PROGMEM const char MSHC[]     = "   SDHC card  ";
PROGMEM const char MSSD[]     = "Normal SD card";
PROGMEM const char PART[]     = "Type: ";
PROGMEM const char FAT3[]     = "FAT32";
PROGMEM const char FAT1[]     = "FAT16";

PROGMEM const char SDOUT[]    = "No SD inserted";




PROGMEM const char CFG[]  = "SDISKII CFG";

// a table for head stepper motor movement
PROGMEM const prog_uchar stepper_table[4] = {0x0f,0xed,0x03,0x21};

int main(void)
{
	static unsigned char oldStp = 0, stp;
	init(1);
	old_trk = 255;
	while(1)
	{
		verify_status();
		if(diskII_disable())
		{
			SD_led_off();
		}
		else
		{
			SD_led_on();
			stp = (PINB & 0b00001111);
			if (stp != oldStp)
			{
				oldStp = stp;
				unsigned char ofs = ((stp==0b00001000) ? 2 : ((stp==0b00000100) ? 4 : ((stp==0b00000010) ? 6 : ((stp==0b00000001) ? 0 : 0xff))));
				if (ofs != 0xff)
				{
					ofs = ((ofs+ph_track)&7);
					unsigned char bt = pgm_read_byte_near(stepper_table + (ofs >> 1));
					oldStp = stp;
					if (ofs & 1) bt &= 0x0f;
					else bt >>= 4;
					ph_track += ((bt & 0x08) ? (0xf8 | bt) : bt);
					if (ph_track > 196) ph_track = 0;
					if (ph_track > 139) ph_track = 139;
				}
			}
			if (inited && prepare)
			{
				cli();
				sector = ((sector + 1) & 0xf);
				{
					trk = (ph_track >> 2);
					long_sector = (unsigned short)trk*16 + sector;
					long_cluster = long_sector>>FAT_sectorsPerClusterBitShift;
					ft = fatNic[long_cluster];
					if(trk!=old_trk)
					{
						old_trk = trk;
						lcd_gotoxy(TRX,TRY);
						lcd_put_p(TRAK);
						if(trk<10) lcd_put_s(" ");
						lcd_put_i((unsigned int)trk);
					}
					
					if (((sectors[0]==sector)&&(tracks[0]==trk)) || ((sectors[1]==sector)&&(tracks[1]==trk)) ||
					((sectors[2]==sector)&&(tracks[2]==trk)) || ((sectors[3]==sector)&&(tracks[3]==trk)) ||
					((sectors[4]==sector)&&(tracks[4]==trk))
					) writeBackSub();
					
					// Timing is an issue here. This is the reason I copied explicitly the conversion from cluster to block
					// instead of using getCluster() method. I also did bit shift instead of multiplications to make it faster.
					// even inline function is not fast enough.
					// SD_CMD17Special is also faster than the regular SD_sendCommand method.
					SD_CMD17Special((((ft-2)<<FAT_sectorsPerClusterBitShift)+FAT_firstDataSector) + (long_sector&(FAT_sectorsPerCluster-1)));
					bitbyte = 0;
					prepare = 0;
				}
				
				sei();
			}
		}
	}
}
void display_sd_ejected()
{
	lcd_clear();
	#ifdef _LCD_
	lcd_gotoxy(0,0);
	lcd_put_p(SDOUT);
	lcd_gotoxy(1,1);
	lcd_put_p(EMP);
	#endif
	#ifdef _LCD_NOKIA_
	lcd_gotoxy(15,1);
	lcd_put_p(ERR);
	lcd_gotoxy(0,3);
	lcd_put_p(SDOUT);
	#endif
	do {} while(SD_ejected());
	return;
}
void init_sd(char splash)
{
	id_of_config_file = -1;
	SPI_slow();
	SD_select_card();
	unsigned char ok;
	if(SD_ejected())
	{
		display_sd_ejected();
		return;
	}
	
	buffer = &writeData[0][0];
	ok = SD_init();
	
	if(!ok)
	{
		lcd_clear();
		#ifdef _LCD_
		lcd_gotoxy(0,0);
		lcd_put_p(ERR);
		lcd_gotoxy(0,1);
		#endif
		#ifdef _LCD_NOKIA_
		lcd_gotoxy(15,1);
		lcd_put_p(ERR);
		lcd_gotoxy(0,3);
		#endif
		if (errorCode==1) lcd_put_p(ERM1);
		else if (errorCode==2) lcd_put_p(ERM2);
		else if (errorCode==3) lcd_put_p(ERM3);
		else if (errorCode==4) lcd_put_p(ERM4);
		else if (errorCode==5) lcd_put_p(ERM5);
		else if (errorCode==6) lcd_put_p(ERM6);
		else if (errorCode==7) lcd_put_p(ERM7);
		else { lcd_put_p(ERMC); lcd_put_i(errorCode);}
		while(1)
		{
			inited = 0;
			if(SD_ejected()) return;
		};
	}
	if(splash)
	{
		lcd_clear();
		#ifdef _LCD_
		lcd_gotoxy(1,0);
		#endif
		#ifdef _LCD_NOKIA_
		lcd_gotoxy(0,1);
		#endif
		if(SD_version==SD_RAW_SPEC_SDHC) lcd_put_p(MSHC);
		else lcd_put_p(MSSD);
		SD_select_card();
		_delay_ms(1000);
	}
	
	ok = FAT_init();
	if(!ok)
	{
		lcd_clear();
		#ifdef _LCD_
		lcd_gotoxy(0,0);
		lcd_put_p(ERR);
		lcd_gotoxy(0,1);
		#endif
		#ifdef _LCD_NOKIA_
		lcd_gotoxy(15,1);
		lcd_put_p(ERR);
		lcd_gotoxy(0,3);
		#endif
		if (errorCode==1) lcd_put_p(ERM1);
		else if (errorCode==2) lcd_put_p(ERM2);
		else if (errorCode==3) lcd_put_p(ERM3);
		else if (errorCode==4) lcd_put_p(ERM4);
		else if (errorCode==5) lcd_put_p(ERM5);
		else if (errorCode==6) lcd_put_p(ERM6);
		else if (errorCode==7) lcd_put_p(ERM7);
		else { lcd_put_p(ERMC); lcd_put_i(errorCode);}
		while(1)
		{
			inited = 0;
			if(SD_ejected()) return;
		};
	}
	
	if((FAT_bytesPerSector*FAT_sectorsPerCluster/1024) < (NIC_FILE_SIZE/FAT_NIC_ELEMS))
	{
		errorCode = 5;
		lcd_clear();
		#ifdef _LCD_
		lcd_gotoxy(0,0);
		lcd_put_p(ERR);
		lcd_gotoxy(0,1);
		#endif
		#ifdef _LCD_NOKIA_
		lcd_gotoxy(15,1);
		lcd_put_p(ERR);
		lcd_gotoxy(0,3);
		#endif
		lcd_put_p(ERM5);
		while(1)
		{
			inited = 0;
			if(SD_ejected()) return;
		};
	}
	
	if(splash)
	{
		#ifdef _LCD_
		lcd_gotoxy(0,1);
		#endif
		#ifdef _LCD_NOKIA_
		//lcd_border();
		lcd_gotoxy(9,4);
		#endif
		lcd_put_p(PART);
		if(FAT_partitionType==PARTITION_TYPE_FAT32) lcd_put_p(FAT3);
		else lcd_put_p(FAT1);
		_delay_ms(1000);
		SD_select_card();
	}
	
	buffClear();
	SPI_fast();
}
void init(char splash)
{
	// configuracao das portas do microcontrolador
	
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	
	// configuração do LED
	set_bit(SD_LED_PORTD,SD_LED);
	set_bit(SD_LED_PORT,SD_LED); // LED ligado
	
	// configuração das portas do LCD da SDISK original
	#ifdef _LCD_
	DDRC = 0b00111010;  /* PC1 = READ PULSE/LCD D4, PC3 = WRITE PROTECT/LCD D5, PC4 = LCD RS, PC5 = LCD E */
	DDRD = 0b00110010;  /* PD1 = SD CS, PD4 = SD DI/LCD D6, PD5 = SD SCK/LCD D7 */
	PORTC = 0b00000010; /* PC4=0 - LCD RS, PC5=0 - LCD Desabilitado */
	PORTD = 0b00000010; /* PD1=0 - SD Desabilitado */
	#endif
	
	// configuração da interface com o Apple
	
	set_bit(DDRC, 1);
	set_bit(DDRC, 3);
	set_bit(PORTC, 1);

	// configuração da interrupção de tempo
	OCR0A = 0;
	TCCR0A = 0;
	TCCR0B = 1;

	// configuração da interrupção int0
	MCUCR = 0b00000010;
	EICRA = 0b00000010;

	sector = 0;
	inited = 0;
	readPulse = 0;
	protect = 0;
	bitbyte = 0;
	magState = 0;
	prepare = 1;
	ph_track = 0;
	buffNum = 0;
	formatting = 0;
	writePtr = &(writeData[buffNum][0]);
	
	if(splash)
	{
		lcd_init();
		lcd_clear();
		lcd_gotoxy(0,0);
		#ifdef _LCD_
		lcd_put_p(SPLASH1);
		lcd_gotoxy(0,1);
		lcd_put_p(SPLASH2);
		#endif
		#ifdef _LCD_NOKIA_
		lcd_border();
		lcd_gotoxy(18,1);
		lcd_put_p(SPLASH1);
		lcd_gotoxy(3,2);
		lcd_put_p(SPLASH2);
		lcd_gotoxy(3,4);
		lcd_put_p(VERSION);
		#endif
		_delay_ms(1000);
	}
}

void verify_status(void)
{
	unsigned int i;
	configButtons();
	if (SD_ejected())
	{
		for (i = 0; i != 0x500; i++) if (!SD_ejected()) return;
		TIMSK0 &= ~(1<<TOIE0);
		EIMSK &= ~(1<<INT0);
		inited = 0;
		prepare = 0;
		display_sd_ejected();
		return;
	}
	else if(diskII_disable() && (enter_is_pressed() || down_is_pressed() || up_is_pressed()))
	{
		if(enter_is_pressed())  // drive disabled
		{
			unsigned char flg = 1;
			for (i = 0; i != 100; i++) if (!enter_is_pressed()) flg = 0;
			if (flg)
			{
				while (enter_is_pressed());  // enter button pushed !
				cli();
				select_nic();
				if(SD_ejected()) return;
				if (inited)
				{
					TIMSK0 |= (1<<TOIE0);
					EIMSK |= (1<<INT0);
				}
				sei();
			}
			return;
		}
		else if(down_is_pressed()) //setup
		{
			unsigned char flg = 1;
			for (i = 0; i != 100; i++) if (!down_is_pressed()) flg = 0;
			if (flg)
			{
				while (down_is_pressed());  // enter button pushed !
				cli();
				#ifdef _LCD_
				set_speed();
				#endif
				#ifdef _LCD_NOKIA_
				setup();
				#endif
				if(SD_ejected()) return;
				find_previous_nic();
				if (inited)
				{
					TIMSK0 |= (1<<TOIE0);
					EIMSK |= (1<<INT0);
				}
				sei();
			}
			return;
		}
		else if(up_is_pressed())
		{
			unsigned char flg = 1;
			for (i = 0; i != 100; i++) if (!up_is_pressed()) flg = 0;
			if(flg)
			{
				while (up_is_pressed()); // botao ainda apertado
				cli();
				swap_nic();
				if(SD_ejected()) return;
				if(inited)
				{
					TIMSK0 |= (1<<TOIE0);
					EIMSK |= (1<<INT0);
				}
				sei();
			}
		}
	}
	else if(!inited) // if not initialized
	{
		for (i = 0; i != 0x500; i++)  if (SD_ejected()) return;
		cli();
		init_sd(1);
		find_previous_nic();
		if (inited)
		{
			TIMSK0 |= (1<<TOIE0);
			EIMSK |= (1<<INT0);
		}
		sei();
	}
}
#ifdef _LCD_NOKIA_
void set_contrast()
{
	unsigned char contrast = 0XAF;
	if(id_of_config_file!=-1)
	{
		cd(0);
		buffer = &writeData[0][0];
		buffClear();
		lastBlockRead = 0;
		struct dir_Structure* config_file = getFile(id_of_config_file);
		unsigned long cluster;
		
		if(FAT_partitionType == PARTITION_TYPE_FAT32) cluster = (unsigned long)config_file->firstClusterHI<<16 | (unsigned long)config_file->firstClusterLO;
		else cluster = (unsigned long)config_file->firstClusterLO;
		
		SD_readSingleBlock(getSector(cluster));
		struct Sdisk_config_structure *config = (struct Sdisk_config_structure *)buffer;
		if(config->checksum == CHECKSUM_CONFIG)
		{
			contrast = config->lcd_contrast;
		}
	}
	lcd_clear();
	lcd_gotoxy(0,0);
	lcd_underline();
	lcd_put_p(CONT);
	lcd_underline();
	icons(4,5,3);
	
	lcd_gotoxy(0,2);
	lcd_put_p(VALUE);
	lcd_put_i(contrast);
	
	unsigned char old_contrast = contrast;
	unsigned char original_contrast = contrast;
	while(1)
	{
		configButtons();
		if(contrast!=old_contrast)
		{
			lcd_gotoxy(0,2);
			lcd_put_p(VALUE);
			lcd_put_i(contrast);
			lcd_put_p(PSTR(" "));
			old_contrast = contrast;
		}
		
		if(SD_ejected()) return; // card is removed
		if(down_is_pressed())
		{
			while(down_is_pressed()){}
			_delay_ms(200);
			
			contrast--;
			if(contrast<MIN_CONTRAST) contrast = MIN_CONTRAST;
			else
			{
				lcd_contrast = contrast;
				lcd_config();
			}
		}
		if(up_is_pressed())
		{
			while(up_is_pressed()){}
			_delay_ms(200);
			
			contrast++;
			if(contrast>MAX_CONTRAST) contrast = MAX_CONTRAST;
			else
			{
				lcd_contrast = contrast;
				lcd_config();
			}
		}
		if(enter_is_pressed())
		{
			while(enter_is_pressed()){}
			_delay_ms(200);
			if(contrast != original_contrast)
			{
				lcd_contrast = contrast;
				lcd_config();
				SD_select_card();
				if(id_of_config_file!=-1)
				{
					cd(0);
					buffer = &writeData[0][0];
					buffClear();
					lastBlockRead = 0;
					struct dir_Structure* config_file = getFile(id_of_config_file);
					unsigned long cluster;
					
					if(FAT_partitionType == PARTITION_TYPE_FAT32) cluster = (unsigned long)config_file->firstClusterHI<<16 | (unsigned long)config_file->firstClusterLO;
					else cluster = (unsigned long)config_file->firstClusterLO;
					
					SD_readSingleBlock(getSector(cluster));
					struct Sdisk_config_structure *config = (struct Sdisk_config_structure *)buffer;
					if(config->checksum == CHECKSUM_CONFIG)
					{
						config->lcd_contrast = contrast;
						SD_writeSingleBlock(getSector(cluster));
					}
				}
				
			}
			return;
		}
	}
	return;
}
void icons(unsigned char i1, unsigned char i2, unsigned char i3)
{
	lcd_gotoxy(0,5);
	lcd_overline();
	lcd_put_p(EMP);
	lcd_gotoxy(0,5);
	lcd_icon(i1);
	lcd_gotoxy(36,5);
	lcd_icon(i2);
	lcd_gotoxy(71,5);
	lcd_icon(i3);
	lcd_overline();
	
}
void setup()
{
	lcd_clear();
	lcd_gotoxy(0,0);
	lcd_underline();
	lcd_put_p(SETUP);
	lcd_underline();
	icons(4,5,3);
	
	unsigned char option = 1;
	lcd_gotoxy(0,2);
	lcd_inverse();
	lcd_put_p(SET1);
	lcd_inverse();
	lcd_gotoxy(0,3);
	lcd_put_p(SET2);
	
	while(1)
	{
		configButtons();
		if(down_is_pressed())
		{
			while(down_is_pressed()){}
			_delay_ms(200);
			
			option++;
			if(option>2) option = 2;
			else
			{
				lcd_gotoxy(0,2);
				lcd_put_p(SET1);
				lcd_inverse();
				lcd_gotoxy(0,3);
				lcd_put_p(SET2);
				lcd_inverse();
			}
		}
		if(up_is_pressed())
		{
			while(up_is_pressed()){}
			_delay_ms(200);
			
			option--;
			if(option<1) option = 1;
			else
			{
				lcd_gotoxy(0,2);
				lcd_inverse();
				lcd_put_p(SET1);
				lcd_inverse();
				lcd_gotoxy(0,3);
				lcd_put_p(SET2);
			}
		}
		if(enter_is_pressed())
		{
			while(enter_is_pressed()){}
			_delay_ms(200);
			if(option == 1) set_speed();
			if(option == 2) set_contrast();
			return;
		}
	}
	
}
#endif
void set_speed()
{
	lcd_clear();
	#ifdef _LCD_NOKIA_
	lcd_gotoxy(0,0);
	lcd_underline();
	lcd_put_p(DLAY);
	lcd_underline();
	icons(4,5,3);
	lcd_gotoxy(0,2);
	lcd_put_p(VALUE);
	#endif
	
	#ifdef _LCD_
	lcd_gotoxy(0,0);
	lcd_put_p(SPE1);
	lcd_gotoxy(0,1);
	lcd_put_p(SPE0);
	#endif
	
	lcd_put_i(SD_speed);
	unsigned char old_speed = SD_speed;
	unsigned char original_speed = SD_speed;
	while(1)
	{
		configButtons();
		if(SD_speed!=old_speed)
		{
			#ifdef _LCD_
			lcd_gotoxy(0,1);
			lcd_put_p(SPE0);
			#endif
			#ifdef _LCD_NOKIA_
			lcd_gotoxy(0,2);
			lcd_put_p(VALUE);
			#endif
			
			lcd_put_i(SD_speed);
			lcd_put_p(PSTR(" "));
			
			old_speed = SD_speed;
		}
		
		if(SD_ejected()) return; // card is removed
		if(down_is_pressed())
		{
			while(down_is_pressed()){}
			_delay_ms(200);
			
			SD_speed--;
			if(SD_speed<SD_MIN_SPEED) SD_speed = SD_MAX_SPEED;
		}
		if(up_is_pressed())
		{
			while(up_is_pressed()){}
			_delay_ms(200);
			
			SD_speed++;
			if(SD_speed>SD_MAX_SPEED) SD_speed = SD_MIN_SPEED;
		}
		if(enter_is_pressed())
		{
			while(enter_is_pressed()){}
			_delay_ms(200);
			if(SD_speed != original_speed)
			{
				if(id_of_config_file!=-1)
				{
					cd(0);
					buffer = &writeData[0][0];
					buffClear();
					lastBlockRead = 0;
					struct dir_Structure* config_file = getFile(id_of_config_file);
					unsigned long cluster;
					
					if(FAT_partitionType == PARTITION_TYPE_FAT32) cluster = (unsigned long)config_file->firstClusterHI<<16 | (unsigned long)config_file->firstClusterLO;
					else cluster = (unsigned long)config_file->firstClusterLO;
					
					SD_readSingleBlock(getSector(cluster));
					struct Sdisk_config_structure *config = (struct Sdisk_config_structure *)buffer;
					if(config->checksum == CHECKSUM_CONFIG)
					{
						config->sd_card_speed = SD_speed;
						SD_writeSingleBlock(getSector(cluster));
					}
				}
				
			}
			return;
		}
	}
	return;
}

void select_nic()
{
	int i = 0, j =0, k =0;
	char name1[8];
	nfiles = 0;
	buffer = &writeData[0][0];
	files_id = &writeData[2][0];
	lastBlockRead = -1;
	buffClear();
	
	#ifdef _LCD_NOKIA_
	lcd_underline();
	#endif
	lcd_gotoxy(0,0);
	lcd_put_p(MSG7);
	lcd_gotoxy(0,1);
	#ifdef _LCD_NOKIA_
	lcd_underline();
	icons(4,5,3);
	for(i= 1;i<=4;i++)
	{
		lcd_gotoxy(0,i);
		lcd_put_p(EMP);
	}
	lcd_gotoxy(0,2);
	#endif
	lcd_put_p(MSGC);
	
	i = 0;

	do
	{
		struct dir_Structure *file = getFile(i);
		if(file) if(file->name[0]== 0x00) break; // last file on directory
		if(is_a_nic(file) || is_a_dir(file))
		{
			files_id[nfiles] = i;
			nfiles++;
		}
		i++;
	} while (nfiles < MAXFILES && i<MAXFILES*2);
	
	if(nfiles == 0)
	{
		#ifdef _LCD_
		lcd_gotoxy(0,1);
		#endif
		#ifdef _LCD_NOKIA_
		lcd_gotoxy(0,2);
		#endif
		lcd_put_p(MSG8);
		return;
	}
	
	#ifdef _LCD_
	lcd_gotoxy(0,1);
	#endif
	#ifdef _LCD_NOKIA_
	lcd_gotoxy(0,2);
	#endif
	lcd_put_p(MSGD);
	
	// sort list of files in the directory
	if(nfiles>1)
	{
		for (i = 0; i <= (nfiles - 2); i++)
		{
			for (j = 1; j <= (nfiles - i - 1); j++)
			{
				struct dir_Structure *file1 = getFile(files_id[j]);
				strlcpy(name1,(const char*)file1->name,8);
				struct dir_Structure *file2 = getFile(files_id[j-1]);
				if (memcmp(name1,file2->name,8)<0)
				{
					k = files_id[j];
					files_id[j] = files_id[j - 1];
					files_id[j - 1]=k;
				}
			}
		}
	}
	
	int index = 0;
	for(i = 0; i<nfiles;i++) if(selected_file_id==files_id[i]) index = i;
	int index_old = -1;
	while(1)
	{
		configButtons();
		if(SD_ejected()) return; // card is removed
		if(down_is_pressed())
		{
			while(down_is_pressed()){}
			_delay_ms(200);
			
			index++;
			if(index==nfiles) index = 0;
		}
		if(up_is_pressed())
		{
			while(up_is_pressed()){}
			_delay_ms(200);
			
			index--;
			if(index<0) index = nfiles-1;
		}
		if(enter_is_pressed())
		{
			while(enter_is_pressed()){}
			struct dir_Structure *file = getFile(files_id[index]);
			if(is_a_dir(file))
			{
				cd(file);
				select_nic();
				return;
			}
			else
			{
				mount_nic_image(files_id[index],file);
				return;
			}
			return;
		}
		if(index!=index_old)
		{
			index_old = index;
			struct dir_Structure *file = getFile(files_id[index]);
			#ifdef _LCD_
			lcd_gotoxy(0,1);
			if(is_a_dir(file)) lcd_put_s("[");
			#endif
			#ifdef _LCD_NOKIA_
			lcd_gotoxy(0,2);
			if(is_a_dir(file)) lcd_icon(6); else lcd_icon(1);
			#endif
			for(int i = 0;i<8;i++)  if(file->name[i]!=' ') lcd_char(file->name[i]);
			#ifdef _LCD_
			if(is_a_dir(file)) lcd_put_s("]");
			#endif
			lcd_put_p(EMP);
		}
	}
	
}
void find_previous_nic()
{
	buffer = &writeData[0][0];
	//buffClear();
	lastBlockRead = -1;
	cd(0);
	int firstNic = -1;
	id_of_config_file = -1;
	int i = 0;
	do
	{
		struct dir_Structure *file = getFile(i);
		if(file)
		{
			if(file->name[0]== 0x00) break; // last file on directory
			
			// save pointer of first NIC found, just in case the config file is not found
			if(is_a_nic(file) && firstNic == -1)
			{
				firstNic = i;
			}
			
			// found the config file with information on card speed and last NIC mounted
			if(strncmp_P((char*)file->name,CFG,11)==0) // found config file
			{
				id_of_config_file = i;
				unsigned long cluster = 0;
				if(FAT_partitionType == PARTITION_TYPE_FAT32) cluster = (unsigned long)file->firstClusterHI<<16 | (unsigned long)file->firstClusterLO;
				else cluster = (unsigned long)file->firstClusterLO;
				SD_readSingleBlock(getSector(cluster));
				struct Sdisk_config_structure *config = (struct Sdisk_config_structure *)buffer;
				if(config->checksum==CHECKSUM_CONFIG)
				{
					SD_speed =config->sd_card_speed;
					#ifdef _LCD_NOKIA_
					lcd_contrast = config->lcd_contrast;
					if(lcd_contrast > MAX_CONTRAST) lcd_contrast = MAX_CONTRAST;
					if(lcd_contrast < MIN_CONTRAST) lcd_contrast = MIN_CONTRAST;
					lcd_config();
					SD_select_card();
					#endif
					
					// set directory
					FAT_sectorOfCurrentDirectory = config->directory_of_last_mounted_nic;
					unsigned int tmp = config->id_of_last_mounted_nic;
					file = getFile(tmp);
					if(file)
					{
						if(is_a_nic(file))
						{
							firstNic = tmp;
							break;
						}
						else cd(0);
					}
					else cd(0);
				}
				// If it gets here the config file does not point to a valid NIC. Get back to the root directory and keep going
				cd(0);
				// breaks if there is a valid NIC
				if(firstNic!=-1) break;
			}
		}
		i++;
		
	} while (i<MAXFILES*2);
	
	if(firstNic==-1) inited = 0;
	else
	{
		struct dir_Structure *file = getFile(firstNic);
		mount_nic_image(firstNic,file);
	}
	return;
}
unsigned char is_a_nic(struct dir_Structure *file)
{
	if(!file) return 0;
	if(file->name[0]== 0x00 || file->name[0]== 0xe5) return 0; //deleted or empty file
	if(file->name[8]=='N' && file->name[9]=='I'  && file->name[10]=='C') return 1;
	return 0;
}
unsigned char is_a_dir(struct dir_Structure *file)
{
	if(!file) return 0;
	if(file->name[0]== 0x00 || file->name[0]== 0xe5) return 0; //deleted or empty file
	if(file->attrib & 0x10) return 1;
	return 0;
}
void create_config_file()
{
	id_of_config_file = -1;
	// first allocate empty cluster for data. If returns 0 disk if full. In this case, exit
	// just one cluster is enough for the config file
	unsigned long cluster = allocEmptyCluster();

	if(cluster == 0) return;
	
	// go to the root directory. It is up to the caller to save the current directory address
	cd(0);
	int i = 0;
	do
	{
		struct dir_Structure *file = anyFile(i);
		if(file)
		{
			// get first deleted of first empty
			if(((file->name[0] == 0xe5) || (file->name[0] == 0x00)) && (file->attrib != 0xf)  )
			{
				id_of_config_file = i;
				strcpy_P((char*)file->name,CFG);  //copy name to the config file
				file->attrib = 0x01;
				file->firstClusterHI = (unsigned int)((cluster >> 16) & 0xFFFF);
				file->firstClusterLO = (unsigned int)(cluster & 0xFFFF);
				//save modified block back to SD card and exit
				SD_writeSingleBlock(lastBlockRead);
				break;
			}
		}
		i++;
		
	} while (i<MAXFILES*2);
	return;
}
void swap_nic()
{
	unsigned long current_dir = FAT_sectorOfCurrentDirectory;
	
	if(id_of_config_file!=-1)
	{
		cd(0);
		lastBlockRead = -1;
		struct dir_Structure* config_file = getFile(id_of_config_file);
		
		unsigned long cluster = 0;
		
		if(FAT_partitionType == PARTITION_TYPE_FAT32) cluster = (unsigned long)config_file->firstClusterHI<<16 | (unsigned long)config_file->firstClusterLO;
		else cluster = (unsigned long)config_file->firstClusterLO;
		
		SD_readSingleBlock(getSector(cluster));
		struct Sdisk_config_structure *config = (struct Sdisk_config_structure *)buffer;

		lastBlockRead = -1;
		FAT_sectorOfCurrentDirectory  = config->directory_of_previous_mounted_nic;
		int file_id = config->id_of_previous_mounted_nic;
		struct dir_Structure *file = getFile(file_id);
		
		if(is_a_nic(file))
		{
			mount_nic_image(file_id,file);
			return;
		}
	}

	// restore point to the current directory
	FAT_sectorOfCurrentDirectory = current_dir;
}
unsigned int mount_nic_image(int file_id, struct dir_Structure* file)
{
	if(!file) return 0;
	
	#ifdef _LCD_
	lcd_gotoxy(0,0);
	lcd_put_p(NIC);
	for(int i = 0;i<8;i++)  if(file->name[i]!=' ') lcd_char(file->name[i]);
	lcd_put_p(EMP);
	lcd_gotoxy(0,1);
	lcd_put_p(EMP);
	lcd_gotoxy(0,1);
	lcd_put_p(SPE0);
	lcd_put_i(SD_speed);
	#endif
	#ifdef _LCD_NOKIA_
	lcd_clear();
	lcd_gotoxy(0,0);
	lcd_underline();
	lcd_put_p(NIC);
	lcd_gotoxy(53,0);
	if(FAT_partitionType==PARTITION_TYPE_FAT32) lcd_put_p(FAT3);
	else lcd_put_p(FAT1);
	lcd_underline();
	icons(0,1,2);
	lcd_gotoxy(0,2);
	lcd_icon(1);
	for(int i = 0;i<8;i++)  if(file->name[i]!=' ') lcd_char(file->name[i]);
	#endif
	
	selected_file_id = file_id;

	unsigned long cluster = 0;
	if(FAT_partitionType == PARTITION_TYPE_FAT32) cluster = (unsigned long)file->firstClusterHI<<16 | (unsigned long)file->firstClusterLO;
	else cluster = (unsigned long)file->firstClusterLO;
	
	unsigned char n_fat_elements = 0;
	if(cluster==0) return 0;
	
	for(int i = 0; i<FAT_NIC_ELEMS;i++) fatNic[i] = 0;
	
	fatNic[n_fat_elements] = cluster;
	n_fat_elements++;
	
	while(n_fat_elements<FAT_NIC_ELEMS && cluster < 0xfff6)
	{
		cluster = getNextCluster(cluster);
		if(cluster<0xfff6)
		{
			fatNic[n_fat_elements]=cluster;
			n_fat_elements++;
		}
	}
	
	//remover quando for criar arquivo de configuracao
	
	// now that all the information is stored, I need to update the configuration file
	// first save the current directory point
	unsigned long current_dir = FAT_sectorOfCurrentDirectory;
	
	// if there is no config file, create one
	
	//if(id_of_config_file==-1) create_config_file();
	
	// save only if there is a config file
	if(id_of_config_file!=-1)
	{
		cd(0);
		lastBlockRead = -1;
		struct dir_Structure* config_file = getFile(id_of_config_file);
		
		if(FAT_partitionType == PARTITION_TYPE_FAT32) cluster = (unsigned long)config_file->firstClusterHI<<16 | (unsigned long)config_file->firstClusterLO;
		else cluster = (unsigned long)config_file->firstClusterLO;
		
		SD_readSingleBlock(getSector(cluster));
		struct Sdisk_config_structure *config = (struct Sdisk_config_structure *)buffer;
		config->checksum = CHECKSUM_CONFIG;
		
		if(config->directory_of_last_mounted_nic != current_dir || config->id_of_last_mounted_nic != file_id)
		{
			config->directory_of_previous_mounted_nic = config->directory_of_last_mounted_nic;
			config->id_of_previous_mounted_nic = config->id_of_last_mounted_nic;
		}
		
		config->directory_of_last_mounted_nic = current_dir;
		config->id_of_last_mounted_nic = file_id;
		config->sd_card_speed = SD_speed;
		config->lcd_contrast = lcd_contrast;
		
		SD_writeSingleBlock(getSector(cluster));
	}

	// restore point to the current directory
	FAT_sectorOfCurrentDirectory = current_dir;

	bitbyte = 0;
	readPulse = 0;
	magState = 0;
	prepare = 1;
	ph_track = 0;
	sector = 0;
	buffNum = 0;
	formatting = 0;
	writePtr = &(writeData[buffNum][0]);
	SD_select_card();
	SD_sendCommand(SET_BLOCK_LEN, 512);
	buffClear();
	inited = 1;
	

	return 1;
}

void writeBackSub()
{
	unsigned char i, j;

	if (SD_ejected()) return;
	for (j = 0; j < BUF_NUM; j++)
	{
		if (sectors[j] != 0xff)
		{
			for (i = 0; i < BUF_NUM; i++)
			{
				if (sectors[i] != 0xff) writeBackSub2(i, sectors[i], tracks[i]);
				sectors[i] = 0xff;
				tracks[i] = 0xff;
				writeData[i][2]=0;
			}
			buffNum = 0;
			writePtr = &(writeData[buffNum][0]);
			break;
		}
	}
}
void writeBackSub2(unsigned char bn, unsigned char sc, unsigned char track)
{
	unsigned char c,d;
	unsigned short i;
	
	unsigned short long_sector = (unsigned short)track*16+sc;
	unsigned short long_cluster = long_sector>>FAT_sectorsPerClusterBitShift;
	unsigned long  ft = fatNic[long_cluster];
	
	SPI_PORT = NCLKNDI_CS;
	SPI_PORT = NCLKNDINCS;
	
	// Timing is an issue here. This is the reason I copied explicitly the conversion from cluster to block
	// instead of using getCluster() method. I also did bit shift instead of multiplications to make it faster.
	// even inline function is not fast enough.
	SD_cmdFast(WRITE_SINGLE_BLOCK, (((ft-2)<<FAT_sectorsPerClusterBitShift)+FAT_firstDataSector) + (long_sector&(FAT_sectorsPerCluster-1)));

	SPI_send_byte_fast(0xff);
	SPI_send_byte_fast(0xfe);
	// 22 ffs
	for (i = 0; i < 22 * 8; i++)
	{
		SPI_PORT = NCLK_DINCS;
		SPI_PORT = _CLK_DINCS;
	}
	SPI_PORT = NCLKNDINCS;

	// sync header
	SPI_send_byte_fast(0x03);
	SPI_send_byte_fast(0xfc);
	SPI_send_byte_fast(0xff);
	SPI_send_byte_fast(0x3f);
	SPI_send_byte_fast(0xcf);
	SPI_send_byte_fast(0xf3);
	SPI_send_byte_fast(0xfc);
	SPI_send_byte_fast(0xff);
	SPI_send_byte_fast(0x3f);
	SPI_send_byte_fast(0xcf);
	SPI_send_byte_fast(0xf3);
	SPI_send_byte_fast(0xfc);

	// address header
	SPI_send_byte_fast(0xd5);
	SPI_send_byte_fast(0xAA);
	SPI_send_byte_fast(0x96);
	SPI_send_byte_fast((volume >> 1) | 0xaa);
	SPI_send_byte_fast(volume | 0xaa);
	SPI_send_byte_fast((track >> 1) | 0xaa);
	SPI_send_byte_fast(track | 0xaa);
	SPI_send_byte_fast((sc >> 1) | 0xaa);
	SPI_send_byte_fast(sc | 0xaa);
	c = (volume ^ track ^ sc);
	SPI_send_byte_fast((c >> 1) | 0xaa);
	SPI_send_byte_fast(c | 0xaa);
	SPI_send_byte_fast(0xde);
	SPI_send_byte_fast(0xAA);
	SPI_send_byte_fast(0xeb);

	// sync header
	SPI_send_byte_fast(0xff);
	SPI_send_byte_fast(0xff);
	SPI_send_byte_fast(0xff);
	SPI_send_byte_fast(0xff);
	SPI_send_byte_fast(0xff);

	// data
	for (i = 0; i < 349; i++)
	{
		c = writeData[bn][i];
		for (d = 0b10000000; d; d >>= 1)
		{
			if (c & d) {
				SPI_PORT = NCLK_DINCS;
				SPI_PORT = _CLK_DINCS;
				} else {
				SPI_PORT = NCLKNDINCS;
				SPI_PORT = _CLKNDINCS;
			}
		}
	}
	SPI_PORT = NCLKNDINCS;
	for (i = 0; i < 14 * 8; i++)
	{
		SPI_PORT = NCLK_DINCS;
		SPI_PORT = _CLK_DINCS;
	}
	SPI_PORT = NCLKNDINCS;
	for (i = 0; i < 96 * 8; i++)
	{
		SPI_PORT = NCLKNDINCS;
		SPI_PORT = _CLKNDINCS;
	}
	SPI_PORT = NCLKNDINCS;
	SPI_send_byte_fast(0xff);
	SPI_send_byte_fast(0xff);
	SPI_read_byte_fast();
	SD_wait_finish();
	
	SPI_PORT = NCLKNDI_CS;
	SPI_PORT = NCLKNDINCS;
	
}
void writeBack()
{
	static unsigned char sec;
	
	if (SD_ejected()) return;
	if (writeData[buffNum][2] == 0xAD) {
		if (!formatting)
		{
			sectors[buffNum] = sector;
			tracks[buffNum] = (ph_track >> 2);
			sector = ((((sector == 0xf) || (sector == 0xd)) ? (sector + 2) : (sector + 1)) & 0xf);
			if (buffNum == (BUF_NUM - 1))
			{
				// cancel reading
				cancelRead();
				writeBackSub();
				prepare = 1;
			}
			else
			{
				buffNum++;
				writePtr = &(writeData[buffNum][0]);
			}
		}
		else
		{
			sector = sec;
			formatting = 0;
			if (sec == 0xf)
			{
				// cancel reading
				cancelRead();
				prepare = 1;
			}
		}
	}
	if (writeData[buffNum][2] == 0x96)
	{
		sec = (((writeData[buffNum][7] & 0x55) << 1) | (writeData[buffNum][8] & 0x55));
		formatting = 1;
	}
}
void cancelRead()
{
	unsigned short i;
	if (bitbyte < (402 * 8))
	{
		SPI_PORT = NCLK_DINCS;
		for (i = bitbyte; i < (514 * 8); i++) // 512 bytes + 2 CRC
		{
			if (SD_ejected()) return;
			SPI_PORT = _CLK_DINCS;
			SPI_PORT = NCLK_DINCS;
		}
		bitbyte = 402 * 8;
	}
}
void buffClear()
{
	unsigned char i;
	unsigned short j;
	for (i=0; i<BUF_NUM; i++) for (j=0; j<350; j++) writeData[i][j]=0;
	for (i=0; i<BUF_NUM; i++) sectors[i]=tracks[i]=0xff;
}

void  configButtons()
{
	ENTER_PORTD |= 1<<(ENTER_BIT);
	UP_PORTD |= 1<<(UP_BIT);
	DOWN_PORTD |= 1<<(DOWN_BIT);
}