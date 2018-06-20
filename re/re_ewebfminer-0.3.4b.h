

#ifndef RE_EWEBFMINER_0_3_4B_H
#define RE_EWEBFMINER_0_3_4B_H
#include "re_context.h"

static const char *init[] = 
{
    //"INFO: Current pool: eu1-zcash.flypool.org:3333"
    "^INFO: Current pool:\\s(.+)",
    
    // "CUDA: Device: 0 GeForce GTX 1060 3GB, 3019 MB i:64"
    "^CUDA: Device:\\s([0-9]+)\\s(.+),"
};
#define INIT_SIZE (sizeof(init)/sizeof(const char*))

static const char *info[] = 
{
    // "GPU0: 277 Sol/s GPU1: 178 Sol/s " 
    "([0-9]+)\\W+([0-9]+)\\sSol\\/s",
    
    // "Total speed: 455 Sol/s"
    //"^Total speed:\\s([0-9]+)\\sSol\\/s",
    
    // "|  0  |    107W     |  2.59 Sol/W  |"
    //"^\\|\\s+([0-9]+)\\s+"
    //"\\|\\s+([0-9]+)W\\s+"
    //"\\|\\s+([0-9]+\\.[0-9]+)\\sSol\\/W"
    
};
#define INFO_SIZE (sizeof(info)/sizeof(const char*))

static const char *error[] = {
    "error message template"
};
#define ERROR_SIZE (sizeof(error)/sizeof(const char*))

re_context context = 
{
    INIT_SIZE,
    INFO_SIZE,
    ERROR_SIZE,
    init,
    info,
    error,
    INIT,
    0
};

#endif /* RE_EWEBFMINER_0_3_4B_H */

