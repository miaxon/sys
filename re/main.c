#include <stdio.h>

#include "re.h"

void
print_result(rematch_t *result, int len) {
    printf("found %d matches:\n", len);
    for(int i = 0; i < len; i++) {
        printf("\t%s\n", result[i].buf);
    }
}

int
main() {
    char *re, *str;
    rematch_t result[RE_MAX_MATCHES];
    int n;
    //
    memset(result, 0, sizeof result);
    str = "2018-06-15 05:08:18 AM|>  GPU0  68C  43% |  295.1 Sol/s   295.1 Avg   158.5 I/s | 2.82 S/W  105 W |  0.49  100  80 ++";
    re = 
    "([0-9]{4}-[0-9]{2}-[0-9]{2}\\W+[0-9]{2}:[0-9]{2}:[0-9]{2})\\W+([PA]M)"
    "\\|\\W+"
    "(GPU)([0-9]{1,2})\\W+"
    "([0-9]{1,3})(C)\\W+"
    "([0-9]{1,3})(\\W)\\W+"
    "\\|\\W+"
    "([0-9]{1,5}\\.[0-9]{1,5})\\W+"
    "(\\w+\\W?\\w+)\\W+"
    "([0-9]{1,5}\\.[0-9]{1,5})\\W+"
    "(\\w+)\\W+"
    "([0-9]{1,5}\\.[0-9]{1,5})\\W+"
    "(\\w+\\/\\w+)\\W+"
    "\\|\\W+"
    "([0-9]{1,5}\\.[0-9]{1,5})\\W+"
    "(\\w+\\/\\w+)\\W+"
    "([0-9]{1,5})\\W+"
    "(\\w+)\\W+"
    "\\|\\W+"
    "([0-9]{1,5}\\.[0-9]{1,5})\\W+"
    "([0-9]{1,5}|\\.)\\W+"
    "([0-9]{1,5}|\\.)"
    "\\W?"
    ;

    n = re_match(re, str, result);
    if(n != -1)
    print_result(result, n);

    return 0;
    
}
