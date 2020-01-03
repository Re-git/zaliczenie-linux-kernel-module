CC = gcc
FLAGS := -pthread -g -Wpedantic -Wall -Werror -pedantic-errors -std=c11
OUT = prog3

SRC = $(wildcard *.c)
INC = $(wildcard *.h)

$(OUT): $(SRC) $(INC)
	$(CC) $(FLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)

