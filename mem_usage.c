#include <stdlib.h>
#include "logger.h"
#include "os_searcher.h"
#include "string.h"
#include "util.h"
#include "string_extractor.h"

#include "procfs.h"
#include "mem_usage.h"


/**
 * @file File whose sole responsibility is to track the OS's memory usage
 * 
 * Prefix: mem
 */

/** Conversion rate of kb to mb - this many kb = 1 mb */
const int mem_kb_to_mb = 1048576;

/**
 * Struct for containing info from reading memory file once
 */
typedef struct {

	/** Line holding MemAvailable data */
	char mem_available_buf[256];

	/** Line holding MemTotal data */
	char mem_total_buf[256];

	/** Token holding formatted MemAvailable data */
	char* mem_available_token;

	/** Token holding formatted MemTotal data */
	char* mem_total_token;
} Mem_File_Info;

/* Func prototypes */
void mem_init(struct mem_stats *mstats, char *proc_dir);
int mem_read_file_and_store_into(char *proc_dir, Mem_File_Info *read_file_info);
void mem_populate_mem_stats_with(struct mem_stats *mstats, Mem_File_Info *read_file_info);
void destroy_mem_file_info(Mem_File_Info *read_file_info);
void mem_log_stats(struct mem_stats *mstats);

/**
 * @brief      Initalizes mem_stats struct with info from proc_dir/meminfo
 *
 * @param      mstats    mem_stats
 * @param      proc_dir  proc directory
 */
void mem_init(struct mem_stats *mstats, char *proc_dir)
{
	Mem_File_Info read_file_info = {0};

	if (mem_read_file_and_store_into(proc_dir, &read_file_info) == 0) {
		mem_populate_mem_stats_with(mstats, &read_file_info);
	}
	destroy_mem_file_info(&read_file_info);
}

/**
 * @brief      Reads proc/meminfo and stores data into read_file_info
 *
 * @param      proc_dir        proc directory
 * @param      read_file_info  Mem_File_info struct
 *
 * @return     0 on success, -1 on error
 */
int mem_read_file_and_store_into(char *proc_dir, Mem_File_Info *read_file_info)
{
	// Case: search_for_mem_info didn't work
	if (search_for_mem_info(proc_dir, read_file_info->mem_available_buf, read_file_info->mem_total_buf) == 0) {
		return -1;
	}
	// Case: search_for_mem_info did work
	read_file_info->mem_available_token = extract_token_after(read_file_info->mem_available_buf, ":");
	read_file_info->mem_total_token = extract_token_after(read_file_info->mem_total_buf, ":");
	
	return 0;
}

/**
 * @brief      Populates mem_stats struct with info from read_file_info
 *
 * @param      mstats mem_stats struct         
 * @param      read_file_info  Mem_File_Info struct
 */
void mem_populate_mem_stats_with(struct mem_stats *mstats, Mem_File_Info *read_file_info)
{
	double total = atof(read_file_info->mem_total_token) / mem_kb_to_mb;
	double avail = atof(read_file_info->mem_available_token) / mem_kb_to_mb;

	mstats->total = total;
	mstats->used = total - avail;
}

/**
 * @brief      Frees all allocated memory in read_file_info and sets ptrs to NULL
 *
 * @param      read_file_info  Mem_File_Info struct
 */
void destroy_mem_file_info(Mem_File_Info *read_file_info)
{
	strcpy(read_file_info->mem_available_buf, "\0");
	strcpy(read_file_info->mem_total_buf, "\0");

	free_string( &(read_file_info->mem_available_token) );
	free_string( &(read_file_info->mem_total_token) );
}

/**
 * @brief      Convience function, for debugging. Logs all info in mem_stats
 *
 * @param      mstats  mem_stats struct
 */
void mem_log_stats(struct mem_stats *mstats)
{
	LOG("STATS:\n"
		"\t->used: %f\n"
		"\t->total: %f\n",
		mstats->used, mstats->total);
}
