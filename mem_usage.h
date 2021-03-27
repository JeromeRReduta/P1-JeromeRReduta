#ifndef _MEM_USAGE_H_
#define _MEM_USAGE_H_

/**
 * @file header file for mem_usage.c
 */

// Apparently if I do this a compiler warning goes away
// Based on advice from https://stackoverflow.com/questions/16831605/strange-compiler-warning-c-warning-struct-declared-inside-parameter-list
// This time, making it NOT extern makes warnings go away
struct mem_stats;

void mem_init(struct mem_stats *mstats, char *proc_dir);
void mem_log_stats(struct mem_stats *mstats);
#endif
