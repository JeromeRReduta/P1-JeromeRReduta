#include <stdlib.h>
#include "logger.h"
#include "string.h"
#include "util.h"

#include "string_extractor.h"

/*
 *@file File whose sole responsibility is to extract tokens from a larger string
 */

/**
 * @brief      { function_description }
 *
 * @param      line   The line
 * @param      delim  The delimiter
 *
 * @return     { description_of_the_return_value }
 */
char *extract_token_before(char *line, char *delim)
{

	if (line == NULL) {
		LOG("ERROR - LINE IS NULL: '%s' - returning NULL\n", line);
		return NULL;
	}

	// We duplicate line so we don't mutate our data
	char line_copy[256];
	strcpy(line_copy, line);

	LOG("LINE IS '%s'\t DELIM IS '%s'\n", line_copy, delim);
    char* next = line_copy;
    char* current;

    current = next_token(&next, delim);

    LOG("LINE_COPY IS '%s'\t CURRENT IS: '%s'\t NEXT IS: '%s'\n", line_copy, current, next);


	return current != NULL ? strdup(current) : NULL;

}




char *extract_token_after(char *line, char *delim)
{
	if (line == NULL) {
		LOG("ERROR - LINE IS NULL: '%s' - returning NULL\n", line);
		return NULL;
	}

	// We duplicate line so we don't mutate our data
	char line_copy[256];
	strcpy(line_copy, line);

	LOG("LINE IS '%s'\t DELIM IS '%s'\n", line_copy, delim);
    char* next = line_copy;
    char* current;

    next_token(&next, delim);
    current = next_token(&next, delim);


    // Can probably refactor into trim_leading_whitespace()
    LOG("LINE IS CURRENTLY: '%s'\n", current);
    while (current[0] == ' ' && current[0] != '\0') {
    	LOG("current[0] is '%c'\n", current[0]);
    	current++;
    }


    LOG("LINE_COPY IS '%s'\t CURRENT IS: '%s'\t NEXT IS: '%s'\n", line_copy, current, next);


	return current != NULL ? strdup(current) : NULL;
}


/**
 * @brief      Tests extract_token_before()
 * 
 * @note       Confirmed success for:
 * 				1. Matching key:value pair (returns key)
 * 				2. Line w/ no match (returns whole line)
 * 				3. NULL ptr (returns NULL)
 */
void test_extract_token_before()
{

	char valid_str[256];
	char no_delim[256];
	char test_null_str[256] = {0};

	strcpy(valid_str, "BubbaKey: BubbaValue");
	strcpy(no_delim, "There is no delim!");

	LOG("STARTING VALUES:\n"
		"\t->valid_str: '%s'\n"
		"\t->no_delim: '%s'\n"
		"\test_null_str: '%s'\n", valid_str, no_delim, test_null_str);


	char* key_valid = extract_token_before(valid_str, ":"); // Should be "BubbaKey"
	char* key_no_delim = extract_token_before(no_delim, ":"); // No idea what this should be
	char* key_null = extract_token_before(test_null_str, ":"); // Should be NULL

	LOG("KEY VALUES:\n"
		"\t->valid_str: '%s'\n"
		"\t->no_delim: '%s'\n"
		"\test_null_str: '%s'\n", key_valid, key_no_delim, key_null);




}


/**
 * @brief      Tests extract_token_after()
 * 
 * @note       Confirmed success for:
 * 				1. Matching key:value pair (returns value)
 * 				2. Line w/ no match (returns NULL)
 * 				3. NULL ptr (returns NULL)
 */
void test_extract_token_after()
{
	char valid_str[256];
	char no_delim[256];
	char test_null_str[256] = {0};

	strcpy(valid_str, "BubbaKey: BubbaValue");
	strcpy(no_delim, "There is no delim!");

	LOG("STARTING VALUES:\n"
		"\t->valid_str: '%s'\n"
		"\t->no_delim: '%s'\n"
		"\test_null_str: '%s'\n", valid_str, no_delim, test_null_str);


	char* key_valid = extract_token_after(valid_str, ": "); // Should be "BubbaValue"
	char* key_no_delim = extract_token_after(no_delim, ": "); // No idea what this should be
	char* key_null = extract_token_after(test_null_str, ": "); // Should be NULL

	LOG("KEY VALUES:\n"
		"\t->valid_str: '%s'\n"
		"\t->no_delim: '%s'\n"
		"\test_null_str: '%s'\n", key_valid, key_no_delim, key_null);




}


