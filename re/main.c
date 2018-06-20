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

/*void
zm(void) {
    char *re, *str;
    rematch_t result[RE_MAX_MATCHES]; // result[0] contains full matched string
    int n;
    //
    memset(&result, 0, sizeof result);
    str = "2018-06-15 05:08:18 PM|#  pool1 eu1-zcash.flypool.org:3333";
    re = // 4 fields
    "\\|#\\s+"
    "(pool)"
    "([0-9]+)\\s+"
    "(.+)\\W?";
    
    n = re_match(re, str, result);
    print_result(result, n);
    //
    memset(&result, 0, sizeof result);
    str = "2018-06-15 05:08:18 PM|#  GPU0 + GeForce GTX 1060 3GB     MB: 3019  PCI: 2:0";
    re = // 9 fields
    "\\|#\\s+"
    "(GPU)"
    "([0-9]+)\\s+"                
    "([\\+\\-])\\s+"
    "(.+)"
    "(MB):\\s+"
    "([0-9]+)\\s+"
    "(PCI):\\s+"
    "([0-9]:[0-9])"
    "\\W?";
    
    n = re_match(re, str, result);
    print_result(result, n);
    //
    memset(&result, 0, sizeof result);
    str = "2018-06-15 05:08:18 AM|>  GPU0  68C  43% |  295.1 Sol/s   295.1 Avg   158.5 I/s | 2.82 S/W  105 W |  0.49  .  80 ++";
    re = // 22 fields
    "([0-9]{4}-[0-9]{2}-[0-9]{2}\\s+[0-9]{2}:[0-9]{2}:[0-9]{2})\\s+" // datetime
    "([PA]M)"                                                        // AM or PM
    "\\|\\W+"                                                        // |
    "(GPU)([0-9]+)\\s+"                                          // GPU n
    "([0-9]+)(\\w+)\\s+"                                         // tC
    "([0-9]+)(\\W)\\s+"                                          // RPM%
    "\\|\\s+"                                                    // |                            
    "([0-9]+\\.[0-9]+)\\s+"                                  // 0.00   
    "(\\w+\\W?\\w+)\\s+"                                     // Sol/s
    "([0-9]+\\.[0-9]+)\\s+"                                  // 0.00 
    "(\\w+)\\s+"                                             // Avg
    "([0-9]+\\.[0-9]+)\\s+"                                  // 0.00 
    "(\\w+\\W?\\w+)\\s+"                                     // I/s
    "\\|\\s+"                                                // |
    "([0-9]+\\.[0-9]+)\\s+"                                  // 0.00 
    "(\\w+\\W?\\w+)\\s+"                                     // S/W
    "([0-9]+)\\s+"                                           // 000
    "(\\w+)\\s+"                                             // W
    "\\|\\s+"                                                // |
    "([0-9]+\\.[0-9]+)\\s+"                                  // last 3 fields
    "([0-9]+|\\.)\\s+"
    "([0-9]+|\\.)\\W?";

    n = re_match(re, str, result);
    print_result(result, n);

//
    memset(&result, 0, sizeof result);
    str = "2018-06-15 05:08:18 PM|   ============== |  295.1 Sol/s   295.1 Avg   158.5 I/s | 2.82 S/W  105 W |  0.49  100  80 ++";
    re = // 16 fields
    "([0-9]{4}-[0-9]{2}-[0-9]{2}\\s+[0-9]{2}:[0-9]{2}:[0-9]{2})\\s+"
    "([PA]M)"
    "\\|\\W+"
    "\\|\\s+"
    "([0-9]+\\.[0-9]+)\\s+"
    "(\\w+\\W?\\w+)\\s+"
    "([0-9]+\\.[0-9]+)\\s+"
    "(\\w+)\\s+"
    "([0-9]+\\.[0-9]+)\\s+"
    "(\\w+\\/\\w+)\\s+"
    "\\|\\s+"
    "([0-9]+\\.[0-9]+)\\s+"
    "(\\w+\\/\\w+)\\s+"
    "([0-9]+)\\s+"
    "(\\w+)\\s+"
    "\\|\\s+"
    "([0-9]+\\.[0-9]+)\\s+"
    "([0-9]+|\\.)\\s+"
    "([0-9]+|\\.)\\W?";

    n = re_match(re, str, result);
    print_result(result, n);
}

void
zec(void) {
    char *re, *str;
    rematch_t result[RE_MAX_MATCHES];
    int n;
    //
    memset(&result, 0, sizeof result);
    str = "INFO: Current pool: eu1-zcash.flypool.org:3333";
    re = // 3 fields
    "^INFO:.+(pool):\\s+"
    "(.+)\\W?";

    n = re_match(re, str, result);
    print_result(result, n);
    //
    memset(&result, 0, sizeof result);
    str = "CUDA: Device: 0 GeForce GTX 1060 3GB, 3019 MB i:64";
    re = // 5 fields
    "^CUDA:.+Device:\\s"
    "([0-9]+)\\s"
    "(.+GTX\\s)"
    "([0-9]+)\\s"
    "([0-9]+GB)\\W?";

    n = re_match(re, str, result);
    print_result(result, n);
    //
    memset(&result, 0, sizeof result);
    str = "GPU0: 277 Sol/s GPU1: 178 Sol/s ";
    re = // 7 fields
    "^GPU"
    "([0-9]+):\\s"
    "([0-9]+)\\s"
    "(\\w+\\W?\\w+)\\s+"
    "GPU([0-9]+):\\s"
    "([0-9]+)\\s"
    "(\\w+\\W?\\w+)\\W?";

    n = re_match(re, str, result);
    print_result(result, n);
    //
    memset(&result, 0, sizeof result);
    str = "Total speed: 455 Sol/s";
    re = // 3 fields
    "^Total speed:\\s"
    "([0-9]+)\\s"
    "(\\w+\\W?\\w+)\\W?";

    n = re_match(re, str, result);
    print_result(result, n);
    //
    memset(&result, 0, sizeof result);
    str = "|  0  |    107W     |  2.59 Sol/W  |";
    re = // 6 fields
    "^"
    "\\|\\s+([0-9]+)\\s+"
    "\\|\\s+([0-9]+)(\\w+)\\s+"
    "\\|\\s+([0-9]+\\.[0-9]+)\\s"
    "(\\w+\\W?\\w+)\\s+"
    "\\|\\W?";

    n = re_match(re, str, result);
    print_result(result, n);
}
*/
int
main() {
    //zm();
    //zec();
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
