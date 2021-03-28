#ifndef _UPTIME_FORMATTER_H_
#define _UPTIME_FORMATTER_H_

/**
 * Struct containing uptime info for this OS.
 */
typedef struct {
	int seconds;
	int minutes;
	int hours;
	int days;
	int years;
} Uptime_Info;

char *time_f_get_time_str(double time);
void time_f_log_info(Uptime_Info *time_info);

#endif