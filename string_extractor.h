/**
 * @file
 *
 * Header file for string_extractor.c
 */

#ifndef _STRING_EXTRACTOR_H_
#define _STRING_EXTRACTOR_H_

char *extract_nth_token(int n, char **current_ptr, char **next_ptr, const char *delim);
char *extract_token_before(char *line, char *delim);
char *extract_token_after(char *line, char *delim);

#endif