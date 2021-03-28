#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "logger.h"
#include "util.h"
#include "procfs.h"
#include "string_extractor.h"

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

	current = extract_nth_token(3, &current, &next, ":");
	return strcmp(current, uid_str) == 0;
}
