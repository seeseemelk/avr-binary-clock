CC := avr-gcc
CFLAGS := -mmcu=atmega32 -DF_CPU=1000000UL -O3 -fomit-frame-pointer -Wall -Wextra -Werror

BUILD_DIR = ./build
BIN = clock.elf
EEP = $(BIN:.elf=.eep)

SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:.o=.d)

.PHONY: clean create_build_dir upload check

all: create_build_dir $(BIN)

upload: $(BIN) $(EEP)
	avrdude -c jtag1 -p m32 -P /dev/ttyUSB0 -U clock.elf -U eeprom:w:$(EEP):r

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^

$(EEP): $(BIN)
	avr-objcopy -j .eeprom -O binary $< $@

-include $(DEP)

$(BUILD_DIR)/%.o: %.c makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

check: $(SRC)
	cppcheck --enable=all --std=c11 --inconclusive --error-exitcode=1 $^

debug: $(BIN)
	avarice --jtag /dev/ttyUSB0 :4242
