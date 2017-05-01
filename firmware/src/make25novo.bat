




avr-gcc -o "lcd.o" "lcd.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_ -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "SPI_routines.o" "SPI_routines.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "SD_routines.o" "SD_routines.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "FAT32.o" "FAT32.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -x assembler-with-cpp -mmcu=atmega328p -MMD -MP -D_SDISK_ORIGINAL_ -c -o "sub.o" "sub.S"
avr-gcc -o "main.o" "main.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__  -D_SDISK_ORIGINAL_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -Wl,-Map,sdisk2_original.map -mmcu=atmega328p -D_SDISK_ORIGINAL_ -o "sdisk2_original.elf" "main.o" "lcd.o" "SPI_routines.o" "SD_routines.o" "FAT32.o" "sub.o"  
avr-objcopy -R .eeprom -O ihex "sdisk2_original.elf" "sdisk2_original.hex"
avr-size --format=avr --mcu=atmega328p "sdisk2_original.elf"



avr-gcc -o "lcd_nokia.o" "lcd_nokia.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_MOD_NOKIA_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "SPI_routines.o" "SPI_routines.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_MOD_NOKIA_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "SD_routines.o" "SD_routines.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_MOD_NOKIA_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "FAT32.o" "FAT32.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_MOD_NOKIA_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -x assembler-with-cpp -mmcu=atmega328p -MMD -MP -D_SDISK_ORIGINAL_MOD_NOKIA_ -c -o "sub.o" "sub.S"
avr-gcc -o "main.o" "main.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_ORIGINAL_MOD_NOKIA_ -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -Wl,-Map,sdisk2_original_mod_nokia.map -mmcu=atmega328p -D_SDISK_ORIGINAL_MOD_NOKIA_ -o "sdisk2_original_mod_nokia.elf" "main.o" "lcd_nokia.o" "SPI_routines.o" "SD_routines.o" "FAT32.o" "sub.o"  
avr-objcopy -R .eeprom -O ihex "sdisk2_original_mod_nokia.elf" "sdisk2_original_mod_nokia.hex"
avr-size --format=avr --mcu=atmega328p "sdisk2_original_mod_nokia.elf"



avr-gcc -o "oled.o" "oled.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_OLED_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "SPI_routines.o" "SPI_routines.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_OLED_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "SD_routines.o" "SD_routines.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_OLED_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -o "FAT32.o" "FAT32.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_OLED_  -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -x assembler-with-cpp -mmcu=atmega328p -MMD -MP -D_SDISK_OLED_ -c -o "sub.o" "sub.S"
avr-gcc -o "main.o" "main.c" -Wall -Os -Wno-deprecated-declarations -Wno-strict-aliasing -D__PROG_TYPES_COMPAT__ -D_SDISK_OLED_ -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=25000000UL -MMD -MP -c 
avr-gcc -Wl,-Map,sdisk2_oled.map -mmcu=atmega328p -D_SDISK_OLED_ -o "sdisk2_oled.elf" "main.o" "oled.o" "SPI_routines.o" "SD_routines.o" "FAT32.o" "sub.o"  
avr-objcopy -R .eeprom -O ihex "sdisk2_oled.elf" "sdisk2_oled.hex"
avr-size --format=avr --mcu=atmega328p "sdisk2_oled.elf"


del *.o
del *.map
del *.d
del *.elf