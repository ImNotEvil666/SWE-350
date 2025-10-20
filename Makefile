CC=gcc
CFLAGS=-O2 -Wall -Wextra -std=c11
INCLUDES=-Iincludes -Ilib
SRC=src/main.c \
    src/hal/hal-api.c \
    src/peripherals/led.c \
    src/peripherals/switch.c \
    src/peripherals/hex-display.c
BIN=clock_app

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(SRC)

clean:
	rm -f $(BIN)
