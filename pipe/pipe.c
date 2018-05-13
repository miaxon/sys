#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define CMD "./mine"

#define BUF_SIZE 1024
static char* cmd = CMD;
static char buf[BUF_SIZE] = {0};
int read_pipe_for_command(char *cmd) {
    int p[2];
    if(pipe(p))
        return -1;
// try pipe in non-block mode
/*    if(fcntl(p[0], F_SETFL, O_NONBLOCK)) {
        close(p[0]);
        close(p[1]);
        return -1;
    }
    if(fcntl(p[1], F_SETFL, O_NONBLOCK)) {
        close(p[0]);
        close(p[1]);
        return -1;
    }
*/
    switch(fork()) {
        case -1:
            close(p[0]);
            close(p[1]);
            return -1;
        break;
        case 0:
            close(p[0]);
			while( (dup2(p[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
			char* argv[] ={cmd, NULL}; 
			execv(cmd, argv);
			printf("execv: [%d] %s\n", errno, strerror(errno));
            exit(-1);
        break;
        default:
            close(p[1]);
			return p[0];
    }
}

int main() {
	printf("exec cmd %s\n Output:\n", cmd);
    int fd = read_pipe_for_command(CMD);
    if(fd < 0)
        return 0;
    for(;;) {
        size_t n = read(fd, buf, BUF_SIZE);
        if (n == -1){
			printf("Error reading: [%d] %s\n", errno, strerror(errno));
			break;
		}

/*        if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            printf("waiting data [%d] %s \n", errno, strerror(errno));
			sleep(1);
            continue;
        } else {
			printf("Error reading: [%d] %s\n", errno, strerror(errno));
			break;
		}
*/
        if (n > 0) {
            printf("mine print: [len=%zd] %s", n, buf);
        }
		if(n == 0){
            break;
        }
    }
    close(fd);
	return 0;
}
