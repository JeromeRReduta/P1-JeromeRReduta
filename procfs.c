#include "logger.h"
#include "procfs.h"
#include "util.h"
#include "lineread.h"

int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{
    int hostname_fd = open_path(proc_dir, "sys/kernel/hostname");

    ssize_t read_size = lineread(hostname_fd, hostname_buf, buf_sz);

    printf("HOSTNAME:\t%s", hostname_buf);

    int returnVal = read_size >= 0 ? 0 : -1;
    printf("RETURNING:\t%d", returnVal);

    // TODO: Do we need to return only 0 or -1, or can return read_size on success?
    return read_size >= 0 ? 0 : -1;
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

