/**
 * @file
 *
 * Header file for string_extractor.c
 */

#ifndef _STRING_EXTRACTOR_H_
#define _STRING_EXTRACTOR_H_

void test_extract_token_before();
void test_extract_token_after();

char *extract_token_before(char *line, char *delim);
char *extract_token_after(char *line, char *delim);

#endif