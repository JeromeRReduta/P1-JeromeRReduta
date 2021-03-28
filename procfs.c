#include <stdlib.h>
#include "logger.h"
#include "os_searcher.h"
#include "string.h"
#include "util.h"
#include "string_extractor.h"
#include "uptime_formatter.h"
#include "cpu_usage.h"
#include "mem_usage.h"
#include "task_finder.h"
#include "load_avg.h"

#include "procfs.h"

/* Constants */
const char* pfs_hostname_path = "sys/kernel/hostname";
const char* pfs_kernel_path = "sys/kernel/osrelease";
const char* pfs_cpu_path = "cpuinfo";
const char* pfs_uptime_path = "uptime";
const char* pfs_load_avg_path = "loadavg";
const char* pfs_cpu_usage_path = "stat";
const char* pfs_mem_usage_path = "meminfo";


/**
 * @brief      Copies this OS's hostname to hostname_buf
 *
 * @param      proc_dir      proc directory
 * @param      hostname_buf  buffer
 * @param[in]  buf_sz        buffer size
 *
 * @return     0 on success, -1 on error
 */
int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{     
    char* hostname = search_for_hostname(proc_dir);
    int return_val = n_copy_if_present(hostname_buf, hostname, buf_sz);

    destroy_line_and_token(&hostname, NULL);
    return return_val;
}

/**
 * @brief      Copies this OS's kernel version to version_buf
 *
 * @param      proc_dir     proc directory
 * @param      version_buf  buffer
 * @param[in]  buf_sz       buffer size
 *
 * @return     0 on success, -1 on error
 */
int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
    char* kernel_line = search_for_kernel(proc_dir);
    char* kernel_token = extract_token_before(kernel_line, "-");
    int return_val = n_copy_if_present(version_buf, kernel_token, buf_sz);

    destroy_line_and_token(&kernel_line, &kernel_token);
    return return_val;
}

/**
 * @brief      Copies this OS's cpu model to model_buf
 *
 * @param      proc_dir   proc directory
 * @param      model_buf  buffer
 * @param[in]  buf_sz     buffer size
 *
 * @return     0 on success, -1 on error
 */
int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
    char* model_line = search_for_cpu_model(proc_dir);
    char* model_token = extract_token_after(model_line, ":");
    int return_val = n_copy_if_present(model_buf, model_token, buf_sz);

    destroy_line_and_token(&model_line, &model_token);
    return return_val;
}

/**
 * @brief      Gets the num of cpu_units
 *
 * @param      proc_dir  proc directory
 *
 * @return     number of cpu_units, or -1 on error
 */
int pfs_cpu_units(char *proc_dir)
{
    char* cores_line = search_for_cpu_cores(proc_dir);
    char* cores_token = extract_token_after(cores_line, ":");

    int return_val = cores_token != NULL ? atoi(cores_token) * 2 : -1;
    destroy_line_and_token(&cores_line, &cores_token);
    return return_val;
}

/**
 * @brief      Gets the uptime of this OS
 *
 * @param      proc_dir  proc directory
 *
 * @return     uptime of OS
 */
double pfs_uptime(char *proc_dir)
{
    char* uptime_line = search_for_uptime(proc_dir);
    double return_val = uptime_line != NULL ? atof(uptime_line) : -1.0;

    destroy_line_and_token(&uptime_line, NULL);
    return return_val;
}

/**
 * @brief      Formats the OS's uptime and writes it to uptime_buf
 *
 * @param[in]  time        uptime
 * @param      uptime_buf  uptime buffer, to write formatted uptime into
 *
 * @return     0 on success, -1 on error
 */
int pfs_format_uptime(double time, char *uptime_buf)
{
    char* time_str = time_f_get_time_str(time);
    int return_val = n_copy_if_present(uptime_buf, time_str, 256);
    free_string(&time_str);
    return return_val;
}

/**
 * @brief      Creates a struct containing load average values and returns it
 *
 * @param      proc_dir  proc directory
 *
 * @return     load_avg struct holding load average values
 */
struct load_avg pfs_load_avg(char *proc_dir)
{
   struct load_avg lavg = { 0 };
   char* load_avg_line = search_for_load_avg(proc_dir);
   load_avg_init_with(&lavg, load_avg_line);

   destroy_line_and_token(&load_avg_line, NULL);
   return lavg;
}



/**
 * @brief      calculates this OS's cpu usage from cpu_usage file
 *
 * @param      proc_dir  proc directory
 * @param      prev      previous cpu_stats struct, from the last time this function is called
 * @param      curr      new cpu_stats struct, populated when this function is called
 *
 * @return     this OS's cpu usage, or 0 on error
 */
double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
    cpu_init(proc_dir, curr);
    return cpu_calc_usage(prev, curr);
}

/**
 * @brief      Populates and returns a mem_stats struct with data from meminfo
 *
 * @param      proc_dir  proc directory
 *
 * @return     a populated mem_stats struct with data from meminfo, or NULL on error
 */
struct mem_stats pfs_mem_usage(char *proc_dir)
{
    struct mem_stats mstats = { 0 };
    mem_init(&mstats, proc_dir);
    return mstats;
}

/**
 * @brief      Initalizes a task_stats struct
 *
 * @return     a task_stats struct, used to keep track of all processes in the OS
 */
struct task_stats *pfs_create_tstats()
{
    return task_init_stats();
}

/**
 * @brief      Destroys task_stats data, including freeing allocated memory
 *
 * @param      tstats  task_stats struct
 */
void pfs_destroy_tstats(struct task_stats *tstats)
{
    task_destroy_stats(&tstats);
}

/**
 * @brief      Returns the number of tasks/processes in this OS
 *
 * @param      proc_dir  proc directory
 * @param      tstats    task_stats struct
 *
 * @return     0, for success
 */
int pfs_tasks(char *proc_dir, struct task_stats *tstats)
{
    task_get_tasks_from_proc(proc_dir, tstats);
    return 0;
}
