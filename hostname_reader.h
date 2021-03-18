/**
 * @file
 *
 * Header file for hostname_reader.c
 */

#ifndef _HOSTNAME_READER_H_
#define _HOSTNAME_READER_H_


/**
 * Struct holding public data
 */
typedef struct {

	/** hostname of this OS - should be dynamically allocated to avoid garbage value string */
	char* hostname;

} Host_R_Public_Data;

/** Global data struct ptr */
extern Host_R_Public_Data* host_r_globals;

/** Public functions */
void test_host_r_init_data();
void test_host_r_destroy_data();
void test_host_r_safe_init_data();
void test_host_r_search();

void host_r_safe_init_data(char *proc_dir, size_t buf_sz);
void host_r_destroy_data();


#endif