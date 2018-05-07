// gcc -o prog main.c -lutil

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <pty.h>
#define BUF_SIZE 1024

struct termios tty_orig;

static void             /* Reset terminal mode on program exit */
tty_reset(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty_orig) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
	}
}

FILE* unbuff_popen(char* cmd, char* arg[], char* env[]) {
    
    int pty;
    struct winsize ws;
    
    if (tcgetattr(STDIN_FILENO, &tty_orig) == -1) {
        perror("tcgetattr");
        return NULL;
	}
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &ws) < 0) {
        perror("TIOCGWINSZ error");
        return NULL;
	}

    switch (forkpty(&pty, NULL, &tty_orig, &ws)) {
        case -1:
            perror("pty fork");
            return NULL;
        case 0:
            execve(cmd, arg, env);
            perror("execve");
        default:    /* Parent*/

            if (atexit(tty_reset) != 0) {
                perror("atexit");
                return NULL;
            }

            return fdopen(pty, "r");
    }
}
int
main(int argc, char *argv[])
{
    char buf[BUF_SIZE];
    char *arg[] = {argv[1], argv[2], NULL };
    char *env[] = { NULL };
    FILE* fp = unbuff_popen(argv[1], arg, env);
	if(fp) {
    	while(fgets( buf, BUF_SIZE, fp))
        	printf("out: %s", buf);
	}
	return 0;
}
