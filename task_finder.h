#ifndef _TASK_FINDER_H_
#define _TASK_FINDER_H_

/**
 * @file header file for task_finder.c
 */

#include "task_adder.h"
// Apparently if I do this a compiler warning goes away
// Based on advice from https://stackoverflow.com/questions/16831605/strange-compiler-warning-c-warning-struct-declared-inside-parameter-list
// This time, making it NOT extern makes warnings go away

struct task_stats;
struct task_info;


extern const int task_init_max_len;


struct task_stats *task_init_stats();
void task_destroy_stats(struct task_stats **stats);
void task_get_tasks_from_proc(char *proc_dir, struct task_stats *stats);

void task_log_stats(struct task_stats *stats);
void task_log_all_active_tasks(struct task_stats *stats);
void task_log_info(struct task_info info);

#endif
