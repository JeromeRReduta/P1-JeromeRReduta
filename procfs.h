/**
 * @file
 *
 * Retrieves raw information from procfs, parses it, and provides easy-to-use
 * functions for displaying the data.
 */

#ifndef _PROCFS_H_
#define _PROCFS_H_

#include <sys/types.h>

/* Constants */
extern const char* pfs_hostname_path;
extern const char* pfs_kernel_path;
extern const char* pfs_cpu_path;
extern const char* pfs_uptime_path;
extern const char* pfs_load_avg_path;
extern const char* pfs_cpu_usage_path;
extern const char* pfs_mem_usage_path;

/**
 * @brief      Struct holding load average info
 */
struct load_avg {

    /** load average over one second */
    double one;

    /** load average over five seconds */
    double five;

    /** load average over fifteen seconds */
    double fifteen;
};

/**
 * @brief      Struct holding cpu_stats info
 */
struct cpu_stats {

    /** measure of idle cpu usage */
    long idle;

    /** measure of total cpu usage */
    long total;
};

/**
 * @brief      Struct holding memory usage info
 */
struct mem_stats {

    /** Memory used */
    double used;

    /** Memory total */
    double total;
};

/**
 * @brief      Struct holding task stats info
 */
struct task_stats {

    /** Number of tasks in this OS */
    unsigned int total;

    /** Number of running tasks */
    unsigned int running;

    /** Number of waiting tasks */
    unsigned int waiting;

    /** Number of sleeping tasks */
    unsigned int sleeping;

    /** Number of stopped tasks */
    unsigned int stopped;

    /** Number of zombie tasks */
    unsigned int zombie;

    /** Current size of active_tasks */
    unsigned int active_tasks_size;

    /** Max size of active_tasks */
    unsigned int active_tasks_max_len;

    /** Array of task_info structs - how many active_tasks are in OS */
    struct task_info *active_tasks;
};

/**
 * @brief      Struct holding task info
 */
struct task_info {

    /** Pid */
    pid_t pid;

    /** Uid */
    uid_t uid;

    /** Name */
    char name[26];

    /** State */
    char state[13];
};

int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz);
int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz);
int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz);
int pfs_cpu_units(char *proc_dir);
double pfs_uptime(char *proc_dir);
int pfs_format_uptime(double time, char *uptime_buf);

struct load_avg pfs_load_avg(char *proc_dir);
/* Note: 'prev' is an input that represents the *last* CPU usage state. 'curr'
 * is an *output* that represents the current CPU usage state, and will be
 * passed back in to pfs_load_avg on its next call. */
double pfs_cpu_usage(
        char *procfs_dir, struct cpu_stats *prev, struct cpu_stats *curr);
struct mem_stats pfs_mem_usage(char *procfs_dir);

/* Note: these two functions create and destroy task_stats structs. Depending on
 * your implementation, this might just be a malloc/free. */
struct task_stats *pfs_create_tstats();
void pfs_destroy_tstats(struct task_stats *tstats);

int pfs_tasks(char *proc_dir, struct task_stats *tstats);

#endif
