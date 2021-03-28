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
#include "task_adder.h"

/**
 * @file File whose sole responsibility is to add tasks to the task list in task_finder.c.
 * Essentially a file full of helper functions (a helper file?)
 * 
 * Prefix: task (same as task_finder.c)
 */

/** Func prototypes */
void task_init_info(struct task_info *current_task, Task_Status_File_Info *status_file_info);
int task_read_status_and_store_into(char *proc_dir, char *entry_name, Task_Status_File_Info *status_file_info);
int task_init_status_file_info_tokens(Task_Status_File_Info *status_file_info);
void task_populate_task_stats_with(struct task_stats *stats, Task_Status_File_Info *status_file_info);
int grow_active_tasks(struct task_stats *stats);
void task_add_to_stats_count(struct task_stats *stats, char state);
void task_add_active_task_to(struct task_stats *stats, Task_Status_File_Info *status_file_info);
void task_process_and_copy_state(struct task_info* current_task, char *state_token);

void task_destroy_status_file_info(Task_Status_File_Info *status_file_info);
void task_log_status_file_info(Task_Status_File_Info *status_file_info);

/**
 * @brief      Initalizes task_info struct with info from status_file_info
 *
 * @param      current_task      	task_info struct
 * @param      status_file_info  	Task_Status_File_Info struct
 */			
void task_init_info(struct task_info *current_task, Task_Status_File_Info *status_file_info)
{
	current_task->pid = atoi(status_file_info->pid_token);
	current_task->uid = atoi(status_file_info->uid_token);
	task_process_and_copy_state(current_task, status_file_info->state_token);
	strncpy(current_task->name, status_file_info->name_token, 25);
}

/**
 * @brief      Reads info from proc_dir/entry_name/status and populates status_file_info with the info
 *
 * @param      proc_dir          proc directory
 * @param      entry_name        entry name
 * @param      status_file_info  Task_Status_File_Info struct
 *
 * @return     0 if both tasks succeed, otherwise -1
 */
int task_read_status_and_store_into(char *proc_dir, char *entry_name, Task_Status_File_Info *status_file_info)
{
	search_for_task_info(proc_dir, entry_name, status_file_info);
	return task_init_status_file_info_tokens(status_file_info);
}

/**
 * @brief      Populates the status_file_info tokens with status_file_info bufs
 *
 * @param      status_file_info  Task_Status_File_Info struct
 *
 * @return     0 on success; -1 if one of the bufs is empty, which would mean we couldn't find all the required task info
 */
int task_init_status_file_info_tokens(Task_Status_File_Info *status_file_info)
{
	// Case: 1 of bufs is empty, which would mean we couldn't find all the required task info
	if (strlen(status_file_info->pid_buf) == 0 || strlen(status_file_info->uid_buf) == 0 ||
		strlen(status_file_info->name_buf) == 0 || strlen(status_file_info->state_buf) == 0) {
		return -1;
	}

	status_file_info->pid_token = strdup(status_file_info->pid_buf);
	status_file_info->uid_token = extract_token_after(status_file_info->uid_buf, "\t");
	status_file_info->state_token = extract_token_after(status_file_info->state_buf, "\t");
	status_file_info->name_token = extract_token_after(status_file_info->name_buf, "\t");
	return 0;
}

/**
 * @brief      Given info about one task from status_file info, adds that one task to stats
 *
 * @param      stats             task_stats struct
 * @param      status_file_info  Task_Status_File_Info struct
 */
void task_populate_task_stats_with(struct task_stats *stats, Task_Status_File_Info *status_file_info)
{
	// Case: active tasks array is full -> attempt to grow array -> attempt fails - return
	if (stats->active_tasks_size == stats->active_tasks_max_len && grow_active_tasks(stats) == -1) {
		return;
	}

	char state = status_file_info->state_token[0];
	task_add_to_stats_count(stats, state);

	// Case: Task is not sleeping ('I' counts as sleeping in this project) - add task info to active tasks
	if (state != 'S' && state != 'I') {
		task_add_active_task_to(stats, status_file_info);
	}
}

/**
 * @brief      Double the size of active_tasks array. This function should only be called when the active_tasks array is full.
 *
 * @param      stats  	task_stats_struct
 *
 * @return     0 on success; -1 on error
 */
int grow_active_tasks(struct task_stats *stats)
{
	stats->active_tasks_max_len *= 2;
	struct task_info *new_tasks = realloc(stats->active_tasks, stats->active_tasks_max_len * sizeof(struct task_info));

	// Case: new_tasks is NULL - out of memory - free stats->active_tasks and return -1
	if (new_tasks == NULL) {
		LOGP("ERROR - OUT OF MEM\n");
		free(stats->active_tasks);
		return -1;
	}
	// Case: new_tasks not NULL - realloc worked
	stats->active_tasks = new_tasks;
	return 0;
}

/**
 * @brief      Increases the appropriate task_stats counter for whatever state this task has
 *
 * @param      stats  	task_stats struct
 * @param[in]  state  	char representing the task's state
 */
void task_add_to_stats_count(struct task_stats *stats, char state)
{
	stats->total++; // Always increase total

	if (state == 'R') {
		stats->running++;
	}
	else if (state == 'D') {
		stats->waiting++;
	}
	// 'S' (sleeping) and 'I' (idle) both count as sleeping in this project
	else if (state == 'S' || state == 'I') {
		stats->sleeping++;
	}
	// 'T' (stopped) and 't' (disk stopped) both count as stopped in this project
	else if (state == 'T' || state == 't') {
		stats->stopped++;
	}
	else if (state == 'Z') {
		stats->zombie++;
	}
}

/**
 * @brief      Adds an active task to task_stats
 *
 * @param      stats             	task_stats struct
 * @param      status_file_info  	Task_Status_File_info struct
 */	
void task_add_active_task_to(struct task_stats *stats, Task_Status_File_Info *status_file_info)
{
	struct task_info* current_task = stats->active_tasks + stats->active_tasks_size;
	task_init_info(current_task, status_file_info);
	stats->active_tasks_size++;
}

/**
 * @brief      Removes the paraetheses from <pre>(state)</pre> and copies processed string to buffer
 *
 * @param      current_task  	task_stats struct
 * @param      state_token   	string representing state
 */
void task_process_and_copy_state(struct task_info* current_task, char *state_token)
{
	char state_copy[256];

	strcpy(state_copy, state_token + 3);
	state_copy[strlen(state_copy) - 1] = '\0';

	strcpy(current_task->state, state_copy);
}

/**
 * @brief      Destroys Task_Status_File_Info struct, including freeing allocated memory
 *
 * @param      status_file_info  	Task_Status_File_Info struct
 */
void task_destroy_status_file_info(Task_Status_File_Info *status_file_info)
{
	free_string( &(status_file_info->pid_token) );
	free_string( &(status_file_info->uid_token) );
	free_string( &(status_file_info->name_token) );
	free_string( &(status_file_info->state_token) );
}

/**
 * @brief      Convenience function, for debugging. Logs all the information in a Task_Status_File_Info struct.
 *
 * @param      status_file_info  Task_Status_File_Info struct
 */
void task_log_status_file_info(Task_Status_File_Info *status_file_info)
{
	LOG("STATUS FILE INFO:\n"
		"\t->pid_buf: '%s'\n"
		"\t->uid_buf: '%s'\n"
		"\t->name_buf: '%s'\n"
		"\t->state_buf: '%s'\n"
		"\n"
		"\t->pid_token: '%s'\n"
		"\t->uid_token: '%s'\n"
		"\t->name_token: '%s'\n"
		"\t->state_token: '%s'\n",
		status_file_info->pid_buf, status_file_info->uid_buf, status_file_info->name_buf, status_file_info->state_buf,
		status_file_info->pid_token, status_file_info->uid_token, status_file_info->name_token, status_file_info->state_token);
}