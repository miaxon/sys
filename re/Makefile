CFLAGS := -std=gnu11 -Wall -Wpedantic
PROGS  := main
.PHONY: all clean distclean


all: $(PROGS)

main: re.o main.o
	cc  $^ -o $@
main.o: main.c
	cc $(CFLAGS) -c $^
re.o: re.c
	cc $(CFLAGS) -c $^
clean:
	@- rm *.o $(PROGS) 

distclean: clean
