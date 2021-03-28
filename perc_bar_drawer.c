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

/**
 * @brief      Draws the whole percent bar
 *
 * @param      buf   buffer to put percent bar in
 * @param[in]  frac  fraction input
 */

void draw_percbar(char *buf, double frac);
double get_safe_percent_from(double frac);
int get_num_of_hashes_from(double safe_percent, int max_hashes);
void fill_in_perc_bar(char *buf, Percbar_Info *pb_info);
void make_bar(char *buf, Percbar_Info *pb_info);
void add_percent_string(char *buf, Percbar_Info *pb_info);

void draw_percbar(char *buf, double frac)
{
    Percbar_Info pb_info = {
        .safe_percent = get_safe_percent_from(frac),
        .max_hashes = 20,
        .num_of_hashes = get_num_of_hashes_from(pb_info.safe_percent, pb_info.max_hashes)
    };

    fill_in_perc_bar(buf, &pb_info);
}

/**
 * @brief      Converts frac into a percent. If frac is not a number, or is out of bounds (less than 0.0 or -0.0 or greater than 1.0), gives a default value instead 
 *
 * @param[in]  frac  The frac
 *
 * @return     frac * 100, or a default value if frac is not a number, or is out of bounds.
 */
double get_safe_percent_from(double frac)
{
    // Case: frac too high - return 100
    if (frac > 1.0) {
        return 100;
    }
    // Case: frac not a number or too low - return 0
    // For some reason, if we don't check for -0.00, then get_safe_percent_from(-0.00) returns -0.00...
    else if (isnan(frac) || frac < 0.0 || frac == -0.0) {
        return 0;
    }
    // Case: valid frac - return frac * 100
    return frac * 100;
}

/**
 * @brief      Gets the number of hashes, based of safe_percent and max_hashes
 *
 * @param[in]  safe_percent  safe percent
 * @param[in]  max_hashes    max hashes
 *
 * @return     safe_percent, rounded to nearest whole number, divided by 100 and multipled by max_hashes
 */
int get_num_of_hashes_from(double safe_percent, int max_hashes)
{
    return round(safe_percent) / 100 * max_hashes;
}

/**
 * @brief      Given Percbar_Info struct, draws the percent bar
 *
 * @param      buf      buffer to write into
 * @param      pb_info  Percbar_Info struct
 */
void fill_in_perc_bar(char *buf, Percbar_Info *pb_info)
{
    if (buf == NULL || pb_info == NULL) {
        return;
    }

    make_bar(buf, pb_info);
    add_percent_string(buf, pb_info);
}

/**
 * @brief      Writes the <pre>[###----]</pre> part of the percent bar
 *
 * @param      buf      buffer to write into
 * @param      pb_info  Percbar_Info struct
 */
void make_bar(char *buf, Percbar_Info *pb_info)
{
    buf[0] = '\0';
    add_n_symbols_to_buf(1, "[", buf);
    add_n_symbols_to_buf(pb_info->num_of_hashes, "#", buf);
    add_n_symbols_to_buf(pb_info->max_hashes - pb_info->num_of_hashes, "-", buf);
    add_n_symbols_to_buf(1, "]", buf);  
}

/**
 * @brief      Writes the <pre> ab.c% </pre> part of the percent bar
 *
 * @param      buf      buffer to write into
 * @param      pb_info  Percbar_Info struct
 * 
 * @note       Found out format for formatted float from https://stackoverflow.com/questions/1343890/how-do-i-restrict-a-float-value-to-only-two-places-after-the-decimal-point-in-c
 */
void add_percent_string(char *buf, Percbar_Info *pb_info)
{
    char percent_string[256];
    sprintf(percent_string, " %.1f%%", pb_info->safe_percent);
    add_n_symbols_to_buf(1, percent_string, buf);
}
