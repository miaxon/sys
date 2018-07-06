#include <sys/sysinfo.h>
#include <sensors/sensors.h>
#include <sensors/error.h>
#include <ctype.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "sys_manager.h"
//#include "chips.h"

#define PATH_PROC_CPUINFO "/proc/cpuinfo"
#define PATH_SYS_SYSTEM	  "/sys/devices/system"
#define PATH_SYS_HWMON	  "/sys/class/hwmon"


static void err_sys (const char* msg){perror(msg); exit(1);}
static void err_exit(const char* msg){perror(msg); exit(1);}



static sys_info_t info;

static FILE* cmd_out (const char *cmd);
static FILE* get_file(const char *cmd);

static void get_cpuinfo(void);
static void get_osinfo (void);
static void get_sysinfo(void);

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

static FILE* cmd_out(const char *cmd) {
	FILE* fp;
	if((fp = popen(cmd, "r")) == NULL)
		err_sys("popen failed");
	
	return fp;
}



static void get_cpuinfo(void) {
	// parse cpuinfo
	FILE* fp;
	char buf [SYS_STRLEN_1];
	char freq[SYS_STRLEN_1];
	char path[FILENAME_MAX];
	
	fp = get_file(PATH_PROC_CPUINFO);
	
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
		fp = get_file(path);
		fscanf(fp, "%d", &val);		
		if(info.cpu_info.freq_max < (val / 1000))
			info.cpu_info.freq_max = (val / 1000);
		if(fclose(fp) == -1)
			err_sys("fclose failed");
		
		info.cpu_info.freq_min = 0;
		snprintf(path, FILENAME_MAX, PATH_SYS_SYSTEM "/cpu/cpu%d/cpufreq/cpuinfo_min_freq", i);
		fp = get_file(path);
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
}
static FILE* 
get_file(const char *path) {
	FILE* fp;
	if((fp = fopen(path, "r")) == NULL)
		err_sys("fopen failed");
	
	return fp;
}

static void 
get_osinfo(void) {
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
get_sysinfo(void) {
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
	get_sysinfo();
	get_osinfo();
	get_cpuinfo();
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
			info.mem_info.freeswap);
}

int
sys_manager_update(void) {
	int err = 0;
	return err;
}
