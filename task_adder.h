#ifndef _TASK_ADDER_H_
#define _TASK_ADDER_H_

/**
 * @file header file for task_adder.c
 */

typedef struct {
	char pid_buf[256];
	char uid_buf[256];
	char name_buf[256];
	char state_buf[256];

	char* pid_token;
	char* uid_token;
	char* name_token;
	char* state_token;
} Task_Status_File_Info;

struct task_stats;
struct task_info;


int task_read_status_and_store_into(char *proc_dir, char *entry_name, Task_Status_File_Info *status_file_info);
void task_populate_task_stats_with(struct task_stats *stats, Task_Status_File_Info *status_file_info);
void task_destroy_status_file_info(Task_Status_File_Info *status_file_info);
void task_log_status_file_info(Task_Status_File_Info *status_file_info);

#endif