/**
 * @file File whose sole responsibility is to hold and get static data for OS. os_searcher.c to populate
 * the data.
 * <br>
 * Static data is info about the OS that is (almost) guaranteed to stay the same during
 * ./inspector's run time.
 * <br>
 * Prefix: os_s
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "logger.h"
#include "util.h"
#include "os_searcher.h"
#include "os_static_data.h"

/**
 * Struct holding private data
 */
typedef struct {
	/** Proc directory string - should be dynamically allocated to avoid having garbage value string */
	char* proc_dir;

	/** Buffer size, = to buf_sz from procfs funcs */
	size_t buf_sz;

	/** hostname of this OS */
	char* hostname;

	/** name of kernel this OS uses */
	char* kernel;

	/** name of computer's cpu_model */
	char* cpu_model;

	/** # of cpu units this computer has */
	int cpu_units;
} OS_S_Private_Data;

static OS_S_Private_Data* os_s_locals;

/* Constants */
const char* os_s_hostname_path = "sys/kernel/hostname";
const char* os_s_kernel_path = "sys/kernel/osrelease";
const char* os_s_cpu_path = "cpuinfo";

// Note: Want to order funcs not by public/private, but in terms of funcs and their related sub-funcs

/** Func prototypes */
void os_s_safe_init_data(char *proc_dir, size_t buf_sz);
void os_s_init_data(char *proc_dir, size_t buf_sz);

void os_s_destroy_data();
void os_s_reset_struct_internals();
void os_s_reset_structs();

char *os_s_get_hostname();
char *os_s_get_kernel();
char *os_s_get_cpu_model();
char *os_s_get_cpu_units();

void os_s_log_info();



/**
 * @brief      Initalizes public and private structs, ONLY if these structs are not already initalized
 *
 * @param      proc_dir  The proc dir
 * @param[in]  buf_sz    The buffer size
 */
void os_s_safe_init_data(char *proc_dir, size_t buf_sz)
{
	// Case: Locals already init
	if (os_s_locals != NULL) {
		LOGP("WARNING - GLOBALS AND/OR LOCALS IS ALREADY INITALIZED - will not overwrite data\n");
		return;
	}
	// Case:Locals are both null (safe to initialize)
	LOGP("LOCALS ARE NULL - INITALIZING\n");
	os_s_init_data(proc_dir, buf_sz);
}

// TODO: Rewrite each func for os_static_data and test to make sure they still work

/**
 * @brief      Initializes private struct, including copying proc_dir and buf_sz values
 * 				from proc_fs to this file. This does not populate any other data.
 * @param      proc_dir  proc directory 
 * @param[in]  buf_sz    buffer size
 */
void os_s_init_data(char *proc_dir, size_t buf_sz)
{
	os_s_locals = calloc(1, sizeof(OS_S_Private_Data));

	if (os_s_locals == NULL) {
		LOGP("OUT OF MEM; RETURNING\n"); // destroy_data() is called every time display refreshes, so don't have to destroy here
		return;
	}
	
	os_s_locals->proc_dir = strdup(proc_dir);

	if (os_s_locals->proc_dir == NULL) {
		LOGP("OUT OF MEM; RETURNING\n");
		return;
	}

	os_s_locals->buf_sz = buf_sz;

}


// Note: Plan is to safe_init() data in procfs funcs and destroy data @ end of every display_refresh(), when
// the other destroy() funcs are called (near bottom of file?)

/**
 * @brief      Resets all data in public and private structs (not constants though), including freeing allocated memory
 */
void os_s_destroy_data()
{
	os_s_reset_struct_internals();
	os_s_reset_structs();
}

/**
 * @brief   Helper func for destroy_data()
			<br>Frees and nulls public and private data structs
 */
void os_s_reset_structs()
{
	if (os_s_locals == NULL) {
		LOGP("WARNING - LOCALS ALREADY NULL\n");
	}
	else {
		free(os_s_locals);
		os_s_locals = NULL;
	}
}


/**
 * @brief      Helper func for destroy_data()
 * 				<br>Frees dynamically allocated mem for struct's internal members

 */
void os_s_reset_struct_internals()
{
	LOG("IS LOCALS NULL RN?\n"
		"\t->locals: '%s'\n",
		os_s_locals == NULL ? "true" : "false");

	if (os_s_locals != NULL) {
		free_string( &(os_s_locals->proc_dir) );
		free_string( &(os_s_locals->hostname) );
		free_string( &(os_s_locals->kernel) );
		free_string( &(os_s_locals->cpu_model) );
	}


}




/**
 * @brief      Helper func, for logging all the public, private, and constant info in the file
 * 
 */
void os_s_log_info()
{
	if (os_s_locals != NULL) {

		LOG("\n"
			"LOCAL INFO: {\n"
			"\tproc_dir = '%s'\n"
			"\tbuf_sz = %lu\n"
			"\thostname = '%s'\n"
			"\tkernel = '%s'\n"
			"\tcpu_model = '%s'\n"
			"\tcpu_units = %d\n"
			"}\n",
			os_s_locals->proc_dir, os_s_locals->buf_sz, os_s_locals->hostname,
			os_s_locals->kernel, os_s_locals->cpu_model, os_s_locals->cpu_units);
	}
	else {
		LOG("LOCAL INFO PTR IS: '%s'\n", os_s_locals == NULL ? "NULL" : "NOT ACTUALLY NULL");
	}
	LOG("\nCONSTANTS:\n"
		"\t->os_s_hostname_path = '%s'\n"
		"\t->, os_s_kernel_path = '%s'\n"
		"\t-> os_s_cpu_path = '%s'\n",
		os_s_hostname_path, os_s_kernel_path, os_s_cpu_path);
}

