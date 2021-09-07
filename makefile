DEVICE     = atmega328p
FUSES      = -U lfuse:w:0xce:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m
FILENAME   = main
COMPILE    = avr-g++ -Wall -Wl,-gc-sections -Wl,-relax -g -mcall-prologues -O3 -mmcu=$(DEVICE) -Wl,-Map=$(FILENAME).map

default: compile upload clean

compile:
	$(COMPILE) -c $(FILENAME).c -o $(FILENAME).o 
	$(COMPILE) -o $(FILENAME).elf $(FILENAME).o
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex 
	avr-size --format=avr --mcu=$(DEVICE) $(FILENAME).elf

upload:
	avrdude -v -F -B 0.1 -p $(DEVICE) -c usbasp $(FUSES) -U flash:w:$(FILENAME).hex:i 
	
clean:
	rm -f $(FILENAME).o
	rm -f $(FILENAME).elf
	rm -f $(FILENAME).hex
	rm -f $(FILENAME).lss
	rm -f $(FILENAME).map
