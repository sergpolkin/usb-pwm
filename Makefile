PREFIX ?= avr

CC  := $(PREFIX)-gcc
CXX := $(PREFIX)-g++
LD  := $(PREFIX)-gcc
AR  := $(PREFIX)-ar
AS  := $(PREFIX)-as
OBJCOPY := $(PREFIX)-objcopy
OBJDUMP := $(PREFIX)-objdump
SIZE    := $(PREFIX)-size
AVRDUDE = avrdude

INCDIR = .

# target mcu (at90s8515, atmega16, atmega8515...)
MCU = attiny2313

AVRDMCU = t2313
AVRDUDE_FLAGS = -p $(AVRDMCU)
AVRDUDE_FLAGS+= -c usbasp

# Some more settings
# Clock Frequency of the AVR. Needed for various calculations.
CPUFREQ = 16000000UL

# Additional defines
# Known defines are:
# -DKISS    enables Keep-It-Simple-Stupid mode, which can only handle one
#           temperature sensor on the bus.
ADDDEFS =
SRCS = usbdrv/usbdrv.c usbdrv/oddebug.c main.c
ASMS = usbdrv/usbdrvasm.S
PROG = usb-pwm

# compiler flags
#CFLAGS	= -g -Os -Wall -fno-strict-aliasing -mmcu=$(MCU) $(ADDDEFS)
CFLAGS  = -g -Os -Wall -fdata-sections -ffunction-sections -mmcu=$(MCU)
CFLAGS += -I$(INCDIR)

CPPFLAGS += -MD
CPPFLAGS += -DF_CPU=$(CPUFREQ)

# linker flags
#LDFLAGS = -g -mmcu=$(MCU) -Wl,-Map,$(PROG).map
LDFLAGS = -g -mmcu=$(MCU) -Wl,--gc-sections,-Map,$(PROG).map

OBJS = $(SRCS:.c=.o) $(ASMS:.S=.o)

all: compile dump text eeprom
	$(SIZE) $(PROG).elf

compile: $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROG).elf $(OBJS)

dump: compile
	$(OBJDUMP) -h -S $(PROG).elf > $(PROG).lst

%.o : %c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cpp
%.o : %.S
	$(CC) -x assembler-with-cpp $(CFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).S

# Create the flash contents
text: compile
	@$(OBJCOPY) -j .text -j .data -O ihex $(PROG).elf $(PROG).hex
	@#$(OBJCOPY) -j .text -j .data -O srec $(PROG).elf $(PROG).srec
	@$(OBJCOPY) -j .text -j .data -O binary $(PROG).elf $(PROG).bin

# Rules for building the .eeprom rom images
eeprom: compile
	@$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $(PROG).elf $(PROG)_eeprom.hex
	@#$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $(PROG).elf $(PROG)_eeprom.srec
	@$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $(PROG).elf $(PROG)_eeprom.bin

clean:
	rm -f *~ *.elf *.rom *.bin *.eep *.o *.lst *.map *.srec *.hex
	rm -f $(PROG) $(OBJS) ${OBJS:%.o:%.d}
	rm -f $(patsubst %.o,%.d,$(OBJS))
flash: text
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(PROG).hex:i
flash_eeprom: eeprom
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:w:$(PROG)_eeprom.hex:i

.PHONY: clean compile dump text eeprom flash flash_eeprom

-include $(OBJS:.o=.d)
