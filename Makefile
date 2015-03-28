flags = -O3
micro = atmega164p

makeCompile: main.c
	avr-gcc -g $(flags) -mmcu=$(micro) -std=c99 -c main.c
	avr-gcc -g $(flags) -mmcu=$(micro) -std=c99 -c adc.c
	avr-gcc -g $(flags) -mmcu=$(micro) -std=c99 -c serial.c
	avr-gcc -g $(flags) -mmcu=$(micro) -std=c99 -c int.c
	avr-gcc -g $(flags) -mmcu=$(micro) -std=c99 -c timer.c
	avr-gcc -g $(flags) -mmcu=$(micro) -std=c99 -c roboteq.c
	avr-gcc -g $(flags) -mmcu=$(micro) -std=c99 -o main.elf ./main.o ./adc.o ./serial.o ./int.c ./timer.c ./roboteq.c
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avrdude -c USBasp -p $(micro) -U flash:w:main.hex:i 

flashCrystal:
	# External crystal oscillator 8mhz or greater with max startup time
	# Disables JTAG so PortC can be used as GPIO pins
	avrdude -c USBasp -p $(micro) -U lfuse:w:0xde:m -U hfuse:w:0xd9:m	

flash8mhzCKOPT:
	# Enables full voltage swing on XTAL, don't use this
	# avrdude -c USBasp -p $(micro) -U lfuse:w:0xde:m -U hfuse:w:0xc9:m	

readfuse:
	avrdude -c USBASP -p $(micro) -U hfuse:r:high:h -U lfuse:r:low:h
	cat low
	cat high
	rm low
	rm high
