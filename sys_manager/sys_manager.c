#include <sys/sysinfo.h>
#include <sensors/sensors.h>
#include <sensors/error.h>
#include <sys/statvfs.h>
#include <ctype.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <dirent.h>
#include "sys_manager.h"
//#include "chips.h"

#define PATH_PROC_CPUINFO "/proc/cpuinfo"
#define PATH_SYS_SYSTEM	  "/sys/devices/system"
#define PATH_SYS_BLOCK	  "/sys/block"


static void err_sys (const char* msg){perror(msg); exit(1);}
static void err_exit(const char* msg){perror(msg); exit(1);}



static sys_info_t info;

static FILE* sys_manager_cmdout (const char *cmd);
static FILE* sys_manager_getfile(const char *cmd);

static void sys_manager_cpuinfo (void);
static void sys_manager_osinfo  (void);
static void sys_manager_sysinfo (void);
static void sys_manager_diskinfo(void);

/* Lookup a pattern and get the value from cpuinfo.
 * Format is:
 *
 *	"<pattern>   : <key>"
 */
static int
lookup_cpuinfo(char *line, char *pattern, char *value, size_t size)
{
	char *p, *v;
	int len = strlen(pattern);
	if (!*line)
		return 0;
	/* pattern */
	if (strncmp(line, pattern, len))
		return 0;
	/* white spaces */
	for (p = line + len; isspace(*p); p++);
	/* separator */
	if (*p != ':')
		return 0;
	/* white spaces */
	for (++p; isspace(*p); p++);
	/* value */
	if (!*p)
		return 0;
	v = p;
	/* end of value */
	len = strlen(line) - 1;
	for (p = line + len; isspace(*(p-1)); p--);
	*p = '\0';
	strncpy(value, v, size);
	return 1;
}

static FILE* sys_manager_cmdout(const char *cmd) {
	FILE* fp;
	if((fp = popen(cmd, "r")) == NULL)
		err_sys("popen failed");
	
	return fp;
}



static void sys_manager_cpuinfo(void) {
	// parse cpuinfo
	FILE* fp;
	char buf [SYS_STRLEN_1];
	char freq[SYS_STRLEN_1];
	char path[FILENAME_MAX];
	
	fp = sys_manager_getfile(PATH_PROC_CPUINFO);
	
	while(fgets(buf, sizeof(buf), fp) != NULL) {
		     if (lookup_cpuinfo(buf, "vendor",     info.cpu_info.vendor, SYS_STRLEN_1)) ;
		else if (lookup_cpuinfo(buf, "vendor_id",  info.cpu_info.vendor, SYS_STRLEN_1)) ;
		else if (lookup_cpuinfo(buf, "model name", info.cpu_info.model,  SYS_STRLEN_1)) ;
		else if (lookup_cpuinfo(buf, "cpu MHz",    freq,                 SYS_STRLEN_1)) ;
		else
			continue;
	}
	
	if(fclose(fp) == -1)
		err_sys("fclose failed");
	
	// convert freq to double
	if(*freq)
		info.cpu_info.freq = strtod(freq, NULL);	
	
	// get number of cpus and freq max min 
	for(;;) {
		snprintf(path, FILENAME_MAX, PATH_SYS_SYSTEM "/cpu/cpu%d", info.cpu_info.cpus);
		if(access(path, F_OK) == 0) 
			info.cpu_info.cpus++;
		else
			break;
	}
		
	// get freq max min 
	int val;
	for(int i = 0; i < info.cpu_info.cpus; i++) {
		snprintf(path, FILENAME_MAX, PATH_SYS_SYSTEM "/cpu/cpu%d/cpufreq/cpuinfo_max_freq", i);
		if(access(path, F_OK) == -1)
			continue;
		fp = sys_manager_getfile(path);
		fscanf(fp, "%d", &val);		
		if(info.cpu_info.freq_max < (val / 1000))
			info.cpu_info.freq_max = (val / 1000);
		if(fclose(fp) == -1)
			err_sys("fclose failed");
		
		info.cpu_info.freq_min = 0;
		snprintf(path, FILENAME_MAX, PATH_SYS_SYSTEM "/cpu/cpu%d/cpufreq/cpuinfo_min_freq", i);
		if(access(path, F_OK) == -1)
			continue;
		fp = sys_manager_getfile(path);
		fscanf(fp, "%d", &val);		
		if(info.cpu_info.freq_min > (val / 1000) || info.cpu_info.freq_min == 0)
			info.cpu_info.freq_min = (val / 1000);
		if(fclose(fp) == -1)
			err_sys("fclose failed");
	}
	
	// get temperature
	sensors_init(NULL);
	const sensors_chip_name *chip;
	int chip_nr;

	chip_nr = 0;
	while ((chip = sensors_get_detected_chips(NULL, &chip_nr))) {
		
		if(chip->bus.type != SENSORS_BUS_TYPE_ISA) // only ISA chips needed
			continue;
		
		int a, b, err;
		const sensors_feature *feature;
		const sensors_subfeature *sub;
		//char *label;
		double val;

		a = 0;
		while ((feature = sensors_get_features(chip, &a))) {
			/*if (!(label = sensors_get_label(chip, feature))) {
				fprintf(stderr, "ERROR: Can't get label of feature "
					"%s!\n", feature->name);
				continue;
			}
			printf("%s:\n", label);  
			free(label);*/

			b = 0;
			while ((sub = sensors_get_all_subfeatures(chip, feature, &b))) {
				if (sub->flags & SENSORS_MODE_R) {
					if ((err = sensors_get_value(chip, sub->number, &val)))
						fprintf(stderr, "ERROR: Can't get "
							"value of subfeature %s: %s\n",
							sub->name,
							sensors_strerror(err));
					else {
						//printf("  %s: %.3f\n", sub->name, val);
						if(strstr(sub->name, "temp") && strstr(sub->name, "_input"))
							info.cpu_info.temperature      = val;
						if(strstr(sub->name, "temp") && strstr(sub->name, "_max"))
							info.cpu_info.temperature_max  = val;
					}
				}
			}
		}
		break; // only first chip!
	}
	sensors_cleanup();
}
static void sys_manager_diskinfo(void) {
	// get disks
	DIR *dp;
	struct dirent *de;
	char path[FILENAME_MAX];
	
	if((dp = opendir(PATH_SYS_BLOCK)) == NULL)
		err_sys("Could not list block interface directory");
	int i = 0;
	while((de = readdir(dp))) {
		disk_info_t *di = &info.disks.items[i];		
		strncpy(di->name, de->d_name, SYS_STRLEN_2 - 1);
		
		snprintf(path, FILENAME_MAX, "%s/%s/device/model", PATH_SYS_BLOCK, di->name );
		if(access(path, F_OK) == 0) {
			FILE* fp = sys_manager_getfile(path);
			char val[SYS_STRLEN_1];
			fscanf(fp, "%s", val);
			strncpy(di->model, val, SYS_STRLEN_1 - 1);
			
			if(fclose(fp) == -1)
				err_sys("fclose failed");
		}
		
		unsigned int sec_size = 0;
		snprintf(path, FILENAME_MAX, "%s/%s/queue/hw_sector_size", PATH_SYS_BLOCK, di->name );
		if(access(path, F_OK) == 0) {
			FILE* fp = sys_manager_getfile(path);
			unsigned int val;
			fscanf(fp, "%u", &val);
			sec_size = val;
			
			if(fclose(fp) == -1)
				err_sys("fclose failed");
		}
		
		unsigned int size = 0;
		snprintf(path, FILENAME_MAX, "%s/%s/size", PATH_SYS_BLOCK, di->name );
		if(access(path, F_OK) == 0) {
			FILE* fp = sys_manager_getfile(path);
			unsigned long val;
			fscanf(fp, "%lu", &val);
			size = val;
			
			if(fclose(fp) == -1)
				err_sys("fclose failed");
		}
		
		di->size = (size * sec_size);// / (1000 * 1000); // Gb
		printf("%llu %u %u\n\n", di->size, size, sec_size);
	}
	closedir(dp);
	
}
static FILE* 
sys_manager_getfile(const char *path) {
	FILE* fp;
	if((fp = fopen(path, "r")) == NULL)
		err_sys("fopen failed");
	
	return fp;
}

static void 
sys_manager_osinfo(void) {
	struct utsname uts = {0};
	if (uname(&uts) == -1)
		err_sys("uname failed");
	
	strncpy(info.os_info.sysname,  uts.sysname,  SYS_STRLEN_1 - 1);
	strncpy(info.os_info.machine,  uts.machine,  SYS_STRLEN_2 - 1);
	strncpy(info.os_info.release,  uts.release,  SYS_STRLEN_1 - 1);
	strncpy(info.os_info.version,  uts.version,  SYS_STRLEN_1 - 1);
	strncpy(info.os_info.nodename, uts.nodename, SYS_STRLEN_1 - 1);
	
	strncpy(info.cpu_info.arch,    uts.machine,  SYS_STRLEN_2 - 1);
}

static void 
sys_manager_sysinfo(void) {
    int err = 0;
    struct sysinfo s_info;

    if( (err = sysinfo(&s_info)) == -1)
            err_sys("Could not get sysinfo");

    info.uptime = s_info.uptime;
    memcpy(&info.cpu_info.loads, &s_info.loads, sizeof info.cpu_info.loads);
    info.mem_info.totalram  = s_info.totalram;
    info.mem_info.freeram   = s_info.freeram;
    info.mem_info.totalswap = s_info.totalswap;
    info.mem_info.freeswap  = s_info.freeswap;
}

int
sys_manager_init(void) {
	int err = 0;
	sys_manager_sysinfo ();
	sys_manager_osinfo  ();
	sys_manager_cpuinfo ();
	sys_manager_diskinfo();
	return err;
}

int
sys_manager_destroy(void) {
	int err = 0;
	return err;
}

void
sys_manager_dump_string(char *buf, size_t len) {
	snprintf(buf, len,
			"uptime:     %lu\n"
			"os:\n"
			   "\tsysname:   %s\n"
			   "\tversion:   %s\n"
			   "\trelesase:  %s\n"
			   "\tmachine:   %s\n"
			   "\tnodename:  %s\n"
			"cpu:\n"
			   "\tarch:         %s\n"
			   "\tvendor:       %s\n"
			   "\tmodel:        %s\n"
			   "\tcpus:         %d\n"			
			   "\tfreq MHz:     %.3f\n"
			   "\tfreq_max MHz: %.3f\n"
			   "\tfreq_min MHz: %.3f\n"
			   "\ttempereature: %.1f, %.1f\n"
			   "\tloads:        %lu, %lu, %lu\n"
			"memory:\n"
			   "\ttotalram:   %lu\n"
			   "\tfreeram:    %lu\n"
			   "\ttotalswap:  %lu\n"
			   "\tfreeswap:   %lu\n"
			"disk:\n"
			   "\tname:     %s\n"
			   "\tmodel:    %s\n"
			   "\tsize:     %llu\n"
			, 
			info.uptime,
			info.os_info.sysname, info.os_info.version, info.os_info.release, info.os_info.machine, info.os_info.nodename,
			info.cpu_info.arch,
			info.cpu_info.vendor,
			info.cpu_info.model,
			info.cpu_info.cpus,
			info.cpu_info.freq, info.cpu_info.freq_max, info.cpu_info.freq_min,
			info.cpu_info.temperature, info.cpu_info.temperature_max,
			info.cpu_info.loads[0], info.cpu_info.loads[1], info.cpu_info.loads[2],
			info.mem_info.totalram,
			info.mem_info.freeram,
			info.mem_info.totalswap,
			info.mem_info.freeswap,
			info.disks.items[0].name,
			info.disks.items[0].model,
			info.disks.items[0].size);
}

int
sys_manager_update(void) {
	int err = 0;
	return err;
}
