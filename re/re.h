#ifndef RE_H
#define RE_H

#define RE_MAX_MATCHES 30
#define RE_BUF_SIZE 128

typedef struct rematch {
    char buf[RE_BUF_SIZE];
}rematch_t;

int re_match(const char *re, const char * str, rematch_t * res);

#endif /* RE_H*/
