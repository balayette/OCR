FLAGS= -Wall -Wextra -pedantic -std=c99 -Werror -g -lm

SRC=$(wildcard sources/*.c)

main: $(SRC)
	clang -o $@.out $^ $(FLAGS)

clean:
	rm ./main.out
