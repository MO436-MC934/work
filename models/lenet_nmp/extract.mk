.PHONY : clean build all

MODEL ?= mnist

# Toolchain
CROSS_COMPILE ?= riscv32-unknown-elf-
CC := $(CROSS_COMPILE)objcopy

# FLAGS
TFLAGS = -O binary -j .text -j .rodata
BFLAGS = -O binary -j .rodata -j .eh_frame -j .tdata -j .data -j .sdata

all: clean build

build: $(MODEL).text.bin $(MODEL).data.bin

$(MODEL).text.bin : $(MODEL).x
	@ echo 'Extracting text of $< using GCC objcopy'
	$(CC) $(TFLAGS) $< $@

$(MODEL).data.bin : $(MODEL).x
	@ echo 'Extracting data of $< using GCC objcopy'
	$(CC) $(BFLAGS) $< $@

clean:
	rm -f $(MODEL).text.bin $(MODEL).data.bin
