
#ifndef SYS_MANAGER_H
#define SYS_MANAGER_H
#define SYS_STR_LEN 128
#define SYS_MAX_NET_INTERFACES 64

typedef struct net_info {
    char            hostname[SYS_STR_LEN];
    char            dev[SYS_STR_LEN];
} net_info_t;

typedef struct disk_info {
    unsigned long totalspace;
    unsigned long freespace;
} disk_info_t;

typedef struct memory_info {
    unsigned long totalram; /* Total usable main memory size */
    unsigned long freeram; /* Available memory size */
    unsigned long totalswap; /* Total swap space size */
    unsigned long freeswap; /* swap space still available */
} memory_info_t;

typedef struct cpu_info {
    char model[SYS_STR_LEN];
    char vendor[SYS_STR_LEN];
    unsigned int cpus;
    double freq;
    double freqmax;
    double freqmin;
    unsigned int temperature;
    unsigned long loads[3]; /* 1, 5, and 15 minute load averages */
} cpu_info_t;

typedef struct sys_info {
    char os[SYS_STR_LEN];
    long uptime; /* Seconds since boot */
    memory_info_t mem_info;
    cpu_info_t cpu_info;
    disk_info_t disk_info;
    net_info_t net_info;
} sys_info_t;



int
sys_manager_init(void);

int
sys_manager_destroy(void);

void
sys_manager_dump_string(char *buf, size_t len);

int
sys_manager_update(void);

#endif /* SYS_MANAGER_H */

