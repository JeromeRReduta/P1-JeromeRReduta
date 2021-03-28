#ifndef _TASK_ADDER_H_
#define _TASK_ADDER_H_

/**
 * @file header file for task_adder.c
 */

/**
 * Struct holding information from reading status file
 */
typedef struct {

	/** Holds line of pid info */
	char pid_buf[256];

	/** Holds line of uid info */
	char uid_buf[256];

	/** Holds line of name info */
	char name_buf[256];

	/** Holds line of state info */
	char state_buf[256];

	/** Holds formatted pid token */
	char* pid_token;

	/** Holds formatted uid token */
	char* uid_token;

	/** Holds formatted name token */
	char* name_token;

	/** Holds formatted state token */
	char* state_token;
} Task_Status_File_Info;

struct task_stats;
struct task_info;

int task_read_status_and_store_into(char *proc_dir, char *entry_name, Task_Status_File_Info *status_file_info);
void task_populate_task_stats_with(struct task_stats *stats, Task_Status_File_Info *status_file_info);
void task_destroy_status_file_info(Task_Status_File_Info *status_file_info);
void task_log_status_file_info(Task_Status_File_Info *status_file_info);

#endif