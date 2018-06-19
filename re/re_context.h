
#ifndef RE_CONTEXT_H
#define RE_CONTEXT_H

typedef struct context {
    const int init_size;
    const int info_size;
    const int error_size;
    const char** re_init;
    const char** re_info;
    const char** re_error;
    int init_count;
} re_context;

#endif /* RE_CONTEXT_H */

