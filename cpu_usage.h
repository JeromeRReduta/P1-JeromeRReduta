#ifndef _CPU_USAGE_H_
#define _CPU_USAGE_H_

/**
 * @file header file for cpu_usage.c
 */

// Apparently if I do this a compiler warning goes away
// Based on advice from https://stackoverflow.com/questions/16831605/strange-compiler-warning-c-warning-struct-declared-inside-parameter-list
extern struct cpu_stats curr;

void cpu_init(char *proc_dir, struct cpu_stats *curr);
double cpu_calc_usage(struct cpu_stats *prev, struct cpu_stats *curr);
void cpu_log_info(struct cpu_stats *curr);

#endif

