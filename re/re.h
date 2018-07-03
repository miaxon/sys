#ifndef RE_H
#define RE_H

#define RE_MAX_MATCHES 30
#define RE_MAX_MATCH_SIZE 128
#include "re_context.h"

typedef struct rematch {
    char buf[RE_MAX_MATCH_SIZE];
}rematch_t;

void
re_dump_result(const char *str, re_context *ctx, int n_re, rematch_t *result);

int
re_parse(re_context *ctx, const char *str, rematch_t *res);

#endif /* RE_H*/
