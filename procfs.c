#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "procfs.h"
#include "util.h"

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

    char* buffer;

    return strtod(uptime_string, buffer);
}

int pfs_format_uptime(double time, char *uptime_buf)
{
    return -1;
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

