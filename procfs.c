#include <stdlib.h>
#include "logger.h"
#include "os_searcher.h"
#include "string.h"
#include "util.h"
#include "string_extractor.h"

#include "procfs.h"

/* Constants */
const char* pfs_hostname_path = "sys/kernel/hostname";
const char* pfs_kernel_path = "sys/kernel/osrelease";
const char* pfs_cpu_path = "cpuinfo";
const char* pfs_uptime_path = "uptime";

/* Func prototypes */
void pfs_destroy_line_and_token(char **line_ptr, char **token_ptr);


// Todo: Create big buffer logPs for each func we're currently working on, and remove them once done
int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{     
    
    char* hostname = search_for_hostname(proc_dir, buf_sz);
    int return_val = n_copy_if_present(hostname_buf, hostname, buf_sz);

    pfs_destroy_line_and_token(&hostname, NULL);
    return return_val;
}

int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
    char* kernel_line = search_for_kernel(proc_dir, buf_sz);
    char* kernel_token = extract_token_before(kernel_line, "-");
    int return_val = n_copy_if_present(version_buf, kernel_token, buf_sz);

    pfs_destroy_line_and_token(&kernel_line, &kernel_token);
    return return_val;
}

int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
    char* model_line = search_for_cpu_model(proc_dir, buf_sz);
    char* model_token = extract_token_after(model_line, ":");
    int return_val = n_copy_if_present(model_buf, model_token, buf_sz);

    pfs_destroy_line_and_token(&model_line, &model_token);
    return return_val;
}

int pfs_cpu_units(char *proc_dir)
{

    char* cores_line = search_for_cpu_cores(proc_dir, 256);
    char* cores_token = extract_token_after(cores_line, ":");

    int return_val = cores_token != NULL ? atoi(cores_token) * 2 : -1;
    pfs_destroy_line_and_token(&cores_line, NULL);
    return return_val;
}

double pfs_uptime(char *proc_dir)
{
    char* uptime_line = search_for_uptime(proc_dir);
    double return_val = uptime_line != NULL ? atof(uptime_line) : -1.0;

    pfs_destroy_line_and_token(&uptime_line, NULL);
    return return_val;
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