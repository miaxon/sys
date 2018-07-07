#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys_manager.h"

int main() {

	sys_manager_init();
	//sys_manager_update();
	//sys_manager_update();

	char buf[1024] = {0};
	sys_manager_dump_string(buf, 1024);
	printf(buf);
	return 0;
}
