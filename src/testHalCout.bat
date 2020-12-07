avr-gcc -Os -Wall -DF_CPU=16000000UL -mmcu=atmega328p tout.c hal.c hal_Out.c bsl_COut.c bsl_xtoa.c -o tout.o
avr-objcopy -O ihex -j .text -j .data tout.o  tout.hex
avrdude -c arduino -p atmega328p -b 57600 -P %1 -D -Uflash:w:tout.hex:i
pause
