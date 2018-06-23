#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "ewebfminer-0.3.4b.h"
#include "dstmminer64-0.6.0.h"


void
print_miner_info(const miner_info_t *mi) {
    char buf[1024] = {0};
    miner_info_dump(mi, buf, 1024);
    printf(buf);
}

int
main() {

    printf("parse %s\n", LOG_FILE);
    char buf[MAX_LINE_LENGTH];

    FILE* fp = fopen(LOG_FILE, "r");
    if(!fp) {
        printf("fopen failed\n");
        return 0;
    }

    while(fgets(buf, MAX_LINE_LENGTH, fp)) {
        miner_info_t *mi = parse(buf);
        if(mi)
            print_miner_info(mi);
    }
    fclose(fp);
    return 0;
}
