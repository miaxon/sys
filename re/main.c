#include <stdio.h>
#include <string.h>
#include "re.h"
#include "re_ewebfminer-0.3.4b.h"

static const char *zec_out[] = 
{
    "bla-bla",
    "INFO: Current pool: eu1-zcash.flypool.org:3333",
    "CUDA: Device: 0 GeForce GTX 1060 3GB, 3019 MB i:64",
    "GPU0: 277 Sol/s GPU1: 178 Sol/s ",
    "Total speed: 455 Sol/s",
    "bla-bla",
    "|  0  |    107W     |  2.59 Sol/W  |",
    "|  1  |    200W     |  6.89 Sol/W  |"
};
#define ZEC_OUT_SIZE (sizeof(zec_out)/sizeof(const char*))

void
print_result(rematch_t *result, int len) {
    if( len == -1) {
        printf("re_match failed!\n");
        return;
    }
    printf("found %d matches:\n", len);
    for(int i = 0; i < len; i++) {
        printf("\t%s\n", result[i].buf);
    }
}

void
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

int
main() {
    //zm();
    //zec();
    rematch_t result[RE_MAX_MATCHES];
    int n;
    //
    printf("parse %ld lines\n", ZEC_OUT_SIZE);
    for(int i = 0; i < ZEC_OUT_SIZE; i++) {
        memset(&result, 0, sizeof result);
        n = re_parse(&context, zec_out[i], result);
        print_result(result, n);
    }
    return 0;    
}
