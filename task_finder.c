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
void task_add_status_info(char *proc_dir, struct task_stats *stats, struct dirent *entry);

void task_log_stats(struct task_stats *stats);
void task_log_all_active_tasks(struct task_stats *stats);
void task_log_info(struct task_info info);

/**
 * @brief      Initalizes task_stats struct
 *
 * @return     	An initalized task_stat struct, used to track info for all processes in this OS
 */
struct task_stats *task_init_stats()
{
	struct task_stats *stats = calloc(1, sizeof(struct task_stats));
	stats->active_tasks_size = 0;
	stats->active_tasks_max_len = task_init_max_len;
	stats->active_tasks = calloc(stats->active_tasks_max_len, sizeof(struct task_info));

	return stats;
}

/**
 * @brief      Destroys task_stats struct, including freeing allocated memory and setting pointers to NULL
 *
 * @param      stats  	task_stats struct
 */
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

/**
 * @brief      Adds info from proc_dir/entry_name/status for ALL processes in OS. Here, "process" is any entry in proc that is a number.
 *
 * @param      proc_dir  	proc directory
 * @param      stats     	task_stats struct
 */
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

/**
 * @brief      Safe version of task_add_status_info(). If the entry is a process, adds info from proc_dir/entry_name/status to stats.
 *
 * @param      proc_dir  	proc directory
 * @param      stats     	task_stats struct
 * @param      entry     	entry in proc
 */							
void task_add_status_info_if_process(char *proc_dir, struct task_stats *stats, struct dirent *entry)
{
	// Case: not a process
	if (isdigit(entry->d_name[0]) == 0) {
		return;
	}
	// Case: is a process
	task_add_status_info(proc_dir, stats, entry);

}

/**
 * @brief      Unsafe version of task_add_status_info_if_process(). Use the other one if possible.
 *
 * @param      proc_dir  	proc directory
 * @param      stats     	task_stats struct
 * @param      entry     	entry in proc
 */
void task_add_status_info(char *proc_dir, struct task_stats *stats, struct dirent *entry)
{
	Task_Status_File_Info status_file_info = {0};

	if (task_read_status_and_store_into(proc_dir, entry->d_name, &status_file_info) == 0) {
		task_populate_task_stats_with(stats, &status_file_info);
	}
	task_destroy_status_file_info(&status_file_info);

}

/**
 * @brief      Convenience function, for debugging. Logs all the information inside task_stats struct.
 *
 * @param      stats  	task_stats struct
 */
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

/**
 * @brief      Convenience function, for debugging. Logs all active tasks in task_stats.
 *
 * @param      stats  	task_stats struct
 */	
void task_log_all_active_tasks(struct task_stats *stats)
{
	int len = stats->active_tasks_size;
	LOG("len is %d\n", len);
	for (int i = 0; i < len; i++) {
		task_log_info(stats->active_tasks[i]);
	}
}

/**
 * @brief      Convenience function, for debugging. Logs one active task in task_stats.
 *
 * @param[in]  info  	An active task in task_stats.
 */
void task_log_info(struct task_info info)
{
	LOG("INFO:\n"
		"\t->pid: %d\n"
		"\t->uid: %d\n"
		"\t->name: '%s'\n"
		"\t->state: '%s'\n",
		info.pid, info.uid, info.name != NULL ? info.name : "null", info.state != NULL ? info.state : "null");

}
