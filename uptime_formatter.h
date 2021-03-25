#ifndef _UPTIME_FORMATTER_H_
#define _UPTIME_FORMATTER_H_

typedef struct {
	int seconds;
	int minutes;
	int hours;
	int days;
	int years;
} Uptime_Info;

char *time_f_get_time_str(double time, char *uptime_buf);

#endif