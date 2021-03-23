#ifndef _OS_SEARCHER_H_
#define _OS_SEARCHER_H_

char *search_for_hostname(char *proc_dir);
char *search_for_kernel(char *proc_dir);
char *search_for_cpu_model(char *proc_dir);
char *search_for_cpu_cores(char *proc_dir);
char *search_for_uptime(char *proc_dir);
char *search_for_load_avg(char *proc_dir);
char *search_for_uname(char *uid_str);

char *search_file_first_line(const char *proc_dir, const char *extension);
char *search_file_with_key(const char *proc_dir, const char *extension, const char *key);
void test_search_file_first_line();
void test_search_file_with_key();
void test_search_for_hostname(char *proc_dir);
void test_search_for_kernel(char *proc_dir);
void test_search_for_cpu_cores(char *proc_dir);
void test_search_for_uptime(char *proc_dir);
void test_search_for_load_avg(char *proc_dir);
void test_search_for_uid();
void test_search_passwd_for_uid();
#endif