#include <stdio.h>
#include <string.h>
#include "re.h"
#include "re_ewebfminer-0.3.4b.h"



void
print_result(const char *str, re_context *ctx, int n_re, rematch_t *result) {
    
    
    if(n_re == -1)        
        return;
    
    char* target = (ctx->target == INIT)? "INIT" : "INFO";
    printf("out string '%s', target %s, match %d, error %d\n", str, target, n_re, ctx->error);
    
    for(int i = 0; i < RE_MAX_MATCHES; i++) {
        if(!strlen(result[i].buf))
            break;
        
        printf("\t%s ", result[i].buf);
    }
    printf("\n========================\n");
}


int
main() {
    char buf[128] = {0};
    rematch_t result[RE_MAX_MATCHES];
    int n_re;
    //
    printf("parse zec.out\n");
    FILE* fp = fopen("zec.out", "r");
    if(!fp) {
        printf("fopen failed\n");
        return 0;
    }
    
    while(fgets(buf, 128, fp)) {
        memset(&result, 0, sizeof result);
        n_re = re_parse(&context, buf, result);        
        print_result(buf, &context, n_re, result);
    }
    fclose(fp);
    return 0;    
}
