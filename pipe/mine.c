#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  setvbuf(stdout, NULL, _IONBF, 0);
	for(int i = 0; i < 10; i++) {
		printf("%d --- sample line output\n", i);
		sleep(1);
//		fflush(stdout);
	}
	return 1;
}
