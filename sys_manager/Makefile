CFLAGS := -std=gnu11 -Wall -Wpedantic
PROGS  := main
.PHONY: all clean distclean


all: $(PROGS)

main: sys_manager.o main.o
	gcc  $^ -o $@ -lsensors
main.o: main.c
	gcc $(CFLAGS) -c $^
sys_manager.o: sys_manager.c
	gcc $(CFLAGS) -c $^
clean:
	@- rm *.o $(PROGS) 

distclean: clean
