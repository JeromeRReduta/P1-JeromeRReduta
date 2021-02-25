#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "procfs.h"
#include "util.h"

// Function prototypes
void populate_uptime(double time, Uptime *time_record);
int write_time(Uptime *time_record, char *uptime_buf);
void get_time_substring(int time_in_units, char* append_text, char* time_buf, size_t time_sz);
int init_cpu_stats(char *proc_dir, struct cpu_stats *stats);
int init_mstats(int mem_fd, struct mem_stats *mstats);


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

    char model_name[256] = {0};

    copy_cpu_info(model_fd, "model name", model_name, buf_sz);

    // Case: cpu_info not found
    if (model_name == NULL) {
        return -1;
    }
    // Case: cpu_info found
    strcpy(model_buf, model_name);
    close(model_fd);
    return 0;
  
}

int pfs_cpu_units(char *proc_dir)
{
    int cpu_fd = open_path(proc_dir, "cpuinfo");

    // Case: Cannot find fd
    if (cpu_fd == -1) {
        return -1;
    }
    char cpu_units[256] = {0};

    copy_cpu_info(cpu_fd, "siblings", cpu_units, 200);


    /* Wow problem was actually calling atoi twice for some reason:
     * Original code called log(atoi(cpu_units) * 2) and then returned
     * atoi(cpu_units) * 2, which gave correct result in log
     * but incorrect result in return val
     * I have no idea why */ 
    return cpu_units == NULL ? -1 : atoi(cpu_units) * 2;
    
}

// Segfault occurred when running ./inspector due to this func - have to fix
double pfs_uptime(char *proc_dir)
{
    
    
    int uptime_fd = open_path(proc_dir, "uptime");
    LOG("UPTIME_FD GOOD:\t%d\n", uptime_fd);
    if (uptime_fd == -1) {
        return -1;
    }

    char line[256];
    char* line_ptr = line;
    
    LOG("ATTEMPTING TO DO LINEREAD%d\n", 0);
    if ( lineread(uptime_fd, line_ptr, 256) == -1) {
        return -1;
    }

    LOG("LINE IS:\t%s\n", line_ptr);

    // This feels dirty, but if I don't do this the code segfaults
    char** uptime_string_ptr = &line_ptr;

    char* uptime_string = strsep(uptime_string_ptr, " ");
    if (uptime_string == NULL) {
        return -1;
    }


    LOG("UPTIME STRING IS:\t%s\n", uptime_string);

    close(uptime_fd);

    return atof(uptime_string);
    
}

int pfs_format_uptime(double time, char *uptime_buf)
{
    // Format for this from https://dyclassroom.com/c/c-dynamic-memory-allocation-calloc-function
    Uptime* time_record = (Uptime *)calloc(1, sizeof(Uptime));
    populate_uptime(time, time_record);
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

    size_t time_sz = 64;

    char* years = malloc(time_sz);
    char* days = malloc(time_sz);
    char* hours = malloc(time_sz);
    char* minutes = malloc(time_sz);
    char* seconds = malloc(time_sz);

    get_time_substring(time_record->years, "years, ", years, time_sz);
    get_time_substring(time_record->days, "days, ", days, time_sz);
    get_time_substring(time_record->hours, "hours, ", hours, time_sz);


    snprintf(minutes, 64, "%d %s", time_record->minutes, "minutes, ");
    snprintf(seconds, 64, "%d %s", time_record->seconds, "seconds");

    LOG("SECONDS:\t%s", seconds);
    LOG("MINUTES:\t%s\n", minutes);

     LOG("CURRENT UPTIME:\t %s %s %s %s %s\n", years, days, hours, minutes, seconds);
    snprintf(uptime_buf, 256, "%s%s%s%s%s", years, days, hours, minutes, seconds);
    free(years);
    free(days);
    free(hours);
    free(minutes);
    free(seconds);

    LOG("'%s'", uptime_buf);
    

    return 0;

}

void get_time_substring(int time_in_units, char* append_text, char* time_buf, size_t time_sz)
{
    if (time_in_units > 0) {
        snprintf(time_buf, time_sz, "%d %s", time_in_units, append_text);
        LOG("TIME_BUF RIGHT NOW:\t'%s'\n", time_buf);
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
    
    char load_avg_txt[256];
    char* line = load_avg_txt;

    if (pfs_get_aspect(proc_dir, line, 256, "loadavg") == -1) {
        return lavg;
    }

    LOG("LINE CONTENTS:\t%s\n", line);

    lavg.one = atof(strsep(&line, " "));
    lavg.five = atof(strsep(&line, " "));
    lavg.fifteen = atof(strsep(&line, " "));

    return lavg;
}

double pfs_cpu_usage(char *proc_dir, struct cpu_stats *prev, struct cpu_stats *curr)
{
    init_cpu_stats(proc_dir, curr);

    LOG("AFTER READING FILE:\n\tprev->idle:\t%ld\n\tprev->total:\t%ld\n\tcurrent->idle:\t%ld\n\tcurrent->total:\t%ld\n",
        prev->idle, prev->total, curr->idle, curr->total);

    long idle = curr->idle - prev-> idle;
    long total = curr->total - prev-> total;

    // Case - error: comparing the same entries or would divide by 0
    if (curr->total == prev->total || idle < 0 || total < 0) {
        return 0.0;
    }

    LOG("\n\tIDLE:\t%ld\n\tTOTAL:\t%ld\n", idle, total);

    long double used = (long double)(idle)/total;

    LOG("USED:\t%Lf", used);
    //LOG("FINAL ANSWER:%f", (1 - idle/total)*100);
    return (double)(1 - used);
}

int init_cpu_stats(char *proc_dir, struct cpu_stats *stats)
{
    char buf[256];
    char* buf_ptr = buf;

    stats->idle = 0;
    stats->total = 0;

    if (pfs_get_aspect(proc_dir, buf_ptr, 256, "stat") == -1) {
        return -1;
    }

    LOG("LINE:\t%s\n", buf_ptr);

    char* sep_ptr;

    // Do this twice to get to first cpu number
    sep_ptr = strsep(&buf_ptr, " ");
    sep_ptr = strsep(&buf_ptr, " ");

    // 1st 3 #s
    for (int i = 0; i < 3; i++) {
        sep_ptr = strsep(&buf_ptr, " ");
        stats->total += atol(sep_ptr);

    }

    // 4th # - idle time
    sep_ptr = strsep(&buf_ptr, " ");
    stats->idle = atol(sep_ptr);
    stats->total += atol(sep_ptr);

    // Last 6 #s
    for (int i = 0; i < 6; i++) {
        sep_ptr = strsep(&buf_ptr, " ");
        stats->total += atol(sep_ptr);
    }

    LOG("VALUES:\n"
        "\tstats->idle:\t%ld\n"
        "\tstats->total:\t%ld\n",
        stats->idle, stats->total);



    

    // Because of earlier calloc, don't have to init values of stats

    return 0;

}



struct mem_stats pfs_mem_usage(char *proc_dir)
{
    struct mem_stats mstats = { 0 };

    int mem_fd = open_path(proc_dir, "meminfo");

    if (mem_fd == -1) {
        LOG("MEM_INFO = %d; RETURNING NULL", mem_fd);
        return mstats;
    }

    
    if (init_mstats(mem_fd, &mstats) == -1) {
        LOG("VALUE OF INIT_MSTATS IS %d\n", -1);
        return mstats;
    }
    
    close(mem_fd);
    return mstats;
}

// Initalizes mstats by going through page once and storing values; returns 0 on success, -1 if one or more value not filled in
int init_mstats(int mem_fd, struct mem_stats *mstats)
{
    char line[256] = {0};
    ssize_t read_sz;
    int kb_to_mb = 1048576;

    double mem_total = -1;
    double mem_avail = -1;
    // Is the formula mem_total - mem_avail - mem_free?
    // double mem_free = -1; Maybe not

    while ( (read_sz = lineread(mem_fd, line, 256)) > 0) {
        char* mem_avail_search = strstr(line, "MemAvailable") + '\0';
        char* mem_total_search = strstr(line,"MemTotal") + '\0';

        // Case: found key_name

        if (mem_avail_search != NULL) {
            mem_avail_search = strstr(mem_avail_search, ":") + 1;
            char* txt;

            mem_avail = strtod(mem_avail_search, &txt)/kb_to_mb;
            LOG("FOUND MEM_AVAIL:\t |%s|\n", mem_avail_search);
            // mem_avail[?] = '\0' Set ? to index of "k" in "kb"
        }

        if (mem_total_search != NULL) {
            mem_total_search = strstr(mem_total_search, ":") + 1;
            char* txt;

            mem_total = strtod(mem_total_search, &txt)/kb_to_mb;
            LOG("FOUND mem_total:\t |%s|\n", mem_total_search);
            // mem_total[?] = '\0' Set ? to index of "k" in "kb"
        }

      

    }

    LOG("VALUES:\n"
          "\tmem_avail:\t|%f|\n"
          "\tmem_total:\t|%f|\n",
          mem_avail, mem_total);
    
      // Case: key_name not found in file

      if (mem_avail == -1 || mem_total == -1) {
          LOG("ONE OR MORE VALUES NULL:\n"
          "\tmem_avail:\t%f\n"
          "\tmem_total:\t%f\n",
          mem_avail, mem_total);

          return -1;
      }

      mstats->total = mem_total;
      mstats->used = mem_total - mem_avail;
      return 0;




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
    int read_dir_return = read_proc(proc_dir);
    return -1;
}

