/**
 * @file
 *
 * Header file for os_static_data.c
 */

#ifndef _OS_STATIC_DATA_H_
#define _OS_STATIC_DATA_H_

/** Public functions */
void test_os_s_init_data();
void test_os_s_destroy_data();
void test_os_s_safe_init_data();

void test_os_s_get_kernel(char *proc_dir, size_t buf_sz);

void test_os_s_get_hostname(char *proc_dir, size_t buf_sz);


void os_s_safe_init_data(char *proc_dir, size_t buf_sz);
char *os_s_get_hostname(char *proc_dir, size_t buf_sz);
char *os_s_get_kernel(char *proc_dir, size_t buf_sz);
void os_s_destroy_data();

char *extract_token_before(char *line, char *delim);




#endif