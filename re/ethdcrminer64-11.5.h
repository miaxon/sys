
#include <string.h>

#ifndef RE_ETHDCRMINER64_11_5_H
#define RE_ETHDCRMINER64_11_5_H
#include "re_context.h"
#include "miner_info.h"
#include "re.h"

#define MINER_NAME "ethdcrminer64-11.5"
#define COIN_0 "ETH"
#define COIN_1 "DCR"
#define MAX_LINE_LENGTH 128
#define LOG_FILE "ethdcrminer64-11.5.out"

// массив регулярок для парсинга "шапки" выхлопа майнера
static const char *init[] = 
{
    "Main Ethereum pool is\\s(.+)",
    "Main Decred pool is\\s(.+)",
    "GPU #([0-9]+): (.+),?.+\\(pci bus (.+)\\)"
};
#define INIT_SIZE (sizeof(init)/sizeof(const char*))

// массив регулярок для парсинга диагностических сообщений в выхлопе майнера
static const char *info[] = 
{
    "([0-9]+) ([0-9]+.?[0-9]+) Mh\\/s,?"    
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
dual_miner_info_t dmi = 
{
    MINER_NAME,
    { {COIN_0}, {COIN_1}} 
};

dual_miner_info_t *
parse(char *buf) {
    int n_match;
    miner_info_t *mi;
    gpu_info_t *gi;    
    rematch_t result[RE_MAX_MATCHES] = {0};
    
    n_match = re_parse(&context, buf, result);

    if(n_match != -1) {
        switch(context.target) {
            case INIT: // init re array
                switch(n_match) {
                    case 0: // pool
                        strncpy(dmi.mi[0].pool, result[0].buf, MI_POOL_LEN - 1);
                        break;
                    case 1: // pool
                        strncpy(dmi.mi[1].pool, result[0].buf, MI_POOL_LEN - 1);
                        break;
                    case 2:// gpu device
                    {
                        int n = atoi(result[0].buf);
                        
                        mi = &dmi.mi[0];                        
                        gi = &mi->gpus[n];
                        memset(gi, 0, sizeof *gi);
                        strncpy(gi->name, result[1].buf, MI_GPUNAME_LEN - 1);
                        strncpy(gi->pci, result[2].buf, MI_PCI_LEN - 1);
                        mi->ngpu++;
                        
                        mi = &dmi.mi[1];                        
                        gi = &mi->gpus[n];
                        memset(gi, 0, sizeof *gi);
                        strncpy(gi->name, result[1].buf, MI_GPUNAME_LEN - 1);
                        strncpy(gi->pci, result[2].buf, MI_PCI_LEN - 1);
                        mi->ngpu++;
                    }
                }
                break;
            case INFO: // info re array
                switch(n_match) {
                    case 0: // hashrate
                    {
                        if(strstr(buf, COIN_0)) { // ETH
                            mi = &dmi.mi[0]; 
                            int i = 0;
                            do {                            
                                int n = atoi(result[i++].buf);
                                gi = &mi->gpus[n];
                                gi->hashrate = atof(result[i++].buf);
                            } while(strlen(result[i].buf));
                        }
                        
                        if(strstr(buf, COIN_1)) { // DCR
                            mi = &dmi.mi[1]; 
                            int i = 0;
                            do {                            
                                int n = atoi(result[i++].buf);
                                gi = &mi->gpus[n];
                                gi->hashrate = atof(result[i++].buf);
                            } while(strlen(result[i].buf));
                        }
                    }
                        break;

                }
                break;
        }
        return &dmi;
    }
    return NULL;
}


#endif /* RE_ETHDCRMINER64_11_5_H */

