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
char *search_file_first_line(const char *proc_dir, const char *extension, size_t buf_sz);
char *search_file_with_key(const char *proc_dir, const char *extension, size_t buf_sz, const char *key);



char *search_for_hostname(char *proc_dir, size_t buf_sz)
{
	return search_file_first_line(proc_dir, pfs_hostname_path, buf_sz);
}




char *search_for_kernel(char *proc_dir, size_t buf_sz)
{
	return search_file_first_line(proc_dir, pfs_kernel_path, buf_sz);
}

char *search_for_cpu_model(char *proc_dir, size_t buf_sz)
{
	return search_file_with_key(proc_dir, pfs_cpu_path, buf_sz, "model name");
}

char *search_for_cpu_cores(char *proc_dir, size_t buf_sz)
{
	return search_file_with_key(proc_dir, pfs_cpu_path, buf_sz, "siblings");
}

/**
 * @brief      Searches for the file proc_dir/extension and returns the first line as a string
 *
 * @param[in]  proc_dir   proc directory
 * @param[in]  extension  extension path
 *
 * @return     If the file proc_dir/extensions exists and is not empty, return first line. Else,
 * 				return NULL
 */
char *search_file_first_line(const char *proc_dir, const char *extension, size_t buf_sz)
{
	int fd = open_path(proc_dir, extension);

	if (fd == -1) {
		LOGP("ERROR - OPEN_PATH() FAILED - RETURNING NULL\n");
		return NULL;
	}

	char lineread_buf[256] = {0};

	ssize_t read_size = lineread(fd, lineread_buf, buf_sz - 1); // buf_sz - 1 so last char in buf is \0

	close(fd);

	return read_size > 0 ? strdup(lineread_buf) : NULL;

}

/**
 * @brief      Searches the file proc_dir/extension, and returns the first line containing key, up to buf_sz bytes
 *
 * @param[in]  proc_dir   proc directory
 * @param[in]  extension  extension path
 * @param[in]  buf_sz     buffer size
 * @param[in]  key        search key
 *
 * @return     If proc_dir/extension exists and contains a line with key, returns that line. Otherwise, returns NULL
 * 
 * @note This func allocates memory - have to free later
 */
char *search_file_with_key(const char *proc_dir, const char *extension, size_t buf_sz, const char *key)
{

	if (key == NULL) {
		LOGP("ERROR - NULL KEY - RETURNING NULL\n");
		return NULL;
	}
	int fd = open_path(proc_dir, extension);

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
			return strdup(key_name);
		}
	}

	LOGP("CANNOT FIND KEY - RETURNING NULL\n");
	close(fd);
	return NULL;
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
	char *test_invalid = search_file_first_line("invalid", "path", 256);

	LOG("Should be NULL: '%s'\n", test_invalid);

	if (test_invalid != NULL) {
		free(test_invalid);
	}

	LOGP("HOSTNAME TEST - should return 'KevinM'\n");

	char* test_hostname = search_file_first_line(proc_dir, "sys/kernel/hostname", 256);

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
	char* test_invalid = search_file_with_key(NULL, NULL, buf_sz, "bubbaKey");

	LOG("value of string is '%s'\n", test_invalid);


	LOGP("TEST - NULL KEY - SHOULD RETURN NULL\n");
	char* test_null_key = search_file_with_key(proc_dir, extension, buf_sz, NULL);
	LOG("value of string is '%s'\n", test_null_key);

	LOGP("TEST - THIS OS'S PROC FILE - SHOULD RETURN 'AMD EPYC Processor (with IBPB)'\n");

	char* test_proc = search_file_with_key(proc_dir, extension, buf_sz, "model name");
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
void test_search_for_hostname(char *proc_dir, size_t buf_sz)
{

	LOGP("GETTING HOSTNAME - SHOULD BE 'KevinM'\n");

	char* hostname = search_for_hostname(proc_dir, buf_sz);

	LOG("Hostname is '%s'\n", hostname);

	free(hostname);
}

/**
 * @brief      Tests search_for_kernel()
 *
 * @param      proc_dir  Proc dir, from procfs func
 * @param[in]  buf_sz    Proc dir, from procfs func
 */
void test_search_for_kernel(char *proc_dir, size_t buf_sz)
{

	char *kernel_line = search_for_kernel(proc_dir, buf_sz);
	LOG("KERNEL SHOULD BE 5.10.11: '%s'\n", kernel_line);

}



void test_search_for_cpu_model(char *proc_dir, size_t buf_sz)
{

	char *model_line = search_for_cpu_model(proc_dir, buf_sz);
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
void test_search_for_cpu_cores(char *proc_dir, size_t buf_sz)
{

	char *cores_line = search_for_cpu_cores(proc_dir, buf_sz);
	LOG("Should be non-null: '%s'\n", cores_line);
	free(cores_line);
}