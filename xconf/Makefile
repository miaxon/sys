CFLAGS := -std=gnu11 -Wall -Wpedantic
PROGS  := main
.PHONY: all clean distclean

all: $(PROGS)

main: main.o
	cc  $^ -o $@ -lpci
main.o: main.c
	cc $(CFLAGS) -c $^

clean:
	@- rm *.o $(PROGS) 

distclean: clean
