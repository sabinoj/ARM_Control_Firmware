flags = -O3
micro = atmega8

makeCompile: main.c
	avr-gcc -g $(flags) -mmcu=$(micro) -c main.c
	avr-gcc -g $(flags) -mmcu=$(micro) -c LED_Button.c
	avr-gcc -g $(flags) -mmcu=$(micro) -c LCD.c
	avr-gcc -g $(flags) -mmcu=$(micro) -c adc.c
	avr-gcc -g $(flags) -mmcu=$(micro) -o main.elf ./main.o ./LED_Button.o ./LCD.o ./adc.o
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	sudo avrdude -c USBasp -p $(micro) -U flash:w:main.hex:i 

fuse:
	sudo avrdude -c USBasp -p $(micro) -U lfuse:w:0xc4:m -U hfuse:w:0xd9:m 
