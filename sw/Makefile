MCU	= atmega8
F_CPU = 16000000

AVRDUDE_MCU	= m8
AVRDUDE_PRG	= avrisp2
AVRDUDE_PRG_PORT = usb
AVRDUDE_BITCLOCK = 10

CC = avr-gcc
CFLAGS = -W -Wall -Wextra -Os -ffreestanding -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -ffunction-sections -flto -fwhole-program -fno-tree-scev-cprop -g -DEBUG_HAL
LDFLAGS = -lm -Wl,-gc-sections -Wl,--relax 

SRC = $(shell find -name '*.c')
HDS = $(shell find -name '*.h')

default: main.hex main.eep

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@

%.eep: %.elf
	avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex $< $@

main.elf: $(SRC) $(HDS)
	$(CC) $(CFLAGS) $(LDFLAGS) -mmcu=$(MCU) -DF_CPU=$(F_CPU) $(SRC) -o main.elf
	avr-size --format=avr --mcu=$(MCU) main.elf

chip_erase:
	avrdude -p $(AVRDUDE_MCU) -c $(AVRDUDE_PRG) -P $(AVRDUDE_PRG_PORT) -B $(AVRDUDE_BITCLOCK) -e

chip_write_flash: main.hex
	avrdude -p $(AVRDUDE_MCU) -c $(AVRDUDE_PRG) -P $(AVRDUDE_PRG_PORT) -B $(AVRDUDE_BITCLOCK) -U flash:w:main.hex:a

chip_write_eeprom: main.eep
	avrdude -p $(AVRDUDE_MCU) -c $(AVRDUDE_PRG) -P $(AVRDUDE_PRG_PORT) -B $(AVRDUDE_BITCLOCK) -U eeprom:w:main.eep

chip_write: chip_write_flash chip_write_eeprom
	
open_terminal:
	avrdude -p $(AVRDUDE_MCU) -c $(AVRDUDE_PRG) -P $(AVRDUDE_PRG_PORT) -t

print_size: main.elf
	avr-nm -S --size-sort -r main.elf
	avr-size --format=avr --mcu=$(MCU) main.elf

disassembly: main.elf
	avr-objdump -d -S main.elf > disassembly.asm
