CFLAGS := -std=gnu11 -Wall -Wpedantic
PROGS  := pipe mine
.PHONY: all clean distclean

test: all
	./pipe

all: $(PROGS)

pipe: pipe.o
	cc  $^ -o $@
pipe.o: pipe.c
	cc $(CFLAGS) -c $^
mine: mine.o
	cc $^ -o $@
mine.o: mine.c
	cc $(CFLAGS) -c $^
clean:
	@- rm *.o $(PROGS) 

distclean: clean
