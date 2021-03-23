#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "logger.h"
#include "util.h"
#include "procfs.h"
#include "os_searcher.h"

/**
 * @file File whose sole responsibility is to search the OS for data and return what it finds
 * <br>
 * Prefix: search
 */

/** This file has no vars */

/** Func prototypes */
char *search_file_first_line(const char *base, const char *extension);
char *search_file_with_key(const char *base, const char *extension, const char *key);
char *search_passwd_for_uid(char *uid_str, int fd);
bool contains_uid_str(char *lineread_buf, int fd, char *uid_str);
char *get_nth_token(int n, char **current_ptr, char **next_ptr, const char *delim);





char *search_for_hostname(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_hostname_path);
}




char *search_for_kernel(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_kernel_path);
}

char *search_for_cpu_model(char *proc_dir)
{
	return search_file_with_key(proc_dir, pfs_cpu_path, "model name");
}

char *search_for_cpu_cores(char *proc_dir)
{
	return search_file_with_key(proc_dir, pfs_cpu_path, "siblings");
}

char *search_for_uptime(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_uptime_path);
}

char *search_for_load_avg(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_load_avg_path);
}

char *search_for_uname(char *uid_str)
{
	int fd = open_path("/etc", "passwd");
	char* uname =  uid_str != NULL && fd != -1 ? search_passwd_for_uid(uid_str, fd) : NULL;

	if (fd != -1) {
		close(fd);
	}
	return uname;
}

/**
 * @brief      Searches for the file proc_dir/extension and returns the first line as a string
 *
 * @param[in]		
 * @param[in]  extension  extension path
 *
 * @return     If the file proc_dir/extensions exists and is not empty, return first line. Else,
 * 				return NULL
 */

/**
 * @brief      Searches for the file in base/extension and returns the first line
 *
 * @param[in]  base       base path
 * @param[in]  extension  extension path
 *
 * @return     If the file base/extension exists and is not empty, return first line. Else, return NULL.
 */
char *search_file_first_line(const char *base, const char *extension)
{
	int fd = open_path(base, extension);

	LOG("PATH: '%s/%s'\n", base, extension);

	if (fd == -1) {
		LOGP("ERROR - OPEN_PATH() FAILED - RETURNING NULL\n");
		return NULL;
	}

	char lineread_buf[256] = {0};

	ssize_t read_size = lineread(fd, lineread_buf, 255); // buf_sz - 1 so last char in buf is \0

	close(fd);

	return read_size > 0 ? strdup(lineread_buf) : NULL;
}

/**
 * @brief      Searches the file base/extension, and returns the first line containing key, up to buf_sz bytes
 *
 * @param[in]  base   	  base path
 * @param[in]  extension  extension path
 * @param[in]  buf_sz     buffer size
 * @param[in]  key        search key
 *
 * @return     If base/extension exists and contains a line with key, returns that line. Otherwise, returns NULL
 * 
 * @note This func allocates memory - have to free later
 */
char *search_file_with_key(const char *base, const char *extension, const char *key)
{

	if (key == NULL) {
		LOGP("ERROR - NULL KEY - RETURNING NULL\n");
		return NULL;
	}
	int fd = open_path(base, extension);

	if (fd == -1) {
		LOGP("ERROR - OPEN_PATH() FAILED - RETURNING NULL\n");
		return NULL;
	}

	char lineread_buf[256] = {0};

	ssize_t read_size;

	while ( (read_size = lineread(fd, lineread_buf, 256)) > 0) {
		char* key_name = strstr(lineread_buf, key);

		if (key_name != NULL) {
			LOG("FOUND KEY: '%s'\n", key_name);
			close(fd);
			return strdup(lineread_buf);
		}
	}

	LOGP("CANNOT FIND KEY - RETURNING NULL\n");
	close(fd);
	return NULL;
}

/**
 * @brief      Searches /etc/passwd for the string containing the given uid
 *
 * @param[in]  uid   uid
 *
 * @return     If /etc/passwd exists, and has a line containing the given uid (in the 3rd column), returns the owner of that line,
 * 				else returns NULL.
 * 				
 * @note       This allocates memory. Have to free later.
 * 
 */
char *search_passwd_for_uid(char *uid_str, int fd)
{
	char lineread_buf[256] = {0};

	while (lineread(fd, lineread_buf, 256) > 0) {
		if (contains_uid_str(lineread_buf, fd, uid_str)) {
			return strdup(lineread_buf);
		}
	}

	return NULL;

}

bool contains_uid_str(char *lineread_buf, int fd, char *uid_str)
{
	char *next = lineread_buf;
	char *current;

	current = get_nth_token(3, &current, &next, ":");
	return strcmp(current, uid_str) == 0;
}

char *get_nth_token(int n, char **current_ptr, char **next_ptr, const char *delim)
{
	for (int i = 0; i < n; i++) {
		*current_ptr = next_token(next_ptr, delim);
	}
	return *current_ptr;
}

/**
 * @brief      Tests search_file_first_line()
 *
 * @param[in]  proc_dir  proc_dir, copied from procfs funcs
 * 
 * @note Confirmed success for: 1) invalid path (fails as it should), 2) hostname
 */
void test_search_file_first_line(const char *proc_dir)
{

	LOGP("INVALID TEST - should return NULL\n");
	char *test_invalid = search_file_first_line("invalid", "path");

	LOG("Should be NULL: '%s'\n", test_invalid);

	if (test_invalid != NULL) {
		free(test_invalid);
	}

	LOGP("HOSTNAME TEST - should return 'KevinM'\n");

	char* test_hostname = search_file_first_line(proc_dir, "sys/kernel/hostname");

	LOG("Hostname is: '%s'\n", test_hostname);

	if (test_hostname != NULL) {
		free(test_hostname);
	}


}




/**
 * @brief      Tests search_file_with_key()
 *
 * @param[in]  proc_dir   The proc dir
 * @param[in]  extension  The extension
 * @param[in]  buf_sz     The buffer size
 */
void test_search_file_with_key(const char *proc_dir, const char *extension, size_t buf_sz)
{

	LOG("STATS:\n"
		"\t->path: '%s/%s'\n"
		"\t->buf_sz: %lu\n", proc_dir, extension, buf_sz);

	LOGP("TEST - INVALID PATH - SHOULD RETURN NULL\n");
	char* test_invalid = search_file_with_key(NULL, NULL, "bubbaKey");

	LOG("value of string is '%s'\n", test_invalid);


	LOGP("TEST - NULL KEY - SHOULD RETURN NULL\n");
	char* test_null_key = search_file_with_key(proc_dir, extension, NULL);
	LOG("value of string is '%s'\n", test_null_key);

	LOGP("TEST - THIS OS'S PROC FILE - SHOULD RETURN 'AMD EPYC Processor (with IBPB)'\n");

	char* test_proc = search_file_with_key(proc_dir, extension, "model name");
	LOG("value of string is '%s'\n", test_proc);



}

/**
 * @brief      Tests search_for_hostname()
 *
 * @param      proc_dir  Proc dir, from procfs func
 * @param[in]  buf_sz    Buf_sz, from procfs func
 * 
 * @note       	Confirmed success
 */
void test_search_for_hostname(char *proc_dir)
{

	LOGP("GETTING HOSTNAME - SHOULD BE 'KevinM'\n");

	char* hostname = search_for_hostname(proc_dir);

	LOG("Hostname is '%s'\n", hostname);

	free(hostname);
}

/**
 * @brief      Tests search_for_kernel()
 *
 * @param      proc_dir  Proc dir, from procfs func
 * @param[in]  buf_sz    Proc dir, from procfs func
 */
void test_search_for_kernel(char *proc_dir)
{
	char *kernel_line = search_for_kernel(proc_dir);
	LOG("KERNEL SHOULD BE 5.10.11: '%s'\n", kernel_line);

}

void test_search_for_cpu_model(char *proc_dir)
{

	char *model_line = search_for_cpu_model(proc_dir);
	LOG("MODEL SHOULD BE AMD PROCESSOR... : '%s'\n", model_line);
	free(model_line);
}

/**
 * @brief      Tests search_for_cpu_cores
 *
 * @param      proc_dir  proc dir
 * @param[in]  buf_sz    buffer size
 * 
 * @note Confirmed success
 */
void test_search_for_cpu_cores(char *proc_dir)
{
	char *cores_line = search_for_cpu_cores(proc_dir);
	LOG("Should be non-null: '%s'\n", cores_line);
	free(cores_line);
}

/**
 * @brief      Tests search_for_uptime()
 *
 * @param      proc_dir  proc_directory
 */
void test_search_for_uptime(char *proc_dir)
{
	LOGP("TESTING INVALID PROC_DIR - SHOULD BE NULL\n");
	char *null_uptime = search_for_uptime("invalid");
	LOG("SHOULD BE NULL: '%s'\n", null_uptime);
	free(null_uptime);

	LOGP("TESTING ACTUAL PROC_DIR - SHOULD BE NON-NULL\n");
	char *uptime_line = search_for_uptime(proc_dir);
	LOG("Should be non-null: '%s'\n", uptime_line);
	free(uptime_line);
}

/**
 * @brief      Tests search_for_proc_dir()
 *
 * @param      proc_dir  proc directory
 * 
 * @note       Confirmed success for:
 * 				1. Invalid proc (Note: Gives default values of 78.10 40.40 40.40 2/86 5867)
 * 				2. Real proc - gives actual load_avg values
 */
void test_search_for_load_avg(char *proc_dir)
{
	LOGP("TESTING INVALID PROC_DIR - SHOULD BE NULL\n");
	char *null_load_avg = search_for_load_avg("invalid-proc");
	LOG("SHOULD BE NULL: '%s'\n", null_load_avg); // Note - apparently when doing any-non-existent-proc/loadavg, you get the following values: 78.10 40.40 40.40 2/86 5867 - this is fine
	free_string(&null_load_avg);

	LOGP("TESTING REAL PROC_DIR - SHOULD BE 3 DOUBLES\n");
	char *real_load_avg = search_for_load_avg(proc_dir);
	LOG("SHOULD BE 3 doubles: '%s'\n", real_load_avg);
	free_string(&real_load_avg);
}


/**
 * @brief      Tests search_passwd_for_uid()
 */
void test_search_passwd_for_uid()
{

	int fd = open_path("/etc", "passwd");
	LOGP("INVALID TEST - PROB SHOULD BE NULL\n");
	char* invalid_uid_line = search_passwd_for_uid("3", fd);
	LOG("SHOULD BE NULL: '%s'\n", invalid_uid_line);

	LOGP("VALID TEST - SHOULD BE 'ROOT'\n");
	char* root_uid_line = search_passwd_for_uid("0", fd);
	LOG("SHOULD BE 'ROOT': '%s'\n", root_uid_line);
}