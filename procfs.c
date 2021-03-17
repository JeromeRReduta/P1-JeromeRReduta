#include "logger.h"
#include "hostname_reader.h"

#include "procfs.h"


// Todo: Create big buffer logPs for each func we're currently working on, and remove them once done
int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{
    LOGP("STARTING PFS_HOSTNAME()___________________________________________________\n");

    test_host_r_destroy_data();







    LOGP("FINISHED w/ PFS_HOSTNAME()___________________________________________________\n");
    return -1;
}

int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
    return -1;
}

int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
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

