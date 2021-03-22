#ifndef _OS_SEARCHER_H_
#define _OS_SEARCHER_H_

char *search_for_hostname(char *proc_dir, size_t buf_sz);
char *search_for_kernel(char *proc_dir, size_t buf_sz);

char *search_file_first_line(const char *proc_dir, const char *extension, size_t buf_sz);
char *search_file_with_key(const char* proc_dir, const char *extension, const char *key);

void test_search_file_first_line();
void test_search_file_with_key();
void test_search_for_hostname(char *proc_dir, size_t buf_sz);
void test_search_for_kernel(char *proc_dir, size_t buf_sz);

#endif