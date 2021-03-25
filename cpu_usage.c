#include <stdlib.h>
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
void cpu_reset(struct cpu_stats *curr);
void add_token_to_cpu_stats(struct cpu_stats *curr, long current_num, int counter);

double cpu_calc_usage(struct cpu_stats *prev, struct cpu_stats *curr);

void cpu_log_info(struct cpu_stats *curr);

void cpu_init(char *proc_dir, struct cpu_stats *curr)
{
	// Have to reset current, or else it remembers values from prev
	cpu_reset(curr);
	
	char* cpu_usage_line = search_for_cpu_usage(proc_dir);
	char* next = cpu_usage_line;
	char* current = next_token(&next, " ");

	int counter = 0;
	while ( (current = next_token(&next, " ")) != NULL) {
		add_token_to_cpu_stats(curr, atol(current), counter++);
		LOG("adding: '%s'\n", current);
	}

	LOGP("STATS NOW:\n");
	cpu_log_info(curr);
	free_string(&cpu_usage_line);
}

void cpu_reset(struct cpu_stats *curr)
{
	curr->idle = 0;
	curr->total = 0;
}

void add_token_to_cpu_stats(struct cpu_stats *curr, long current_num, int counter)
{
	if (counter == 3) {
		curr->idle += current_num;
	}
	curr->total += current_num;
}

double cpu_calc_usage(struct cpu_stats *prev, struct cpu_stats *curr)
{
	long idle = curr->idle - prev->idle;
	long total = curr->total - prev->total;

	if (total <= 0 || idle < 0 || total < idle) {
		LOG("ERROR - TOTAL IS %ld\t IDLE IS %ld\t; RETURNING 0.0\n", total, idle);
		return 0.0;
	}



	long double used = (long double)idle/total;

	LOG("STATS:\n"
		"\t->idle: %ld\n"
		"\t->total: %ld\n"
		"\t->used: %Lf\n",
		idle, total, used);
	return (double)(1-used);
}



void cpu_log_info(struct cpu_stats *curr)
{

	LOG("CPU STATS:\n"
		"\t->Idle: %ld\n"
		"\t->Total: %ld\n",
		curr->idle, curr->total);

}
