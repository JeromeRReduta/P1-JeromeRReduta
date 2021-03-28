#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "logger.h"
#include "uptime_formatter.h"

/**
 * @File file whose sole responbility is to format a time number into a string readable by humans.
 * 
 * Prefix: time_f
 */

/** This file has no vars */

/** Func prototypes */
char *time_f_get_time_str(double time);
void time_f_init_uptime_info(Uptime_Info *time_info, double time);
void time_f_normalize(int *smaller_unit, int *larger_unit, int base);

char *time_f_make_time_str(Uptime_Info *time_info);
void time_f_add_years_days_and_hours(char *time_str, Uptime_Info *time_info);
void time_f_add_minutes_and_seconds(char *time_str, Uptime_Info *time_info);
void time_f_add_time_to_str(char *time_str, int time, char *unit_str, bool always_add);

void time_f_log_info(Uptime_Info *time_info);

/**
 * @brief      Converts uptime to a formatted uptime string and returns it
 *
 * @param[in]  time        uptime
 *
 * @return     A formatted uptime string, readable by humans
 */
char *time_f_get_time_str(double time)
{
	Uptime_Info time_info = {0};
	time_f_init_uptime_info(&time_info, time);

	return time_f_make_time_str(&time_info);
}

/**
 * @brief      Initalizes time_info struct
 *
 * @param      time_info  	time_info struct
 * @param[in]  time       	OS uptime
 */
void time_f_init_uptime_info(Uptime_Info *time_info, double time)
{
	time_info->seconds = (int)time;

	time_f_normalize(&(time_info->seconds), &(time_info->minutes), 60);
	time_f_normalize(&(time_info->minutes), &(time_info->hours), 60);
	time_f_normalize(&(time_info->hours), &(time_info->days), 24);
	time_f_normalize(&(time_info->days), &(time_info->years), 365);
}

/**
 * @brief      Normalizes a smaller unit of time by setting it to a base, and carrying the rest over to hte larger unit
 * <br> E.g. if seconds = 100 and minutes = 0, after time_f_normalize(seconds, minutes, 60) seconds = 40 and minutes = 1
 *
 * @param      smaller_unit  Address of smaller unit of time
 * @param      larger_unit   Address of larger unit of time
 * @param[in]  base          How many of the smaller_unit are in the larger_unit (e.g. 60 seconds in 1 minute)
 */
void time_f_normalize(int *smaller_unit, int *larger_unit, int base)
{
	*larger_unit = *smaller_unit / base;
	*smaller_unit %= base;

}

/**
 * @brief      Given info from Uptime_Info struct, creates a formatted time string
 *
 * @param      time_info  	Uptime_Info struct
 *
 * @return     	A formatted time string, readable by humans
 */
char *time_f_make_time_str(Uptime_Info *time_info)
{
	char time_str[512] = {0};
	time_f_add_years_days_and_hours(time_str, time_info);
	time_f_add_minutes_and_seconds(time_str, time_info);

	return strdup(time_str);
}

/**
 * @brief      Adds years, days, and hours to formatted time string, but will only add each if they are greater than 0.
 *
 * @param      time_str   	time string
 * @param      time_info  	Uptime_Info struct
 */
void time_f_add_years_days_and_hours(char *time_str, Uptime_Info *time_info)
{
	time_f_add_time_to_str(time_str, time_info->years, "years, ", false);
	time_f_add_time_to_str(time_str, time_info->days, "days, ", false);
	time_f_add_time_to_str(time_str, time_info->hours, "hours, ", false);

}

/**
 * @brief      Adds minutes and seconds to formatted time string.
 *
 * @param      time_str   	time string
 * @param      time_info  	Uptime_Info struct
 */
void time_f_add_minutes_and_seconds(char *time_str, Uptime_Info *time_info)
{
	time_f_add_time_to_str(time_str, time_info->minutes, "minutes, ", true);
	time_f_add_time_to_str(time_str, time_info->seconds, "seconds", true);
}

/**
 * @brief      If time is greater than 0, or we are always supposed to add the string, adds a string with the format <pre> time unit_str </pre> to time_str.
 *
 * @param      time_str    	time string
 * @param[in]  time        	time
 * @param      unit_str    	unit string - essentially, something we concat onto the formatted time
 * @param[in]  always_add  	whether to always add this string
 */
void time_f_add_time_to_str(char *time_str, int time, char *unit_str, bool always_add)
{
	if (always_add || time > 0) {
		
		char time_unit_str[256] = {0};
		sprintf(time_unit_str, "%d ", time);
		strcat(time_unit_str, unit_str);

		strcat(time_str, time_unit_str);
	}
}

/**
 * @brief      Convenience function, for debugging. Logs all info from Uptime_Info struct
 *
 * @param      time_info  	Uptime_Info struct
 */
void time_f_log_info(Uptime_Info *time_info)
{
	LOG("STATS:\n"
		"\t->seconds: %d\n"
		"\t->minutes: %d\n"
		"\t->hours: %d\n"
		"\t->days: %d\n"
		"\t->years: %d\n",
		time_info->seconds, time_info->minutes, time_info->hours, time_info->days, time_info->years);
}