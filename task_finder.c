#include <stdlib.h>
#include "logger.h"
#include "os_searcher.h"
#include "string.h"
#include "util.h"
#include "string_extractor.h"
#include <dirent.h>
#include <ctype.h>

#include "procfs.h"

#include "task_finder.h"


/**
 * @file File whose sole responsibility is to find and hold a list of the OS's tasks (currently running processes)
 * 
 * Prefix: task
 */

/** Constants */
const int task_init_max_len = 2;

/* Func prototypes */
struct task_stats *task_init_stats();
void task_destroy_stats(struct task_stats **stats);
void task_get_tasks_from_proc(char *proc_dir, struct task_stats *stats);
void task_add_status_info_if_process(char *proc_dir, struct task_stats *stats, struct dirent *entry);

void task_log_stats(struct task_stats *stats);
void task_log_all_active_tasks(struct task_stats *stats);
void task_log_info(struct task_info info);

struct task_stats *task_init_stats()
{
	struct task_stats *stats = calloc(1, sizeof(struct task_stats));
	stats->active_tasks_size = 0;
	stats->active_tasks_max_len = task_init_max_len;
	stats->active_tasks = calloc(stats->active_tasks_max_len, sizeof(struct task_info));

	return stats;
}

void task_destroy_stats(struct task_stats **stats)
{
	if (stats == NULL || *stats == NULL) {
		LOGP("ERROR - NULL TASK STATS PTR - RETURNING\n");
		return;
	}

	if ((*stats)->active_tasks != NULL) {
		free((*stats)->active_tasks);
	}

	free(*stats);
	*stats = NULL;
}

void task_get_tasks_from_proc(char *proc_dir, struct task_stats *stats)
{
	DIR *directory;
	if ( (directory = opendir(proc_dir)) == NULL ) {
		perror("opendir");
		return;
	}

	struct dirent* entry;
	while ( (entry = readdir(directory)) != NULL) {
		task_add_status_info_if_process(proc_dir, stats, entry);
	}
	
	closedir(directory);
}

void task_add_status_info_if_process(char *proc_dir, struct task_stats *stats, struct dirent *entry)
{
	if (isdigit(entry->d_name[0]) != 0) {
		Task_Status_File_Info status_file_info = {0};

		if (task_read_status_and_store_into(proc_dir, entry->d_name, &status_file_info) == 0) {
			task_populate_task_stats_with(stats, &status_file_info);
		}
		task_destroy_status_file_info(&status_file_info);
	}
}

void task_log_stats(struct task_stats *stats)
{
	if (stats == NULL) {
		LOGP("ERROR - STATS IS NULL - RETURNING\n");
		return;
	}

	LOG("STATS:\n"
		"\t->total: %u\n"
		"\t->running: %u\n"
		"\t->waiting: %u\n"
		"\t->sleeping: %u\n"
		"\t->stopped: %u\n"
		"\t->zombie: %u\n"
		"\t->active_tasks_size: %u\n"
		"\t->active_tasks_max_len: %u\n",
		stats->total, stats->running, stats->waiting,
		stats->sleeping, stats->stopped, stats->zombie,
		stats->active_tasks_size, stats->active_tasks_max_len);

	if (stats->active_tasks == NULL) {
		LOGP("active tasks is NULL, returning\n");
	}
	else {
		LOGP("ACTIVE TASKS EXIST\n");
		task_log_all_active_tasks(stats);
	}

}

void task_log_all_active_tasks(struct task_stats *stats)
{
	int len = stats->active_tasks_size;
	LOG("len is %d\n", len);
	for (int i = 0; i < len; i++) {
		task_log_info(stats->active_tasks[i]);
	}
}

void task_log_info(struct task_info info)
{
	LOG("INFO:\n"
		"\t->pid: %d\n"
		"\t->uid: %d\n"
		"\t->name: '%s'\n"
		"\t->state: '%s'\n",
		info.pid, info.uid, info.name != NULL ? info.name : "null", info.state != NULL ? info.state : "null");

}


/**
 * @brief      Tests task_log_stats()
 * 
 * @note       Confirmed success for:
 *				1. Null tstats ptr (error msg)
 *				2. Good tstats ptr, null active_tasks (prints tstats info but gives error msg on active_tasks)
 *				3. Good tstats ptr, good active_tasks (prints info for both, ASSUMING size is accurate/increases when a task is added)
 *				4. Good tstats ptr, good active_tasks, adding more tasks than max_len should allow (should make sure
 *					that we realloc as necessary for task_info, especially since it doesn't show as a warning)
*/
void test_task_log_stats()
{
	LOGP("TEST - NULL TSTATS POINTER - SHOULD GIVE ERROR MSG\n");
	struct task_stats* null_stats = NULL;
	task_log_stats(null_stats);

	LOGP("TEST - TSTATS IS GOOD BUT TASK INFO IS NULL - SHOULD LOG TSTATS AND GIVE ERROR MSG\n");
	struct task_stats* good_stats_null_info = calloc(1, sizeof(struct task_stats));
	good_stats_null_info->zombie = 1;
	good_stats_null_info->stopped = 2;
	good_stats_null_info->sleeping = 3;
	good_stats_null_info->waiting = 4;
	good_stats_null_info->running = 5;
	good_stats_null_info->total = 1 + 2 + 3 + 4 + 5;
	good_stats_null_info->active_tasks_size = 0;
	good_stats_null_info->active_tasks_max_len = 2;

	task_log_stats(good_stats_null_info);
			
	LOGP("TEST - TSTATS AND TASK INFO ARE GOOD - SHOULD LOG BOTH\n");

	struct task_stats* all_good_tasks = calloc(1, sizeof(struct task_stats));
	all_good_tasks->active_tasks = calloc(2, sizeof(struct task_info));

	all_good_tasks->zombie = 1;
	all_good_tasks->stopped = 2;
	all_good_tasks->sleeping = 3;
	all_good_tasks->waiting = 4;
	all_good_tasks->running = 5;
	all_good_tasks->total = 1 + 2 + 3 + 4 + 5;
	all_good_tasks->active_tasks_size = 0;
	all_good_tasks->active_tasks_max_len = 2;

	task_init_info(all_good_tasks->active_tasks, "14", "15", "Yes", "also yes");
	all_good_tasks->active_tasks_size++;

	task_init_info(all_good_tasks->active_tasks + 1, "24", "25", "No", "also no");
	all_good_tasks->active_tasks_size++;

	// Case: Adding more tasks than max_len would allow
	/*
	all_good_tasks->active_tasks[2].pid = 24;
	all_good_tasks->active_tasks[2].uid = 25;
	strncpy(all_good_tasks->active_tasks[2].name, "No", 26);
	strncpy(all_good_tasks->active_tasks[2].state, "Also no", 13);
	all_good_tasks->active_tasks_size++;
	*/
	task_log_stats(all_good_tasks);

}

/**
 * @brief      Tests task_init_stats()
 * 
 * @note       Confirmed success
 */
void test_task_init_stats()
{
	struct task_stats *tstats = task_init_stats();

	//task_init_info(tstats->active_tasks, 14, 15, "Yes", "also yes");
	tstats->active_tasks_size++;

	//task_init_info(tstats->active_tasks + 1, 24, 25, "No", "also no");
	tstats->active_tasks_size++;

	task_log_stats(tstats);
}

/**
 * @brief      Tests task_destroy_stats()
 * 
 * @note       Confirmed success for:
 * 				1. non-null stats and info - frees both, sets stats to NULL
 * 				2. null stats - error message
 * 				3. non-null stats, null info - frees stats, sets stats to NULL
 */
void test_task_destroy_stats()
{

	LOGP("INITALIZING\n");
	struct task_stats* tstats = task_init_stats();
	struct task_info* info = tstats->active_tasks;
	task_log_stats(tstats);
	task_log_info(info[0]);

	LOGP("AFTER DESTROYING\n");
	task_destroy_stats(&tstats);

	task_log_stats(tstats);
	LOG("IS INFO PTR NULL? '%s'\n", info == NULL ? "true" : "false");
	task_log_info(info[0]);

	LOGP("TESTING NULL DESTROY - SHOULD GIVE ERROR MSG\n");
	task_destroy_stats(NULL);

	LOGP("TESTING DESTROY ON GOOD TASK STATS, NULL INFO - SHOULD GIVE ERROR MSG BUT FREE TASK_STATS\n");
	struct task_stats* null_info = task_init_stats();
	free(null_info->active_tasks);
	null_info->active_tasks = NULL;
	task_destroy_stats(&null_info);
	task_log_stats(null_info);

}
