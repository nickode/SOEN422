avr-gcc -Os -Wall -DF_CPU=16000000UL -mmcu=atmega328p -DDebugXtoa bsl_COut.c bsl_xtoa.c -o bsl_COut.o
avr-objcopy -O ihex -j .text -j .data bsl_COut.o  bsl_COut.hex
avrdude -c arduino -p atmega328p -b 57600 -P %1 -D -Uflash:w:bsl_COut.hex:i
pause
