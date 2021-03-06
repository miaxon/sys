#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "re.h"


#define RE_MAX_ERROR_MSG 0x1000

static int re_compile(regex_t * r, const char * regex_text);
static int re_match(const char *re, const char *str, rematch_t *res);

void
re_dump_result(const char *str, re_context *ctx, int n_re, rematch_t *result) {
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

static int
re_compile(regex_t * r, const char *regex_text) {
    int status;

    if(!regex_text || !strlen(regex_text)) {
        printf("re_compile error: regex_text is mull or empty\n");
        return 1;
    }

    if((status = regcomp(r, regex_text, REG_EXTENDED | REG_NEWLINE)) != 0) {
        char error_message[RE_MAX_ERROR_MSG];
        regerror(status, r, error_message, RE_MAX_ERROR_MSG);
        printf("Regex error compiling %s: %s\n",
                regex_text, error_message);
        return 1;
    }
    return 0;
}

static int
re_match(const char *re, const char *str, rematch_t *res) {
    regex_t r;
    if(re_compile(&r, re) != 0)
        return -1;

    const char *p = str;
    const int N = RE_MAX_MATCHES;
    int n = 0, len;
    regmatch_t m[N];

    while(!regexec(&r, p, N, m, 0)) {
        for(int i = 0; i < N; i++) {
            if(m[i].rm_so == -1)
                break;
            if(i > 0) {
                len = m[i].rm_eo - m[i].rm_so;
                strncpy(res[n++].buf, p + m[i].rm_so, (len < RE_MAX_MATCH_SIZE) ? len : RE_MAX_MATCH_SIZE - 1);
            }
        }
        p += m[0].rm_eo;
    }
    regfree(&r);

    return n;
}

int
re_parse(re_context *ctx, const char *str, rematch_t *res) {

    int n_match = 0;
    ctx->error = 0;

    if(ctx->target == INIT) {
        for(int i = 0; i < ctx->init_size; i++) {
            const char *re = ctx->re_init[i];
            if((n_match = re_match(re, str, res)) > 0) {
                return i;
            }
        }
    }

    for(int i = 0; i < ctx->info_size; i++) {
        const char *re = ctx->re_info[i];
        if((n_match = re_match(re, str, res)) > 0) {
            ctx->target = INFO;
            return i;
        }
    }

    for(int i = 0; i < ctx->error_size; i++) {
        const char *re = ctx->re_error[i];
        if((n_match = re_match(re, str, res)) > 0) {
            ctx->error = 1;
            return i;
        }
    }

    return -1;
}