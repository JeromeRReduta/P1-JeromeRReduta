#include <stdlib.h>
#include "logger.h"
#include "os_searcher.h"
#include "string.h"
#include "util.h"
#include "string_extractor.h"

#include "procfs.h"
#include "load_avg.h"

/**
 * @file File whose sole responsibility is to populate a load_avg struct
 */

/** Func prototypes */
void load_avg_init_with(struct load_avg *lavg_ptr, const char *load_avg_line);
void load_avg_init_values(struct load_avg *lavg_ptr, char **current_ptr, char **next_ptr);
void load_avg_set_value(double *lavg_value, char **current_ptr, char **next_ptr);
/**
 * @brief      Initalizes load_avg info with load_avg_line
 *
 * @param      lavg_ptr       ptr to load_avg struct
 * @param[in]  load_avg_line  line containing load avg values
 */
void load_avg_init_with(struct load_avg *lavg_ptr, const char *load_avg_line)
{
    if (lavg_ptr == NULL || load_avg_line == NULL) {
        LOGP("ERROR - LOAD_AVG OR LOAD_AVG_LINE IS NULL PTR\n");
        return;
    }

    char load_avg_line_copy[256] = {0};
    strcpy(load_avg_line_copy, load_avg_line);

    char *next = load_avg_line_copy;
    char *current;
    load_avg_init_values(lavg_ptr, &current, &next);
}

/**
 * @brief      Equivalent of: <br>
 *             <pre>
 *                  if ( (current = next_token(...) != NULL)) {
 *                      lavg_ptr->one = current;
 *                  }
 *                  if ( (current = next_token(...) != NULL)) {
 *                      lavg_ptr->five = current;
 *                  }
 *                  if ( (current = next_token(...) != NULL)) {
 *                      lavg_ptr->fifteen = current;
 *                  }
 *             </pre>
 * 
 *
 * @param      lavg_ptr     pointer to struct load_avg
 * @param      current_ptr  pointer to current string, for next_token()
 * @param      next_ptr     pointer to next string, for next_token()
 */
void load_avg_init_values(struct load_avg *lavg_ptr, char **current_ptr, char **next_ptr)
{
    load_avg_set_value(&(lavg_ptr->one), current_ptr, next_ptr);
    load_avg_set_value(&(lavg_ptr->five), current_ptr, next_ptr);
    load_avg_set_value(&(lavg_ptr->fifteen), current_ptr, next_ptr);
}

/**
 * @brief      Sets a value in load_avg struct to the next token
 *
 * @param      lavg_value   ptr to load_avg_one or load_avg_five or load_avg_fifteen
 * @param      current_ptr  pointer to current, used in next_token
 * @param      next_ptr     pointer to next, used in next_token
 */
void load_avg_set_value(double *lavg_value, char **current_ptr, char **next_ptr)
{
    if ( (*current_ptr = next_token(next_ptr, " ,?!")) != NULL) {
        *lavg_value = atof(*current_ptr);
    }
}