

#ifndef MINER_INFO_H
#define MINER_INFO_H

#define MI_PCI_LEN  16
#define MI_POOL_LEN 64
#define MI_GPUNAME_LEN 128
#define MI_GPU_NUM 64


typedef struct gpu_info {
    char name[MI_GPUNAME_LEN];
    char pci[MI_PCI_LEN];
    float hashrate;
}gpu_info_t;



typedef struct miner_info {
    const char *name;
    char pool[MI_POOL_LEN];
    gpu_info_t gpus[MI_GPU_NUM];
    int ngpu;
}miner_info_t;

typedef struct dual_miner_info {
    const char *name;
    miner_info_t mi[2];
} dual_miner_info_t;

void 
miner_info_dump(const miner_info_t *mi, char *buf, size_t len) {
   snprintf(buf, len, "dump miner_info\nminer: %s pool: %s ngpu: %d\n", mi->name, mi->pool, mi->ngpu);
    for(int i = 0; i < mi->ngpu; i++) {
        int l = strlen(buf);
        const gpu_info_t *gi = &mi->gpus[i];
        snprintf(&buf[l], len - l, "pci: %s\tGPU%d: %s\t%.2f Sol/s\n", gi->pci, i, gi->name, gi->hashrate);
    }
}

void 
dual_miner_info_dump(const dual_miner_info_t *dmi, char *buf, size_t len) {
    int l;
    const miner_info_t *mi;
    
   snprintf(buf, len, "dump dual_miner_info\nminer: %s\n", dmi->name);
   l = strlen(buf);   
   mi = &dmi->mi[0];
   snprintf(&buf[l], len - l, "coin: %s pool: %s ngpu: %d\n", mi->name, mi->pool, mi->ngpu);
    for(int i = 0; i < mi->ngpu; i++) {
        l = strlen(buf);
        const gpu_info_t *gi = &mi->gpus[i];
        snprintf(&buf[l], len - l, "pci: %s\tGPU%d: %s\t%.2f Sol/s\n", gi->pci, i, gi->name, gi->hashrate);
    }
    l = strlen(buf);
    mi = &dmi->mi[1];
    snprintf(&buf[l], len - l, "coin: %s pool: %s ngpu: %d\n", mi->name, mi->pool, mi->ngpu);    
    for(int i = 0; i < mi->ngpu; i++) {
        l = strlen(buf);
        const gpu_info_t *gi = &mi->gpus[i];
        snprintf(&buf[l], len - l, "pci: %s\tGPU%d: %s\t%.2f Sol/s\n", gi->pci, i, gi->name, gi->hashrate);
    }
}

#endif /* MINER_INFO_H */

