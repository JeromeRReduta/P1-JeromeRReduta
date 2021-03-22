#include <stdlib.h>
#include "logger.h"
#include "os_static_data.h"
#include "os_searcher.h"
#include "string.h"
#include "util.h"

#include "procfs.h"

/* Constants */
const char* pfs_hostname_path = "sys/kernel/hostname";
const char* pfs_kernel_path = "sys/kernel/osrelease";
const char* pfs_cpu_path = "cpuinfo";

/* Func prototypes */
void pfs_destroy_line_and_token(char **line_ptr, char **token_ptr);


// Todo: Create big buffer logPs for each func we're currently working on, and remove them once done
int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{     
    
    char* hostname = search_for_hostname(proc_dir, buf_sz);
    int return_val = copy_if_present(hostname_buf, hostname);

    pfs_destroy_line_and_token(&hostname, NULL);
    return return_val;
}

int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
    char* kernel_line = search_for_kernel(proc_dir, buf_sz);
    char* kernel_token = extract_token_before(kernel_line, "-");
    int return_val = copy_if_present(version_buf, kernel_token);

    pfs_destroy_line_and_token(&kernel_line, &kernel_token);
    return return_val;
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


void pfs_destroy_line_and_token(char **line_ptr, char **token_ptr)
{
    free_string(line_ptr);
    free_string(token_ptr);
}