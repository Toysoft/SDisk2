SDISKII firmware
================

This package contains a new version of the firmware for the SDISKII with the LCD and buttons to select the NIC image.

Formatting the SD card
======================

You can use this version of the firmware with standard SD cards as well as with SDHC cards. You can format them in FAT16 or FAT32 only. The minimum cluster size is 32kB (or 64 blocks per cluster). In order to formar a SD card, use the command:

Windows:

  format [letter:] /FS:FAT /A:32K

  you can replace FAT by FAT32

MAC OS/X

  newfs_msdos -F 16 -c 64 /dev/[disk name]

  you can replace 16 (FAT16) by 32 (FAT32). 64 is the number of blocks per cluster and can
  be larger.

Linux

  mkfs.msdos -F 16 -s 64 /dev/[disk name]

  you can replace 16 (FAT16) by 32 (FAT32). 64 is the number of blocks per cluster and can
  be larger.


Selecting NIC image
===================

When there is no disk activity (LED off), press the middle button (ENTER). A list of NIC files in the current directory as well as the subdirectories, will be created. Navigate through the list using the LEFT and RIGHT buttons. Use the ENTER button to select a NIC file or a directory. Directories are shown around [ ]. 

Only NIC files are supported.

Selecting the speed of the SD card
==================================

Standard SD and SDHC cards can differ by a large value in terms of speed. In order to compensate it a little bit the user can set the speed of the card from 1 (fastest) to 10 (slowest). The default value is 5 and it looks fine for the cards I testes. If you get a lot of problems reading NIC file, try to change it a little. I noticed that ProDOS images are more sensitive to this setting than DOS 3.3 images.

When there is no disk activity (LED off), press the LEFT button. The SDISKII will prompt you to select the speed of the card. Use the LEFT and RIGHT buttons to navigate the available speeds and the ENTER (middle) button to select it.

Saving the default values and last NIC mounted
==============================================

In order to save your configurations (SD card speed and last NIC mounted) you need to copy the file SDISKII.CFG (provided here) in the root directory of your SD card. If this file is not present every time you turn on the SDISKII the first found NIC will be mounted and the speed of the SD card will be set to 5.

