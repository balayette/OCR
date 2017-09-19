FLAGS= -Wall -Wextra -pedantic -std=c99 -Werror -g -lm

SRC=$(wildcard sources/*.c)

main: $(SRC)
	clang -o $@ $^ $(FLAGS)

clean:
	rm ./main
