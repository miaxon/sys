

#ifndef RE_EWEBFMINER_0_3_4B_H
#define RE_EWEBFMINER_0_3_4B_H
#include "re_context.h"
#include "miner_info.h"
#include "re.h"

#define MINER_NAME "ewebfminer-0.3.4b"
#define MAX_LINE_LENGTH 128
#define LOG_FILE "zec.out"

// массив регулярок для парсинга "шапки" выхлопа майнера
static const char *init[] = 
{
    //"INFO: Current pool: eu1-zcash.flypool.org:3333"
    "^INFO: Current pool:\\s(.+)",
    
    // "CUDA: Device: 0 GeForce GTX 1060 3GB, 3019 MB i:64"
    "^CUDA: Device:\\s([0-9]+)\\s(.+),"
};
#define INIT_SIZE (sizeof(init)/sizeof(const char*))

// массив регулярок для парсинга диагностических сообщений в выхлопе майнера
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

// массив регулярок для парсинга сообщений об ошибках в выхлопе майнера
static const char *error[] = {
    "error message template"
};
#define ERROR_SIZE (sizeof(error)/sizeof(const char*))

// контекст парсинга выхлопа
re_context context = 
{
    .init_size  = INIT_SIZE,
    .info_size  = INFO_SIZE,
    .error_size = ERROR_SIZE,
    .re_init    = init,
    .re_info    = info,
    .re_error   = error,
    .target     = INIT,
    .error      = 0
};

// структура с информацией из выхлопа
miner_info_t mi = 
{
    .name = MINER_NAME,
    .pool = {0}
};

miner_info_t *
parse(char *buf) {
    int n_match;
    rematch_t result[RE_MAX_MATCHES] = {0};
    n_match = re_parse(&context, buf, result);

    if(n_match != -1) {
        switch(context.target) {
            case INIT: // init re array
                switch(n_match) {
                    case 0: // pool
                        strncpy(mi.pool, result[0].buf, MI_POOL_LEN - 1);
                        break;
                    case 1:// gpu device
                    {
                        int n = atoi(result[0].buf);
                        gpu_info_t *gi = &mi.gpus[n];
                        memset(gi, 0, sizeof *gi);
                        strncpy(gi->name, result[1].buf, MI_GPUNAME_LEN - 1);
                        mi.ngpu++;
                    }
                }
                break;
            case INFO: // info re array
                switch(n_match) {
                    case 0: // hashrate
                    {
                        int i = 0;
                        do {
                            int n = atoi(result[i++].buf);
                            gpu_info_t *gi = &mi.gpus[n];
                            gi->hashrate = atof(result[i++].buf);
                        } while(strlen(result[i].buf));
                    }
                        break;

                }
                break;
        }
        return &mi;
    }
    return NULL;
}


#endif /* RE_EWEBFMINER_0_3_4B_H */

