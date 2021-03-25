#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "logger.h"
#include "perc_bar_drawer.h"
/**
 * @File file whose sole responsibility is to fill a buffer with percent bar info
 * 
 * <br>
 * Percent bar has the following format:
 * <br>
 * <pre>
 * [(20 # or -)] #.#%
 * </pre>
 * 
 * <br> E.g.:
 * <pre>
 * [####----------------] 20.0%
 * </pre>
 */



// Found out format for formatted float from https://stackoverflow.com/questions/1343890/how-do-i-restrict-a-float-value-to-only-two-places-after-the-decimal-point-in-c
void draw_percbar(char *buf, double frac)
{
    Percbar_Info pb_info = {
        .safe_percent = get_safe_percent_from(frac),
        .max_hashes = 20,
        .num_of_hashes = get_num_of_hashes_from(pb_info.safe_percent, pb_info.max_hashes)
    };

    fill_in_perc_bar(buf, &pb_info);
}


double get_safe_percent_from(double frac)
{
    if (frac > 1.0) {
        return 100;
    }
    // For some reason, if we don't check for -0.00, then get_safe_percent_from(-0.00) returns -0.00...
    else if (isnan(frac) || frac < 0.0 || frac == -0.0) {
        return 0;
    }
    return frac * 100;
}

int get_num_of_hashes_from(double safe_percent, int max_hashes)
{
    double rounded_percent = round(safe_percent); // Adding 0.1 so 99.5 rounds to 100 instead of 99
    return rounded_percent / 100 * max_hashes;
}

void fill_in_perc_bar(char *buf, Percbar_Info *pb_info)
{
    if (buf == NULL || pb_info == NULL) {
        return;
    }

    make_bar(buf, pb_info);
    add_percent_string(buf, pb_info);
}

void make_bar(char *buf, Percbar_Info *pb_info)
{
    buf[0] = '\0';
    add_n_symbols_to_buf(1, "[", buf);
    add_n_symbols_to_buf(pb_info->num_of_hashes, "#", buf);
    add_n_symbols_to_buf(pb_info->max_hashes - pb_info->num_of_hashes, "-", buf);
    add_n_symbols_to_buf(1, "]", buf);  
}

void add_percent_string(char *buf, Percbar_Info *pb_info)
{
    char percent_string[256];
    sprintf(percent_string, " %.1f%%", pb_info->safe_percent);
    add_n_symbols_to_buf(1, percent_string, buf);
}
