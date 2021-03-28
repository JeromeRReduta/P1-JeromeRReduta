#include <stdlib.h>
#include "logger.h"
#include "string.h"
#include "util.h"

#include "string_extractor.h"

/**
 * @file File whose sole responsibility is to extract strings from larger strings
 */

/**
 * @brief      Extracts token in line before the first appearance of delimiter
 *
 * @param      line   line
 * @param      delim  delimiter
 *
 * @return     If delimiter exists, returns token in line before first appearance of delimiter. Else, returns NULL.
 * 
 * @note       Allocates memory. Must free() later. May segfault if get_nth_token() segfaults.
 */
char *extract_token_before(char *line, char *delim)
{
	if (line == NULL) {
		LOG("ERROR - LINE IS NULL: '%s' - returning NULL\n", line);
		return NULL;
	}
	
	char line_copy[256]; // We duplicate line so we don't mutate our data
	strcpy(line_copy, line);

    char* next = line_copy;
    char* current;

    current = extract_nth_token(1, &current, &next, delim);

	return current != NULL ? strdup(current) : NULL;
}

/**
 * @brief      Extracts token in line after the first appearance of delimiter.
 *
 * @param      line   line
 * @param      delim  delimiter
 *
 * @return     If delimiter exists, returns token in line after first apperance of delimiter. Else, returns NULL.
 * 
 * @note       Allocates memory. Must free() later. May segfault if get_nth_token segfaults.
 */
char *extract_token_after(char *line, char *delim)
{
	if (line == NULL) {
		LOG("ERROR - LINE IS NULL: '%s' - returning NULL\n", line);
		return NULL;
	}


	char line_copy[256]; // We duplicate line so we don't mutate our data
	strcpy(line_copy, line);

    char* next = line_copy;
    char* current;

    current = extract_nth_token(2, &current, &next, delim);

    trim_leading_whitespace(&current);

	return current != NULL ? strdup(current) : NULL;
}

/**
 * @brief      Gets the nth token from a tokenized string
 *
 * @param[in]  n            n, to get nth token
 * @param      current_ptr  pointer to current - used for next_token()
 * @param      next_ptr     pointer to next - used for next_token()
 * @param[in]  delim        delimiter
 *
 * @return     The nth token.
 * 
 * @note       Warning: This function will segfault if you try to go out of bounds, e.g. getting the 4th token when the string only has 3.
 * 			   You must know the number of tokens in advance before you can safely use this function.
 */
char *extract_nth_token(int n, char **current_ptr, char **next_ptr, const char *delim)
{
	for (int i = 0; i < n; i++) {
		*current_ptr = next_token(next_ptr, delim);
	}
	return *current_ptr;
}
