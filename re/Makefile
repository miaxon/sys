CFLAGS := -std=gnu11 -Wall -Wpedantic
PROGS  := main
.PHONY: all clean distclean


all: $(PROGS)

main: re.o main.o
	gcc  $^ -o $@
main.o: main.c
	gcc $(CFLAGS) -c $^
re.o: re.c
	gcc $(CFLAGS) -c $^
clean:
	@- rm *.o $(PROGS) 

distclean: clean
