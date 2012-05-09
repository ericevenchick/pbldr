# pbldr #
A bootloader for PIC18F devices.

### Tested devices: ###
* PIC18F26K80

### How To ###
* Build bootloader in firmware/src
* Build application using linker script (demos/pic18f26k80-pbldr.lkr)
  * Put main() (reset vector) at 0x920 (#pragma code main=0x920)
  * Put interrupt vectors at 0x908 and 0x918
* Connect to UART1 and flash using software/pbldr-flash.py
  * Run pbldr-flash then restart device to start flash


