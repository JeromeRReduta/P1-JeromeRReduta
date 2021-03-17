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

void test_host_r_init_data();
void test_host_r_destroy_data();
void test_host_r_safe_init_data();
void test_host_r_search();


#endif