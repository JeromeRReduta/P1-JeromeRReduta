/**
 * @file
 *
 * File for holding and returning a string representation of this OS's hostname.
 * <br>
 * File prefix is "host_r"
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "logger.h"
#include "hostname_reader.h"


/**
 * Struct holding private data
 */
typedef struct {

	/** Proc directory string - should be dynamically allocated to avoid having garbage value string */
	char* proc_dir;

	/** Buffer size, = to buf_sz from procfs funcs */
	size_t buf_sz;

} Host_R_Private_Data;


/**
 * Ptrs to public and private data
 */
Host_R_Public_Data* host_r_globals;
static Host_R_Private_Data* host_r_locals;


/**
 * Define constants below
 */
const char* host_r_path = "sys/kernel/hostname";

/**
 * Function prototypes
 */
void host_r_init_data(char *proc_dir, size_t buf_sz);
void log_info();
void host_r_reset_struct_internals();
void host_r_reset_structs();



/**
 * @brief      Initializes public and private structs, including copying proc_dir and buf_sz values
 * 				from proc_fs to this file. This includes finding the hostname.
 *
 * @param      proc_dir  The proc dir
 * @param[in]  buf_sz    The buffer size
 */
void host_r_init_data(char *proc_dir, size_t buf_sz)
{

	host_r_globals = calloc(1, sizeof(Host_R_Public_Data));
	host_r_locals = calloc(1, sizeof(Host_R_Private_Data));

	if (host_r_globals == NULL || host_r_locals == NULL) {
		LOGP("OUT OF MEM; RETURNING\n"); // destroy_data() is called every time display refreshes, so don't have to destroy here
		return;
	}

	host_r_globals->hostname = strdup("PLACEHOLDER - CHANGE w/ FIND_HOSTNAME() LATER"); // find_hostname() will deal w/ strdup error here
	host_r_locals->proc_dir = strdup(proc_dir);

	if (host_r_locals->proc_dir == NULL) {
		LOGP("OUT OF MEM; RETURNING\n");
		return;
	}

	host_r_locals->buf_sz = buf_sz;
}

// Note: Plan is to safe_init() data in procfs funcs and destroy data @ end of every display_refresh(), when
// the other destroy() funcs are called (near bottom of file?)

/**
 * @brief      Resets all data in public and private structs (not constants though), including freeing allocated memory
 */
void host_r_destroy_data()
{
	host_r_reset_struct_internals();
	host_r_reset_structs();
}

/**
 * @brief      Helper func for destroy_data()
 * 				<br>Frees dynamically allocated mem for struct's internal members

 */
void host_r_reset_struct_internals()
{

	LOG("ARE EITHER PTRS NULL RN?\n"
		"\t->globals: '%s'\n"
		"\t->locals: '%s'\n", host_r_globals == NULL ? "true" : "false",
		host_r_locals == NULL ? "true" : "false");


	if (host_r_globals != NULL && host_r_globals->hostname != NULL) {
		LOG("host_r_globals->hostname = '%s', now freeing\n", host_r_globals->hostname);

		free(host_r_globals->hostname);
	}

	if (host_r_locals != NULL && host_r_locals->proc_dir != NULL) {
		LOG("host_r_locals->proc_dir = '%s', now freeing\n", host_r_locals->proc_dir);

		free(host_r_locals->proc_dir);
	}

}

/**
 * @brief   Helper func for destroy_data()
			<br>Frees and nulls public and private data structs
 */
void host_r_reset_structs()
{
	if (host_r_globals == NULL) {
		LOGP("WARNING - GLOBALS ALREADY NULL\n");
	}
	else {
		free(host_r_globals);
		host_r_globals = NULL;
	}

	if (host_r_locals == NULL) {
		LOGP("WARNING - GLOBALS ALREADY NULL\n");
	}
	else {
		free(host_r_locals);
		host_r_locals = NULL;
	}
}




/**
 * @brief      Tests host_r_init_data()
 * 
 * @note Confirmed success
 */
void test_host_r_init_data()
{

	log_info(); // Both ptrs should be NULL

	host_r_init_data("/proc_but_not", 1024);

	log_info(); // Each ptr should have given info


}

/**
 * @brief      Tests host_r_destroy_data()
 * 
 * @note Confirmed success for: 
 * 		1. Initializing data, then destroying it
 * 		2. Not initalizing data, then destroying it
 * 		3. Initalizing data, destroying it, and then trying to destroy it again (testing safety checks)
 */
void test_host_r_destroy_data()
{

	LOGP("BEFORE INITALIZING:\n");
	log_info();

	host_r_init_data("fake-proc", 256);

	LOGP("AFTER INITALIZING:\n");
	log_info();

	host_r_destroy_data();

	LOGP("AFTER FREEING:\n");
	log_info();

	LOGP("RUNNING DESTROY AGAIN ON PURPOSE, TO SEE IF SAFETY CHECKS STOP ME\n");
	host_r_destroy_data();
	LOGP("SO FAR SO GOOD\n");
	log_info();



}


/**
 * @brief      Helper func, for logging all the public, private, and constant info in the file
 * 
 */
void log_info()
{

	if (host_r_globals != NULL) {

		LOG("\n"
		"GLOBAL INFO: {\n"
		"\thostname = '%s'\n"
		"}\n", host_r_globals->hostname);
	}
	else {
		LOG("GLOBAL INFO PTR IS NULL: %d\n", host_r_globals == NULL);
	}


	if (host_r_locals != NULL) {

		LOG("\n"
			"LOCAL INFO: {\n"
			"\tproc_dir = '%s'\n"
			"\tbuf_sz = %lu\n"
			"}\n",
			host_r_locals->proc_dir, host_r_locals->buf_sz);
	}
	else {
		LOG("LOCAL INFO PTR IS NULL: %d\n", host_r_locals == NULL);
	}
	LOG("\nCONSTANTS:\n"
		"\t->host_r_path = '%s'\n", host_r_path);
}



