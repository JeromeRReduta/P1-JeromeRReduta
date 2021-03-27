#ifndef _PERC_BAR_DRAWER_H_
#define _PERC_BAR_DRAWER_H_

/**
 * @file Header file for perc_bar_filler.c
 */

/**
 * Struct holding info for creating a percent bar
 */
typedef struct {

	/** How full the bar should be in percent. If given a percent that is not a number, or below 0 percent, or 
	 above 100 percent, this var is a default value instead. */
    double safe_percent;

    /** The max number of hashes this bar can have. In this project, it's 20. */
    int max_hashes;

    /** The number of hashes this bar does have */
    int num_of_hashes;
} Percbar_Info;

void draw_percbar(char *buf, double frac);
double get_safe_percent_from(double frac);
int get_num_of_hashes_from(double safe_percent, int max_hashes);
void fill_in_perc_bar(char *buf, Percbar_Info *pb_info);
void make_bar(char *buf, Percbar_Info *pb_info);
void add_percent_string(char *buf, Percbar_Info *pb_info);

#endif
