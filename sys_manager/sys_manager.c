#include <sys/sysinfo.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sys_manager.h"

static void err_sys(const char* msg){perror(msg); exit(1);}
static void err_exit(const char* msg){perror(msg); exit(1);}

static sys_info_t info;

int
sys_manager_init(void) {
	int err = 0;
	struct sysinfo s_info;

	if( (err = sysinfo(&s_info)) == -1)
		err_sys("Could not get sysinfo");
	
	info.uptime = s_info.uptime;
	memcpy(&info.cpu_info.loads, &s_info.loads, sizeof info.cpu_info.loads);
	info.mem_info.totalram = s_info.totalram;
	info.mem_info.freeram = s_info.freeram;
	info.mem_info.totalswap = s_info.totalswap;
	info.mem_info.freeswap = s_info.freeswap;
	return err;
}

int
sys_manager_destroy(void) {
	int err = 0;
	return err;
}

void
sys_manager_dump_string(char *buf, size_t len) {
	snprintf(buf, len,
			"uptime:     %lu\n"
			"loads:      %lu, %lu, %lu\n"
			"totalram:   %lu\n"
			"freeram:    %lu\n"
			"totalswap:  %lu\n"
			"freeswap:   %lu\n"
			, 
			info.uptime,
			info.cpu_info.loads[0], info.cpu_info.loads[1], info.cpu_info.loads[2],
			info.mem_info.totalram,
			info.mem_info.freeram,
			info.mem_info.totalswap,
			info.mem_info.freeswap);
}

int
sys_manager_update(void) {
	int err = 0;
	return err;
}
