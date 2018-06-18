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
    const int n_matches = RE_MAX_MATCHES;
    regmatch_t m[n_matches];
    int i, start, finish;
    while(!regexec(&r, p, n_matches, m, 0)) {
        for (i = 0; i < n_matches; i++) {            
            if (m[i].rm_so == -1) break;            
            start = m[i].rm_so + (p - str);
            finish = m[i].rm_eo + (p - str);
            strncpy(res[i].buf, str + start, finish - start);
        }
        
        p += m[0].rm_eo;
    }
    return i;
}
