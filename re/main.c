#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "ewebfminer-0.3.4b.h"
#include "dstmminer64-0.6.0.h"

void
print_result(const char *str, re_context *ctx, int n_re, rematch_t *result) {
    if(n_re == -1)
        return;

    char* target = (ctx->target == INIT) ? "INIT" : "INFO";
    printf("out string '%s', target %s, match %d, error %d\n", str, target, n_re, ctx->error);

    printf("[");
    for(int i = 0; i < RE_MAX_MATCHES; i++) {
        if(!strlen(result[i].buf)) {
            printf("\b\b]");
            break;
        }
        printf("%s, ", result[i].buf);
    }
    printf("\n========================\n");
}

void
print_miner_info(const miner_info_t *mi) {
    printf("miner: %s pool: %s ngpu: %d\n", mi->name, mi->pool, mi->ngpu);
    for(int i = 0; i < mi->ngpu; i++) {
        const gpu_info_t *gi = &mi->gpus[i];
        printf("pci: %s\tGPU%d: %s\t%.2f Sol/s\n", gi->pci, i, gi->name, gi->hashrate);
    }
    printf(">>>>>>>>>>>>\n");
}

int
main() {

    printf("parse %s\n", LOG_FILE);
    char buf[128];
    miner_info_t mi;
    strncpy(mi.name, MINER_NAME, 64);

    FILE* fp = fopen(LOG_FILE, "r");
    if(!fp) {
        printf("fopen failed\n");
        return 0;
    }

    while(fgets(buf, 128, fp)) {
        if(parse(buf, &mi) != -1)
            print_miner_info(&mi);
    }
    fclose(fp);
    return 0;
}
