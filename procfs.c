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
    if (model_fd == 1) {
        return -1;
    }

    /* I was told to add this comment in lab:
     * Max model name size for model buf is 128; we're
     * adding a bit more buffer space
     * 
     * Also forgot I can't instantiate arrays w/ variable length -
     * would have to define global var at top, but would move var
     * too far away from this func. I'll just hard-code 256 instead. */
    char line[256] = {0};
    ssize_t read_sz;

    while ( (read_sz = lineread(model_fd, line, 256)) > 0) {
        char* model_name = strstr(line, "model name");

        printf("CURRENT model_name:\t%s", model_name);

        // Case: found model_name
        if (model_name != NULL) {
            LOG("FOUND IT:\t%s", model_name);
            printf("FOUND IT:\t%s", model_name);
        }

        
    }

    // Case: Did not find model info
    return -1;
}

int pfs_cpu_units(char *proc_dir)
{
    return 0;
}

double pfs_uptime(char *proc_dir)
{
    return 0.0;
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

