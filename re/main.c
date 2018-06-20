#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re.h"
#include "re_ewebfminer-0.3.4b.h"
#include "miner_info.h"

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

void ewebfminer_0_3_4b(char *buf, miner_info_t *mi) {
    int n_match;
    rematch_t result[RE_MAX_MATCHES];
    memset(&result, 0, sizeof result);
    n_match = re_parse(&context, buf, result);
    //print_result(buf, &context, n_re, result);return;

    if(n_match != -1) {
        switch(context.target) {
            case INIT: // init re array
                switch(n_match) {
                    case 0: // pool
                    {
                        strncpy(mi->pool, result[0].buf, 64);
                    }
                        break;
                    case 1:// gpu device
                    {
                        int n = atoi(result[0].buf);
                        gpu_info_t *gi = &mi->gpus[n];
                        memset(gi, 0, sizeof *gi);
                        strncpy(gi->name, result[1].buf, 128);
                        mi->ngpu++;
                    }
                }
                break;
            case INFO: // info re array
                switch(n_match) {
                    case 0: // hashrate
                    {
                        int i = 0;
                        do {
                            int n = atoi(result[i].buf);
                            gpu_info_t *gi = &mi->gpus[n];
                            gi->hashrate = atof(result[i + 1].buf);
                            i += 2;
                        } while(strlen(result[i].buf));
                    }
                        break;

                }
                break;
        }

        printf("miner: %s pool: %s ngpu: %d\n", mi->name, mi->pool, mi->ngpu);
        for(int i = 0; i < mi->ngpu; i++) {
            gpu_info_t *gi = &mi->gpus[i];
            printf("GPU%d: %s %.2f Sol/s\n", i, gi->name, gi->hashrate);
        }
        printf(">>>>>>>>>>>>\n");
    }
}

int
main() {
    char buf[128] = {0};
    miner_info_t mi;
    memset(&mi, 0, sizeof mi);
    strncpy(mi.name, "ewebfminer_0_3_4b", 64);
    //
    printf("parse 'zec.out'\n");
    FILE* fp = fopen("zec.out", "r");
    if(!fp) {
        printf("fopen failed\n");
        return 0;
    }

    while(fgets(buf, 128, fp)) {
        ewebfminer_0_3_4b(buf, &mi);
    }
    fclose(fp);
    return 0;
}
