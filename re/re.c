#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "re.h"


#define RE_MAX_ERROR_MSG 0x1000

static int re_compile (regex_t * r, const char * regex_text);

static int
re_compile (regex_t * r, const char * regex_text) {
    int status;
    if ( (status = regcomp (r, regex_text, REG_EXTENDED | REG_NEWLINE)) != 0) {
        char error_message[RE_MAX_ERROR_MSG];
        regerror (status, r, error_message, RE_MAX_ERROR_MSG);
        printf("Regex error compiling %s: %s\n",
                regex_text, error_message);
        return 1;
    }
    return 0;
}

int 
re_match(const char *re, const char *str, rematch_t *res) {
    regex_t r;
    if(re_compile(&r, re) != 0)
        return -1;
    
    const char *p = str;
    const int N = RE_MAX_MATCHES;
    int n = 0, len;
    regmatch_t m[N];

    while(!regexec(&r, p, N, m, 0)) {
        for (int i = 0; i < N; i++) {            
            if (m[i].rm_so == -1) 
                break;            
            len = m[i].rm_eo - m[i].rm_so;
            strncpy(res[i].buf, p + m[i].rm_so, (len < RE_BUF_SIZE) ? len : RE_BUF_SIZE);
            n++;
        }        
        p += m[0].rm_eo;
    }
    regfree(&r);
    return n;
}
