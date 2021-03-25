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
const int mem_kb_to_mb = 1048576;

/**
 * Struct for containing info from reading memory file once
 */
typedef struct {
	char mem_available_buf[256];
	char mem_total_buf[256];
	char* mem_available_token;
	char* mem_total_token;
} Mem_File_Info;

/* Func prototypes */
void mem_init(struct mem_stats *mstats, char *proc_dir);
int mem_read_file_and_store_into(char *proc_dir, Mem_File_Info *read_file_info);
void mem_populate_mem_stats_with(struct mem_stats *mstats, Mem_File_Info *read_file_info);
void destroy_mem_file_info(Mem_File_Info *read_file_info);
void mem_log_info(struct mem_stats *mstats);


void mem_init(struct mem_stats *mstats, char *proc_dir)
{
	Mem_File_Info read_file_info = {0};

	if (mem_read_file_and_store_into(proc_dir, &read_file_info) == 0) {
		mem_populate_mem_stats_with(mstats, &read_file_info);
	}
	destroy_mem_file_info(&read_file_info);
}

int mem_read_file_and_store_into(char *proc_dir, Mem_File_Info *read_file_info)
{
	search_for_mem_info(proc_dir, read_file_info->mem_available_buf, read_file_info->mem_total_buf);
	LOG("TOTAL AND AVAIL BUF: '%s'\t '%s'\n", read_file_info->mem_available_buf, read_file_info->mem_total_buf);

	// Case: search_for_mem_info doesn't work (i.e. one or more buffers have strlen 0)
	if (strlen(read_file_info->mem_available_buf) == 0 || strlen(read_file_info->mem_total_buf) == 0) {
		return -1;
	}
	// Case: search_for_mem_info did work
	read_file_info->mem_available_token = extract_token_after(read_file_info->mem_available_buf, ":");
	read_file_info->mem_total_token = extract_token_after(read_file_info->mem_total_buf, ":");

	return 0;
}

void mem_populate_mem_stats_with(struct mem_stats *mstats, Mem_File_Info *read_file_info)
{
	double total = atof(read_file_info->mem_total_token) / mem_kb_to_mb;
	double avail = atof(read_file_info->mem_available_token) / mem_kb_to_mb;

	mstats->total = total;
	mstats->used = total - avail;
}

void destroy_mem_file_info(Mem_File_Info *read_file_info)
{
	strcpy(read_file_info->mem_available_buf, "\0");
	strcpy(read_file_info->mem_total_buf, "\0");

	free_string( &(read_file_info->mem_available_token) );
	free_string( &(read_file_info->mem_total_token) );
}

void mem_log_info(struct mem_stats *mstats)
{
	LOG("STATS:\n"
		"\t->used: %f\n"
		"\t->total: %f\n",
		mstats->used, mstats->total);
}