// gcc -o prog main.c -lutil

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include <pty.h>
#define BUF_SIZE 1024

struct termios ttyOrig;

static void             /* Reset terminal mode on program exit */
ttyReset(void)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, &ttyOrig) == -1)
        perror("tcsetattr");
}

FILE* unbuff_popen(char* cmd, char* arg[], char* env[]) {
    
    int masterFd;
    struct winsize ws;
    
    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
        perror("tcgetattr");
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &ws) < 0)
        perror("TIOCGWINSZ error");

    switch (forkpty(&masterFd, NULL, &ttyOrig, &ws)) {
        case -1:
            perror("ptyFork");
        case 0:
                execve(cmd, arg, env);
                perror("execve");
        default:    /* Parent*/

            if (atexit(ttyReset) != 0)
                perror("atexit");

            return fdopen(masterFd, "r");
    }
}
int
main(int argc, char *argv[])
{
    char buf[BUF_SIZE];
    char *arg[] = {argv[1], argv[2], NULL };
    char *env[] = { NULL };
    FILE* fd = unbuff_popen(argv[1], arg, env);
    while(fgets( buf, BUF_SIZE, fd))
        printf("out: %s", buf);
}
