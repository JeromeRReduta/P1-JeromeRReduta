#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "procfs.h"
#include "util.h"

// Function prototypes
void populate_uptime(double time, Uptime *time_record);
int write_time(Uptime *time_record, char *uptime_buf);
void get_time_substring(int time_in_units, char* append_text, char* time_buf);


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
    Uptime* time_record = (Uptime *)calloc(1, sizeof(Uptime));
    populate_uptime(36000000, time_record);
    // Note: no segfault so far

    return write_time(time_record, uptime_buf);
}

// Requires existing uptime so that I can calloc and free in the same func
// Makes debugging a lot easier in the long run - only have to check for calloc
// and free in one func @ a time
void populate_uptime(double time, Uptime *time_record)
{

    time_record->seconds = (int)time;
    // Note: Int division rounds down
    time_record->minutes = time_record->seconds/60;
    time_record->seconds %= 60;

    time_record->hours = time_record->minutes / 60;
    time_record->minutes %= 60;

    time_record->days = time_record->hours/24;
    time_record->hours %= 24;
    
    time_record->years = time_record->days/365;
    time_record->days %= 365;

    //Note: Currently, populate_uptime works as intended
    LOG("SECONDS:\t%d\n:\t", time_record->seconds);
    LOG("MINUTES:\t%d\n:\t", time_record->minutes);
    LOG("HOURS:\t%d\n:\t", time_record->hours);
    LOG("DAYS:\t%d\n:\t", time_record->days);
    LOG("YEARS:\t%d\n:\t", time_record->years);



}

int write_time(Uptime *time_record, char *uptime_buf)
{
    if (time_record == NULL) {
        return -1;
    }

    char time_string[256] = {0};
    char years[64], days[64], hours[64], minutes[64], seconds[64];

    get_time_substring(time_record->years, "years,", years);
    get_time_substring(time_record->days, "days,", days);
    get_time_substring(time_record->hours, "hours,", hours);
    snprintf(minutes, strlen(minutes), "%d %s", time_record->minutes, "minutes, ");
    snprintf(seconds, strlen(seconds), "%d %s", time_record->seconds, "seconds");

    LOG("SECONDS:\t%s", seconds);
    LOG("MINUTES:\t%s\n", minutes);

    LOG("CURRENT UPTIME:\t %s %s %s %s %s\n", years, days, hours, minutes, seconds);


    return -1;

}

void get_time_substring(int time_in_units, char* append_text, char* time_buf)
{
    if (time_in_units > 0) {
        snprintf(time_buf, strlen(time_buf), "%d %s", time_in_units, append_text);
    } 
    else {
        /* Learned from https://cboard.cprogramming.com/c-programming/112370-setting-char-*-null.html that I can null a char* by 
        setting char[0] to '\0'*/
        time_buf[0] = '\0';
    }
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

