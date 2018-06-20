
#ifndef RE_CONTEXT_H
#define RE_CONTEXT_H

typedef enum target{
    INIT,
    INFO        
}re_target;

typedef struct context {
    const int init_size;
    const int info_size;
    const int error_size;
    const char** re_init;
    const char** re_info;
    const char** re_error;
    re_target target;
    int error;
} re_context;

#endif /* RE_CONTEXT_H */

