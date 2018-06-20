

#ifndef MINER_INFO_H
#define MINER_INFO_H

typedef struct gpu_info {
    char name[128];
    char pci[16];
    float hashrate;
}gpu_info_t;

typedef struct miner_info {
    char name[64];
    char pool[64];
    gpu_info_t gpus[100];
    int ngpu;
}miner_info_t;

#endif /* MINER_INFO_H */

