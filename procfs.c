#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "procfs.h"
#include "util.h"

// Function prototypes
void populateUptime(double time, struct uptime *timeRecord);

int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{
   return pfs_get_aspect(proc_dir, hostname_buf, buf_sz, "sys/kernel/hostname");
}

int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
    if (pfs_get_aspect(proc_dir, version_buf, buf_sz, "sys/kernel/osrelease") == -1) {
        return -1;
    }

    char* kernel_version = strsep(&version_buf, "-") + '\0';

    // Safe to use cpy because we know kernel_version has \0
    strcpy(version_buf, kernel_version);

    return 0;
}

int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
    int model_fd = open_path(proc_dir, "cpuinfo");

    // Case: Cannot find fd
    if (model_fd == -1) {
        return -1;
    }

    char* model_name = copy_cpu_info(model_fd, "model name", buf_sz);

    // Case: cpu_info not found
    if (model_name == NULL) {
        return -1;
    }
    // Case: cpu_info found
    strcpy(model_buf, model_name);
    return 0;
  
}

int pfs_cpu_units(char *proc_dir)
{
    int cpu_fd = open_path(proc_dir, "cpuinfo");

    // Case: Cannot find fd
    if (cpu_fd == -1) {
        return -1;
    }

    char* cpu_units = copy_cpu_info(cpu_fd, "siblings", 200);

    /* Wow problem was actually calling atoi twice for some reason:
     * Original code called log(atoi(cpu_units) * 2) and then returned
     * atoi(cpu_units) * 2, which gave correct result in log
     * but incorrect result in return val
     * I have no idea why */ 
    return cpu_units == NULL ? -1 : atoi(cpu_units) * 2;
    
}

double pfs_uptime(char *proc_dir)
{
    int uptime_fd = open_path(proc_dir, "uptime");

    if (uptime_fd == -1) {
        return -1;
    }

    char* line;
    
    
    if ( lineread(uptime_fd, line, 256) == -1) {
        return -1;
    }

    LOG("LINE IS:\t%s\n", line);

    char* uptime_string = strsep(&line, " ");
    if (uptime_string == NULL) {
        return -1;
    }


    LOG("UPTIME STRING IS:\t%s\n", uptime_string);

    return atof(uptime_string);
}

int pfs_format_uptime(double time, char *uptime_buf)
{

    // Format for this from https://dyclassroom.com/c/c-dynamic-memory-allocation-calloc-function
    Uptime* timeRecord = (Uptime *)calloc(1, sizeof(Uptime));
    
    // Note: no segfault so far

    populateUptime(36000000.00, timeRecord);

    
    LOG("SECONDS:\t%d\n:\t", timeRecord->seconds);
    LOG("MINUTES:\t%d\n:\t", timeRecord->minutes);
    LOG("HOURS:\t%d\n:\t", timeRecord->hours);
    LOG("DAYS:\t%d\n:\t", timeRecord->days);
    LOG("YEARS:\t%d\n:\t", timeRecord->years);

    
    
    return -1;
}

// Requires existing uptime so that I can calloc and free in the same func
// Makes debugging a lot easier in the long run - only have to check for calloc
// and free in one func @ a time
void populateUptime(double time, Uptime *timeRecord)
{

    timeRecord->seconds = (int)time;
    // Note: Int division rounds down
    timeRecord->minutes = timeRecord->seconds/60;
    timeRecord->seconds %= 60;

    timeRecord->hours = timeRecord->minutes / 60;
    timeRecord->minutes %= 60;

    timeRecord->days = timeRecord->hours/24;
    timeRecord->hours %= 24;
    
    timeRecord->years = timeRecord->days/365;
    timeRecord->days %= 365;





}

struct load_avg pfs_load_avg(char *proc_dir)
{
   struct load_avg lavg = { 0 };
   return lavg;
}

double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
    return 0.0;
}



struct mem_stats pfs_mem_usage(char *proc_dir)
{
    struct mem_stats mstats = { 0 };
    return mstats;
}

struct task_stats *pfs_create_tstats()
{
    return NULL;
}

void pfs_destroy_tstats(struct task_stats *tstats)
{

}

int pfs_tasks(char *proc_dir, struct task_stats *tstats)
{
    return -1;
}

