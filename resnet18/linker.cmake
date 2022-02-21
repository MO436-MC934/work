.PHONY : clean build all

MODEL ?= resnet18
BUNDLE ?= bundle
STACK ?= 2048

# Toolchain
CROSS_COMPILE ?= riscv32-unknown-elf-
CC := $(CROSS_COMPILE)gcc

# FLAGS
FLAGS = -march=rv32imxnmp -mabi=ilp32 -mcmodel=medany -nostdlib -lgcc -lm -O2 \
        -flto -fuse-linker-plugin -fno-builtin -fno-common -Wall \
	-T ${NMPRTL_PATH}/generic.ld -Wl,--defsym=__intmem_size=$(STACK) -Wl,-Map=$(MODEL).map \
	-L ${NMPRTL_PATH} -llne ${NMPRTL_PATH}/crt0.o ${NMPRTL_PATH}/stdlib.o

all: clean build

build: $(MODEL).x

$(MODEL).x : ${BUNDLE}/$(MODEL).o
	@ echo 'Building binary $@ using GCC linker'
	$(CC) $< $(FLAGS) -o $@

clean:
	rm -f $(MODEL).map $(MODEL).x
