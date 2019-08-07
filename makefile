CC := avr-gcc
CFLAGS := -mmcu=atmega32 -DF_CPU=1000000UL -O3 -fomit-frame-pointer -Wall -Wextra -Werror

BUILD_DIR = ./build
BIN = clock.elf

SRC = $(wildcard src/*.c)
OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:.o=.d)

.PHONY: clean create_build_dir upload check

all: create_build_dir $(BIN)

upload: $(BIN)
	avrdude -c jtag1 -p m32 -P /dev/ttyUSB0 -U clock.elf 

clean:
	rm -rf $(BUILD_DIR)

$(BIN): $(OBJ)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^

-include $(DEP)

$(BUILD_DIR)/%.o: %.c makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

check: $(SRC)
	cppcheck --enable=all --std=c11 --inconclusive --error-exitcode=1 $^
