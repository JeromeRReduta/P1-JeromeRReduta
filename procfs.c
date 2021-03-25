#include <stdlib.h>
#include "logger.h"
#include "os_searcher.h"
#include "string.h"
#include "util.h"
#include "string_extractor.h"
#include "uptime_formatter.h"
#include "cpu_usage.h"
#include "mem_usage.h"

#include "procfs.h"

/* Constants */
const char* pfs_hostname_path = "sys/kernel/hostname";
const char* pfs_kernel_path = "sys/kernel/osrelease";
const char* pfs_cpu_path = "cpuinfo";
const char* pfs_uptime_path = "uptime";
const char* pfs_load_avg_path = "loadavg";
const char* pfs_cpu_usage_path = "stat";
const char* pfs_mem_usage_path = "meminfo";

/* Func prototypes */

void pfs_init_load_avg_with(struct load_avg *lavg_ptr, const char *load_avg_line);
void pfs_log_lavg_info(struct load_avg *lavg);
void pfs_init_load_avg_with(struct load_avg *lavg_ptr, const char *load_avg_line);
void pfs_init_load_avg_values(struct load_avg *lavg_ptr, char **current_ptr, char **next_ptr);
void pfs_set_value(double *lavg_value, char **current_ptr, char **next_ptr);


void test_pfs_log_lavg_info();
void test_pfs_init_load_avg_with(char *proc_dir);


// Todo: Create big buffer logPs for each func we're currently working on, and remove them once done
int pfs_hostname(char *proc_dir, char *hostname_buf, size_t buf_sz)
{     
    char* hostname = search_for_hostname(proc_dir);
    int return_val = n_copy_if_present(hostname_buf, hostname, buf_sz);

    destroy_line_and_token(&hostname, NULL);
    return return_val;
}

int pfs_kernel_version(char *proc_dir, char *version_buf, size_t buf_sz)
{
    char* kernel_line = search_for_kernel(proc_dir);
    char* kernel_token = extract_token_before(kernel_line, "-");
    int return_val = n_copy_if_present(version_buf, kernel_token, buf_sz);

    destroy_line_and_token(&kernel_line, &kernel_token);
    return return_val;
}

int pfs_cpu_model(char *proc_dir, char *model_buf, size_t buf_sz)
{
    char* model_line = search_for_cpu_model(proc_dir);
    char* model_token = extract_token_after(model_line, ":");
    int return_val = n_copy_if_present(model_buf, model_token, buf_sz);

    destroy_line_and_token(&model_line, &model_token);
    return return_val;
}

int pfs_cpu_units(char *proc_dir)
{

    char* cores_line = search_for_cpu_cores(proc_dir);
    char* cores_token = extract_token_after(cores_line, ":");

    int return_val = cores_token != NULL ? atoi(cores_token) * 2 : -1;
    destroy_line_and_token(&cores_line, &cores_token);
    return return_val;
}

double pfs_uptime(char *proc_dir)
{
    char* uptime_line = search_for_uptime(proc_dir);
    double return_val = uptime_line != NULL ? atof(uptime_line) : -1.0;

    destroy_line_and_token(&uptime_line, NULL);
    return return_val;
}

int pfs_format_uptime(double time, char *uptime_buf)
{
    char* time_str = time_f_get_time_str(time, uptime_buf);
    int return_val = n_copy_if_present(uptime_buf, time_str, 256);
    free_string(&time_str);
    return return_val;
}

struct load_avg pfs_load_avg(char *proc_dir)
{
   struct load_avg lavg = { 0 };
   char* load_avg_line = search_for_load_avg(proc_dir);
   pfs_init_load_avg_with(&lavg, load_avg_line);

   destroy_line_and_token(&load_avg_line, NULL);
   return lavg;
}

void pfs_init_load_avg_with(struct load_avg *lavg_ptr, const char *load_avg_line)
{
    if (lavg_ptr == NULL || load_avg_line == NULL) {
        LOGP("ERROR - LOAD_AVG OR LOAD_AVG_LINE IS NULL PTR\n");
        return;
    }

    char load_avg_line_copy[256] = {0};
    strcpy(load_avg_line_copy, load_avg_line);

    char *next = load_avg_line_copy;
    char *current;
    pfs_init_load_avg_values(lavg_ptr, &current, &next);
}

/**
 * @brief      Equivalent of: <br>
 *             <pre>
 *                  if ( (current = next_token(...) != NULL)) {
 *                      lavg_ptr->one = current;
 *                  }
 *                  if ( (current = next_token(...) != NULL)) {
 *                      lavg_ptr->five = current;
 *                  }
 *                  if ( (current = next_token(...) != NULL)) {
 *                      lavg_ptr->fifteen = current;
 *                  }
 *             </pre>
 * 
 *
 * @param      lavg_ptr     pointer to struct load_avg
 * @param      current_ptr  pointer to current string, for next_token()
 * @param      next_ptr     pointer to next string, for next_token()
 */
void pfs_init_load_avg_values(struct load_avg *lavg_ptr, char **current_ptr, char **next_ptr)
{
    pfs_set_value(&(lavg_ptr->one), current_ptr, next_ptr);
    pfs_set_value(&(lavg_ptr->five), current_ptr, next_ptr);
    pfs_set_value(&(lavg_ptr->fifteen), current_ptr, next_ptr);
}

void pfs_set_value(double *lavg_value, char **current_ptr, char **next_ptr)
{
    if ( (*current_ptr = next_token(next_ptr, " ,?!")) != NULL) {
        *lavg_value = atof(*current_ptr);
    }
}

double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
    cpu_init(proc_dir, curr);
    return cpu_calc_usage(prev, curr);
}

struct mem_stats pfs_mem_usage(char *proc_dir)
{
    struct mem_stats mstats = { 0 };
    mem_init(&mstats, proc_dir);
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



/**
 * @brief      Tests pfs_init_load_avg
 *
 * @param[in]  proc_dir  proc directory
 */
void test_pfs_init_load_avg_with(char *proc_dir)
{
    
    LOGP("TEST - INVALID PROC_DIR - SHOULD HAVE ALL DEFAULT VALUES\n");

    struct load_avg invalid_proc_lavg = {0};
    char* invalid_proc_line = search_for_load_avg("invalid-proc");
    pfs_init_load_avg_with(&invalid_proc_lavg, invalid_proc_line);

    pfs_log_lavg_info(&invalid_proc_lavg);


    LOGP("TEST - NULL LOAD_AVG PTR - SHOULD GIVE ERROR MESSAGE\n");

    char* null_test_line = search_for_load_avg(proc_dir);
    pfs_init_load_avg_with(NULL, null_test_line);

    LOGP("TEST - ACTUAL LOAD_AVG - SHOULD BE NON-NULL\n");

    struct load_avg real_lavg = {0};
    char* real_test_line = search_for_load_avg(proc_dir);

    pfs_init_load_avg_with(&real_lavg, real_test_line);

    pfs_log_lavg_info(&real_lavg);



}

void pfs_log_lavg_info(struct load_avg* lavg)
{
    if (lavg == NULL) {
        LOGP("LOAD_AVG PTR IS NULL\n");
        return;
    }

    LOG("LOAD_AVG STATS: \n"
        "\t->one: %f\n"
        "\t->five: %f\n"
        "\t->fifteen: %f\n", lavg->one, lavg->five, lavg->fifteen);


}

/**
 * @brief      Tests pfs_log_lavg_info()
 * 
 * @note       Confirmed success
 */
void test_pfs_log_lavg_info()
{

    LOGP("TEST - NULL PTR\n");
    pfs_log_lavg_info(NULL);

    LOGP("TEST - INITALIZED BUT ALL 0\n");
    struct load_avg all_zero = {0};
    pfs_log_lavg_info(&all_zero);

    LOGP("TEST - INITALIZED W/ ANY VALUE\n");

    struct load_avg init = {1, 2, 3};
    pfs_log_lavg_info(&init);
}