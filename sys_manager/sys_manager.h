
#ifndef SYS_MANAGER_H
#define SYS_MANAGER_H
#define SYS_STRLEN_1 128
#define SYS_STRLEN_2 16
#define SYS_MAX_NET_INTERFACES 64

typedef struct net_info {
    char   hostname[SYS_STRLEN_1];
    char   dev     [SYS_STRLEN_1];
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
    int           id;
    char          model [SYS_STRLEN_1];
    char          arch  [SYS_STRLEN_2];
    char          vendor[SYS_STRLEN_1];
    unsigned int  cpus;
    double        freq;
    double        freq_max;
    double        freq_min;
    double        temperature;
    double        temperature_max;
    unsigned long loads[3]; /* 1, 5, and 15 minute load averages */
} cpu_info_t;

typedef struct os_info {
    char sysname [SYS_STRLEN_1];
    char nodename[SYS_STRLEN_1];
    char release [SYS_STRLEN_1];
    char version [SYS_STRLEN_1];
    char machine [SYS_STRLEN_2];
}os_info_t;

typedef struct sys_info {    
    long          uptime; /* Seconds since boot */
    os_info_t     os_info;
    memory_info_t mem_info;
    cpu_info_t    cpu_info;
    disk_info_t   disk_info;
    net_info_t    net_info;
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

