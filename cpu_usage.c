#include <stdlib.h>
#include <stdbool.h>
#include "logger.h"
#include "os_searcher.h"
#include "string.h"
#include "util.h"
#include "string_extractor.h"

#include "procfs.h"
#include "cpu_usage.h"

/**
 * @file File whose sole responsibility is to track the OS's cpu-usage
 * 
 * Prefix: cpu
 */

/* Func prototypes */
void cpu_init(char *proc_dir, struct cpu_stats *curr);
void populate_cpu_stats(struct cpu_stats *curr, char *cpu_usage_line);
void add_token_to_cpu_stats(struct cpu_stats *curr, long current_num, int counter);

void cpu_reset(struct cpu_stats *curr);

double cpu_calc_usage(struct cpu_stats *prev, struct cpu_stats *curr);

void cpu_log_info(struct cpu_stats *curr);

/**
 * @brief      Initalizes cpu_stats
 *
 * @param      proc_dir  proc directory
 * @param      curr      current cpu_stats struct
 */
void cpu_init(char *proc_dir, struct cpu_stats *curr)
{
	cpu_reset(curr); // Have to reset current, or else it remembers values from prev
	
	char* cpu_usage_line = search_for_cpu_usage(proc_dir);
	populate_cpu_stats(curr, cpu_usage_line);

	cpu_log_info(curr);
	free_string(&cpu_usage_line);
}

/**
 * @brief      Adds data from cpu_usage_line to current cpu struct
 *
 * @param      curr            current cpu_stats struct
 * @param      cpu_usage_line  line of cpu usage data
 */
void populate_cpu_stats(struct cpu_stats *curr, char *cpu_usage_line)
{
	char* next = cpu_usage_line;
	char* current = next_token(&next, " ");

	int counter = 0;
	while ( (current = next_token(&next, " ")) != NULL) {
		add_token_to_cpu_stats(curr, atol(current), counter++);
		LOG("adding: '%s'\n", current);
	}
}

/**
 * @brief      Adds a single token from a line of cpu usage data to curr's total usage. If it's
 * 				the 4th token, adds it to curr's idle usage, too.
 *
 * @param      curr         current cpu_stats struct
 * @param[in]  current_num  token from line, converted to a long
 * @param[in]  counter      counter, to track which token this func is adding
 */
void add_token_to_cpu_stats(struct cpu_stats *curr, long current_num, int counter)
{
	// Case: 4th token (counter == 3)
	if (counter == 3) {
		curr->idle += current_num;
	}

	curr->total += current_num;
}

/**
 * @brief      Resets cpu_stats values to 0
 *
 * @param      curr  current cpu_stats struct
 */
void cpu_reset(struct cpu_stats *curr)
{
	curr->idle = 0;
	curr->total = 0;
}

/**
 * @brief      Calculates cpu usage. If given invalid values, returns 0.0 instead.
 *
 * @param      prev  previous cpu_stats struct from the previous pfs_cpu_usage() call
 * @param      curr  current cpu_stats struct from this pfs_cpu_usage() call
 *
 * @return     The cpu_usage, defined as 1 - (curr->idle - prev->idle) / (curr->total - prev->total).
 *             If given invalid values, will return 0 instead.
 */
double cpu_calc_usage(struct cpu_stats *prev, struct cpu_stats *curr)
{
	long idle = curr->idle - prev->idle;
	long total = curr->total - prev->total;
	bool has_invalid_values = total <= 0 || idle < 0 || total < idle;

	return has_invalid_values ? 0.0 : 1 - (long double)idle/total;
}

/**
 * @brief      Convenience function, for debugging. Logs info in a cpu_stats struct.
 *
 * @param      curr  current cpu_stats struct
 */
void cpu_log_info(struct cpu_stats *curr)
{
	LOG("CPU STATS:\n"
		"\t->Idle: %ld\n"
		"\t->Total: %ld\n",
		curr->idle, curr->total);
}
