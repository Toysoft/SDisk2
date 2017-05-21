SDISKII 
=======

This package contains a new version of the firmware for the SDISKII with the LCD and 
buttons to select the NIC image.

Images are saved in the card in NIC format. You can convert DSK to NIC and NIC to DSK
using the utility provided by Nishida in his webpage 

     http://tulip-house.ddo.jp/digital/SDISK2/english.html
     
You can organize your images in subdirectories and you can nest them in as many levels
as you want. In order to save your configuration and keep track of your last mounted
NIC image and directory you need to have the SDISKII.CFG file saved in the top directory
of your SD card. Please use the SDISKII.CFG file provided in this repository.

FIRMWARE
========

The are 3 compiled versions of this firmware:

1. sdisk2_original.hex
  
  This is the original SDISK II with the 16 x 2 LCD done by Victor Trucco and Fabio
  Belavenuto. See sdisk2_original.jpg for a picture of it as sold by Victor Trucco.
  
  Please note the three buttons in the botton of the sdisk2. They are used for selecting
  the disk image and navigation as well as setup of the sdisk. 
  
  The schematics for this version can be found in the file sdisk2_original.pdf

2. sdisk2_original_mod_nokia.hex

  Sometime ago I decided to replace the 16x2 LCD with a graphical Nokia 5110 LCD. I used
  the same board sold by Victor, took out the original LCD and replaced it by the Nokia
  LCD. I used some prototyping board to adapt the Nokia LCD to the original Sdisk. It is
  not pretty but it works very well. See sdisk2_original_mod_nokia.jpg for a picture of it.
  
  This is the original SDISK II hardware done by Victor Trucco and Fabio Belavenuto
  with the mod done by Alexandre Suaide in order to replace the 16 x 2 LCD with
  the Nokia 5110 graphical display
  
  the schematics for this version is the same as for original just replacing the 16x2 LCD
  in the original version by the Nokia 5110 display. The connections for this display
  can be found in the file sdisk2_original_mod_nokia.pdf

3. sdisk2_oled.hex

  This is for the second version of the SDISK II done by Victor Trucco already with the
  128 x 64 OLED graphical display and USB connection to update the firmware.
  
  Photo and schematics for this layout will be posted as soon as the first batch is made and
  the firmware is well tested. It will come soon.

Formatting the SD card
======================

You can use this version of the firmware with standard SD cards as well as with SDHC 
cards. You can format them in FAT16 or FAT32 only. The minimum cluster size is 32kB 
(or 64 blocks per cluster). In order to formar a SD card, use the command:

Windows:

  format [letter:] /FS:FAT /A:32K

  you can replace FAT by FAT32

MAC OS/X

  diskutil partitionDisk /dev/[disk name] MBR "MS-DOS FAT16" SDISKII 0b
  newfs_msdos -F 16 -c 64 /dev/[disk name]

  you can replace 16 (FAT16) by 32 (FAT32). The same for the partition scheme. 
  64 is the number of blocks per cluster and can be larger.

Linux

  mkfs.msdos -F 16 -s 64 /dev/[disk name]

  you can replace 16 (FAT16) by 32 (FAT32). 64 is the number of blocks per cluster 
  and can be larger.


Selecting NIC image
===================

When there is no disk activity (LED off), press the middle button (ENTER). A list 
of NIC files in the current directory as well as the subdirectories, will be created. 
Navigate through the list using the LEFT and RIGHT buttons. Use the ENTER button to 
select a NIC file or a directory. Directories are shown around [ ]. 

Only NIC files are supported.

Selecting the delay of the SD card
==================================

Standard SD and SDHC cards can differ by a large value in terms of speed. In order 
to compensate it a little bit the user can set the delay of the card from 1 (small delays) 
to 10 (long delays). The default value is 5 and it looks fine for the cards I testes. 
If you get a lot of problems reading NIC file, try to change it a little. I noticed 
that ProDOS images are more sensitive to this setting than DOS 3.3 images.

When there is no disk activity (LED off), press the LEFT button. The SDISKII will 
prompt you to select the speed of the card. Use the LEFT and RIGHT buttons to 
navigate the available speeds and the ENTER (middle) button to select it.

Swap between two disk images
============================

If you have the SDISKII.CFG file saved in your SD card (see bellow) we keep track of the
previous mounted NIC image. In this case, if you press the RIGHT button we swap the current
mounted NIC with the previous one. This may be useful if you are copying from one image to
another or if you are playing a game that spans over two images.

Saving the default values and last NIC mounted
==============================================

In order to save your configurations (SD card speed and last NIC mounted) you need 
to copy the file SDISKII.CFG (provided here) in the root directory of your SD card. 
If this file is not present every time you turn on the SDISKII the first found NIC 
will be mounted and the speed of the SD card will be set to 5.

Upgrading your SDISKII with this new firmware
=============================================

In order to upgrade your SDISKII with this new firmware you will need a chip programmer, such 
as the MiniPro TL866CS. Remove the ATMEGA328P from your SDISKII, plug it in the programmer.
Open the programmer software, load the firmware (sdisk2.hex for the old model with 16x2 LCD
or sdisk2_nokia.hex for the Nokia5110 LCD). 

Check if the fuses are right. The values for the fuses are:

   LFUSE = 0xDE
   HFUSE = 0xD9
   EFUSE = 0x07

then program your ATMEGA328P with this new firmware. Put it back to the SDISKII and you are set.

