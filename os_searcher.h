#ifndef _OS_SEARCHER_H_
#define _OS_SEARCHER_H_

#include "task_finder.h"

char *search_for_hostname(char *proc_dir);
char *search_for_kernel(char *proc_dir);
char *search_for_cpu_model(char *proc_dir);
char *search_for_cpu_cores(char *proc_dir);
char *search_for_uptime(char *proc_dir);
char *search_for_load_avg(char *proc_dir);
char *search_for_uname(char *uid_str);
char *search_for_cpu_usage(char *proc_dir);
int search_for_mem_info(char *proc_dir, char *mem_avail_buf, char *mem_total_buf);
void search_for_task_info(char *proc_dir, char *entry_name, Task_Status_File_Info *status_file_info);

#endif