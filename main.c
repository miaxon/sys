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

FILE* run(char* cmd, char* arg[], char* env[]) {
    
    int slaveFd;
    int masterFd;
    
    
    struct winsize ws;
    
    if (tcgetattr(STDIN_FILENO, &ttyOrig) == -1)
        perror("tcgetattr");
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &ws) < 0)
        perror("TIOCGWINSZ error");

    openpty(&masterFd, &slaveFd, NULL, &ttyOrig, &ws);
    switch (fork()) {
        case -1:
            perror("ptyFork");
        case 0:
            close(masterFd);
            if (setsid() == -1)               
                perror("ptyFork:setsid");        
            if (slaveFd == -1)
                perror("ptyFork:open-slave");
            if (ioctl(slaveFd, TIOCSCTTY, 0) == -1)
                perror("ptyFork:ioctl-TIOCSCTTY");
            if (dup2(slaveFd, STDIN_FILENO) != STDIN_FILENO)
                perror("ptyFork:dup2-STDIN_FILENO");
            if (dup2(slaveFd, STDOUT_FILENO) != STDOUT_FILENO)
                perror("ptyFork:dup2-STDOUT_FILENO");
            if (dup2(slaveFd, STDERR_FILENO) != STDERR_FILENO)
                perror("ptyFork:dup2-STDERR_FILENO");
            if (slaveFd > STDERR_FILENO)       
                close(slaveFd); 
                execve(cmd, arg, env);
                perror("execvp");
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
    FILE* fd = run(argv[1], arg, env);
    while(fgets( buf, BUF_SIZE, fd))
        printf("out: %s", buf);
}
