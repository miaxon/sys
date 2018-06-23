

#ifndef RE_DSTMMINER64_0_6_0_H
#define RE_DSTMMINER64_0_6_0_H
#include "re_context.h"
#include "miner_info.h"
#include "re.h"

#define MINER_NAME "dstmminer64-0.6.0"
#define MAX_LINE_LENGTH 128
#define LOG_FILE "zm.out"

// массив регулярок для парсинга "шапки" выхлопа майнера
static const char *init[] = 
{
    //"connected to: eu1-zcash.flypool.org:3333 [1/2]"
    "connected to:\\s(.+:[0-9]+)",
    
    // "2018-06-15 05:08:18 PM|#  GPU0 + GeForce GTX 1060 3GB     MB: 3019  PCI: 2:0"
    "GPU([0-9]+)\\s[\\+\\-]?\\s(.+\\w)\\s+MB:.+PCI:\\s([0-9]+:[0-9]+)"
};
#define INIT_SIZE (sizeof(init)/sizeof(const char*))

// массив регулярок для парсинга диагностических сообщений в выхлопе майнера
static const char *info[] = 
{
    // "2018-06-15 05:08:18 AM|>  GPU0  68C  43% |  295.1 Sol/s   295.1 Avg   158.5 I/s | 2.82 S/W  105 W |  0.49  .  80 ++" 
    "GPU([0-9]+).+\\s([0-9]+\\.?[0-9]+)\\sSol\\/s"      
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
                    {
                        strncpy(mi.pool, result[0].buf, MI_POOL_LEN - 1);
                    }
                        break;
                    case 1:// gpu device
                    {
                        int n = atoi(result[0].buf);
                        gpu_info_t *gi = &mi.gpus[n];
                        memset(gi, 0, sizeof *gi);
                        strncpy(gi->name, result[1].buf, MI_GPUNAME_LEN - 1);
                        strncpy(gi->pci, result[2].buf, MI_PCI_LEN - 1);
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

#endif /* RE_DSTMMINER64_0_6_0_H */

