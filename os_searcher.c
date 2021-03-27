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
void search_file_for_memory_usage(const char *base, const char *extension, char *avail_buf, char *total_buf);
void search_file_for_task_stats(const char *base, const char *extension, Task_Status_File_Info *status_file_info);
bool contains_uid_str(char *lineread_buf, char *uid_str);
char *get_nth_token(int n, char **current_ptr, char **next_ptr, const char *delim);

/**
 * @brief      Searches the OS for hostname
 *
 * @param      proc_dir  proc directory
 *
 * @return     A string representing the hostname of the OS, or NULL on error
 * 
 * @note       This func allocates memory. Must free() later.
 */
char *search_for_hostname(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_hostname_path);
}

/**
 * @brief      Searches the OS for its kernel name
 *
 * @param      proc_dir  proc directory
 *
 * @return     A string representing the kernel name of the OS, or NULL on error.
 * 
 * @note       This func allocates memory. Must free() later.
 */
char *search_for_kernel(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_kernel_path);
}

/**
 * @brief      Searches the OS for its cpu model name
 *
 * @param      proc_dir  proc directory
 *
 * @return     A string representing the cpu model name of the OS, or NULL on error.
 * 
 * @note       This func allocates memory. Must free() later.
 */
char *search_for_cpu_model(char *proc_dir)
{
	return search_file_with_key(proc_dir, pfs_cpu_path, "model name");
}

/**
 * @brief      Searches the OS for how many cpu cores it has
 *
 * @param      proc_dir  proc directory
 *
 * @return     A copy of the line from proc_dir/cpuinfo about how many cpu cores this OS has
 * 
 * @note       This func allocates memory. Must free() later.
 */
char *search_for_cpu_cores(char *proc_dir)
{
	return search_file_with_key(proc_dir, pfs_cpu_path, "siblings");
}

/**
 * @brief      Searches the OS for how long it's been running
 *
 * @param      proc_dir  proc directory
 *
 * @return     A copy of the line from proc_dir/uptime representing how long this OS has been running, or NULl on error
 * 
 * @note       This func allocates memory. Must free() later.
 */
char *search_for_uptime(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_uptime_path);
}

/**
 * @brief      Searches the OS for its load average values.
 *
 * @param      proc_dir  proc directory
 *
 * @return     A copy of the line from proc_dir/loadavg representing its load average values.
 * 
 * @note       This func allocates memory. Must free() later.
 */
char *search_for_load_avg(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_load_avg_path);
}

/**
 * @brief      Given a UID as a string, searches for the associated username in /etc/passwd
 *
 * @param      uid_str  UID, as a string
 *
 * @return     the username associated with uid_str, or NULL on error
 */
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
 * @brief      Searches the OS for its cpu usage numbers
 *
 * @param      proc_dir  proc directory
 *
 * @return     A copy of the line from proc_dir/stat representing its cpu usage numbers.
 * 
 * @note       This func allocates memory. Must free() later.
 */
char *search_for_cpu_usage(char *proc_dir)
{
	return search_file_first_line(proc_dir, pfs_cpu_usage_path);
}

/**
 * @brief      Searches the OS for its memory usage numbers.
 *
 * @param      proc_dir       proc directory
 * @param      mem_avail_buf  A buffer to copy the line holding "MemAvailable" data
 * @param      mem_total_buf  A buffer to copy the line holding "MemTotal" data
 *
 * @return     0 on FAILURE, 1 on SUCCESS
 * 
 * @note       Decided to use two buffers instead of allocating memory, and decided to read both values at once,
 * 			   due to bug w/ search_file_for_memory_usage() and maybe lineread(). If I try to read the same file
 * 			   twice, it seems that the functions don't read from the top, and will go into the next mem_info file
 * 			   if necessary. This causes values from Test N + 1 to show up in Test N. Using two buffers solves this problem.
 */
int search_for_mem_info(char *proc_dir, char *mem_avail_buf, char *mem_total_buf)
{
	search_file_for_memory_usage(proc_dir, pfs_mem_usage_path, mem_avail_buf, mem_total_buf);

	return !strlen(mem_avail_buf) == 0 && !strlen(mem_total_buf) == 0;
}

/**
 * @brief      Searches proc for a given task entry_name, and stores info from proc/entry_name/status into status_file_info
 *
 * @param      proc_dir          proc directory
 * @param      entry_name        entry name
 * @param      status_file_info  Task_Status_File_Info struct
 */
void search_for_task_info(char *proc_dir, char *entry_name, Task_Status_File_Info *status_file_info)
{
	strcpy(status_file_info->pid_buf, entry_name); // pid value is already entry_name

	char extension[256] = {0};
	sprintf(extension, "%s/%s", entry_name, "/status");

	search_file_for_task_stats(proc_dir, extension, status_file_info);
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
 * 
 * @note       This func allocates memory. Must free() later.
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
 * @note This func allocates memory. Must free() later.
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
 * @brief      Searches the file base/extension for MemAvailable data and MemTotal data, and if it
 * 			   finds them copies them into their respective buffers.
 *	
 * @param[in]  base       base path
 * @param[in]  extension  extension path
 * @param      avail_buf  buffer for MemAvailable data
 * @param      total_buf  buffer for MemTotal data
 */
void search_file_for_memory_usage(const char *base, const char *extension, char *avail_buf, char *total_buf)
{
	int fd = open_path(base, extension);

	if (fd == -1) {
		LOGP("ERROR - OPEN_PATH() FAILED - RETURNING\n");
		return;
	}

	char lineread_buf[256] = {0};
	ssize_t read_size;

	while ( (read_size = lineread(fd, lineread_buf, 256)) > 0) {
		char *mem_avail = strstr(lineread_buf, "MemAvailable");
		char *mem_total = strstr(lineread_buf, "MemTotal");

		if (mem_avail != NULL) {
			LOG("FOUND KEY: '%s'\n", mem_avail);
			strcpy(avail_buf, mem_avail);
		}
		else if (mem_total != NULL) {
			LOG("FOUND KEY: '%s'\n", mem_total);
			strcpy(total_buf, mem_total);
		}

	}

	close(fd);
}

/**
 * @brief      Searches base/extension for process stats, adding info it finds to status_file_info
 *
 * @param[in]  base              base path
 * @param[in]  extension         extension path
 * @param      status_file_info  Task_Status_File_Info struct
 */
void search_file_for_task_stats(const char *base, const char *extension, Task_Status_File_Info *status_file_info)
{
	int fd = open_path(base, extension);

	if (fd == -1) {
		LOGP("ERROR - OPEN_PATH() FAILED - RETURNING\n");
		return;
	}

	char lineread_buf[256] = {0};
	ssize_t read_size;

	while ( (read_size = lineread(fd, lineread_buf, 256)) > 0) {
		char *uid_str = strstr(lineread_buf, "Uid");
		char *name_str = strstr(lineread_buf, "Name");
		char *state_str = strstr(lineread_buf, "State");

		if (uid_str != NULL) {
			strncpy(status_file_info->uid_buf, uid_str, 256);
		}
		else if (name_str != NULL) {
			strncpy(status_file_info->name_buf, name_str, 256);
		}
		else if (state_str != NULL) {
			strncpy(status_file_info->state_buf, state_str, 256);
		}
	}

	close(fd);
}

/**
 * @brief      Searches /etc/passwd for the string containing the given uid
 *
 * @param[in]  uid   uid
 *
 * @return     If /etc/passwd exists, and has a line containing the given uid (in the 3rd column), returns the owner of that line,
 * 				else returns NULL.
 * 				
 * @note       This func allocates memory. Must free later.
 * 
 */
char *search_passwd_for_uid(char *uid_str, int fd)
{
	char lineread_buf[256] = {0};

	while (lineread(fd, lineread_buf, 256) > 0) {
		// Case: found uid_str
		if (contains_uid_str(lineread_buf, uid_str)) {
			return strdup(lineread_buf);
		}
	}
	// Case: No uid_str in /etc/passwd
	return NULL;
}

/**
 * @brief      Tells whether or not lineread_buf contains a given uid_str
 *
 * @param      lineread_buf  The lineread buffer
 * @param[in]  fd            { parameter_description }
 * @param      uid_str       The uid string
 *
 * @return     { description_of_the_return_value }
 */
bool contains_uid_str(char *lineread_buf, char *uid_str)
{
	char *next = lineread_buf;
	char *current;

	current = get_nth_token(3, &current, &next, ":");
	return strcmp(current, uid_str) == 0;
}

/**
 * @brief      Gets the nth token from a tokenized string
 *
 * @param[in]  n            n, to get nth token
 * @param      current_ptr  pointer to current - used for next_token()
 * @param      next_ptr     pointer to next - used for next_token()
 * @param[in]  delim        delimiter
 *
 * @return     The nth token.
 * 
 * @note       Warning: This function will segfault if you try to go out of bounds, e.g. getting the 4th token when the string only has 3.
 * 			   You must know the number of tokens in advance before you can safely use this function.
 */
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
	char *null_load_avg = search_for_load_avg("iawegawgeawegwgegec");
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

/**
 * @brief      Tests search_for_cpu_usage()
 *
 * @param      proc_dir  proc directory
 * 
 * @note       Confirmed success for:
 * 				1. Invalid proc (Note: gives default value of 'cpu: 20 20 20 40 0 0...')
 * 				2. Real proc - gives actual cpu stats
 */
void test_search_for_cpu_usage(char *proc_dir)
{
	LOGP("TEST - INVALID PROC - SHOULD RETURN NULL\n");
	char* invalid_proc_usage = search_for_cpu_usage("invalid-proc");
	LOG("Invalid proc usage is : '%s'\n", invalid_proc_usage);

	LOGP("TEST - REAL PROC - SHOULD RETURN LINE\n");
	char* real_proc_usage = search_for_cpu_usage(proc_dir);
	LOG("Real proc usage is : '%s'\n", real_proc_usage);

}
