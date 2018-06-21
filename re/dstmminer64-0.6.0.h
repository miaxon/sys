

#ifndef RE_DSTMMINER64_0_6_0_H
#define RE_DSTMMINER64_0_6_0_H
#include "re_context.h"
#include "miner_info.h"
#include "re.h"

#define MINER_NAME "dstmminer64-0.6.0"
#define LOG_FILE "zm.out"

static const char *init[] = 
{
    //"connected to: eu1-zcash.flypool.org:3333 [1/2]"
    "connected to:\\s(.+:[0-9]+)",
    
    // "2018-06-15 05:08:18 PM|#  GPU0 + GeForce GTX 1060 3GB     MB: 3019  PCI: 2:0"
    "GPU([0-9]+)\\s[\\+\\-]?\\s(.+\\w)\\s+MB:.+PCI:\\s([0-9]+:[0-9]+)"
};
#define INIT_SIZE (sizeof(init)/sizeof(const char*))

static const char *info[] = 
{
    // "2018-06-15 05:08:18 AM|>  GPU0  68C  43% |  295.1 Sol/s   295.1 Avg   158.5 I/s | 2.82 S/W  105 W |  0.49  .  80 ++" 
    "GPU([0-9]+).+\\s([0-9]+\\.?[0-9]+)\\sSol\\/s"      
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

int
parse(char *buf, miner_info_t *mi) {
    int n_match;
    rematch_t result[RE_MAX_MATCHES] = {0};
    n_match = re_parse(&context, buf, result);
    
    if(n_match != -1) {
        switch(context.target) {
            case INIT: // init re array
                switch(n_match) {
                    case 0: // pool
                    {
                        strncpy(mi->pool, result[0].buf, 64);
                    }
                        break;
                    case 1:// gpu device
                    {
                        int n = atoi(result[0].buf);
                        gpu_info_t *gi = &mi->gpus[n];
                        memset(gi, 0, sizeof *gi);
                        strncpy(gi->name, result[1].buf, 128);
                        strncpy(gi->pci, result[2].buf, 16);
                        mi->ngpu++;
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
                            gpu_info_t *gi = &mi->gpus[n];
                            gi->hashrate = atof(result[i++].buf);
                            //i += 2;
                        } while(strlen(result[i].buf));
                    }
                        break;

                }
                break;
        }
    }
    return n_match;
}

#endif /* RE_DSTMMINER64_0_6_0_H */

