#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>

#include "os_searcher.h"
#include "string_extractor.h"
#include "logger.h"
#include "util.h"

void destroy_line_and_token(char **line_ptr, char **token_ptr);
void add_n_symbols_to_buf(int n, char *symbol, char *buf);
void uid_to_uname(char *name_buf, uid_t uid);
int open_path(const char *base, const char *extension);
ssize_t lineread(int fd, char *buf, size_t sz);
char *next_token(char **str_ptr, const char *delim);
void free_string(char **string_ptr);
int n_copy_if_present(char *dest, char *src, int n);
int n_copy_if_present_with_default(char *dest, char *src, int n, char *default_value);
void trim_leading_whitespace(char **current_ptr);
int reference_for_how_to_use_next_token(void);

/**
 * @brief      Destroys one line and one token, including freeing allocated memory
 *
 * @param      line_ptr   pointer to a line
 * @param      token_ptr  pointer to a token
 */
void destroy_line_and_token(char **line_ptr, char **token_ptr)
{
    free_string(line_ptr);
    free_string(token_ptr);
}

/**
 * @brief      Adds a symbol to a buf, n times.
 *
 * @param[in]  n        number of times to add symbol to a buf
 * @param      symbol   symbol to add
 * @param      buf      buffer to add to
 */
void add_n_symbols_to_buf(int n, char *symbol, char *buf)
{
    for (int i = 0; i < n; i++) {
        strcat(buf, symbol);
    }
}

/**
 * @brief      Gets a username from a uid and writes it to name_buf
 *
 * @param      name_buf     buffer to write to
 * @param[in]  uid          uid
 */
void uid_to_uname(char *name_buf, uid_t uid)
{
    char uid_str[256];
    sprintf(uid_str, "%d", uid);

    char* uname_token = search_for_uname(uid_str);
    n_copy_if_present_with_default(name_buf, uname_token, 16, uid_str);

    free_string(&uname_token);
}

/**
 * @brief      Opens a path to base/extension. Lovingly ripped out of lab code.
 *
 * @param[in]  base         base path
 * @param[in]  extension    extension path
 *
 * @return      A READ-ONLY file descriptor to this path. -1 on error.
 * 
 * @note This function opens a file descriptor. Must close() later.
 */
int open_path(const char *base, const char *extension)
{
    // Case: Invalid base or extension
    if (base == NULL || extension == NULL) {
        return -1;
    }

    // Add 2 for null terminators
    size_t path_size = (strlen(base) + strlen(extension) + 2) * sizeof(char);
    char* path = malloc(path_size);
    // Apparently this will concat base and extension, and make it the value of path
    snprintf(path, path_size, "%s/%s", base, extension);

    // Case: Invalid full path
    if (path == NULL) {
        return -1;
    }
    int path_fd = open(path, O_RDONLY);
    free(path);

    return path_fd;
}
/**
 * @brief      Reads one line (or up to sz bytes, whichever comes first) from a file. Also lovingly ripped out of lab code.
 *
 * @param[in]  fd    file descriptor   
 * @param      buf   buffer to store read line into
 * @param[in]  sz    max number of bytes to read
 *
 * @return     number of bytes read. If it's 0 or < sz, we've either encountered an error or reached the end of the line.
 */
ssize_t lineread(int fd, char *buf, size_t sz)
{
    for (size_t i = 0; i < sz; i++) {
        char c;
        ssize_t bytesRead = read(fd, &c, 1);

        // Case: EOF or error - return 0 (EOF) or -1 (error)
        if (bytesRead <= 0) {
            return bytesRead;
        }
        // Case: read is successful
        buf[i] = c;
       
        if (c == '\n') {
            buf[i] = '\0';
            return i + 1;
        }
    }

    // Case: All sz bytes have been read
    return sz;
}


/**
 * Lovingly and absolutely ripped from lab code
 */
/**

 */


/**
 * @brief      Ripped so hard from lab code the formatting changed. The following description is word-for-word from Prof. Malensek:
 * 
 *             Demonstrates string tokenization in C using the strspn(3) and strcspn(3)
 *             functions. Unlike strtok(3), this implementation is thread safe. The code
 *             is based on the following newsgroup post:
 *
 *             https://groups.google.com/forum/message/raw?msg=comp.lang.c/ff0xFqRPH_Y/Cen0mgciXn8J
 *
 * @param      str_ptr  maintains context in the string, i.e., where the next token in the
 *                      string will be. If the function returns token N, then str_ptr will be
 *                      updated to point to token N+1. To initialize, declare a char * that points
 *                      to the string being tokenized. The pointer will be updated after each
 *                      successive call to next_token.
 * @param[in]  delim    the set of characters to use as delimiters (Note from Jerome: This means that each one will count as a delimiter, not that the delimiter is all of them stuck together)
 *
 * @return     char pointer to the next token in the string.
 */
char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}



/**
 * @brief      Frees a string pointer and sets it to null. If the pointers are already null, does nothing.
 *
 * @param      string_ptr  pointer to a string
 */
void free_string(char **string_ptr)
{
    if (string_ptr != NULL && *string_ptr != NULL) {
        free(*string_ptr);
        *string_ptr = NULL;
    }
}

/**
 * @brief      Copies up to n chars from src into dest. If n == 0 or there is an error, there is no copy, and the function returns -1.
 *
 * @param      dest     destination string
 * @param      src      source string
 * @param[in]  n        max number of chars to copy
 *
 * @return      0 on success. 1 on error.
 */
int n_copy_if_present(char *dest, char *src, int n)
{
    if (dest == NULL || src == NULL || n == 0) {
        return -1;
    }

    /* Copy src into dest, then set its last index to NULL terminator */
    strncpy(dest, src, n-1);        
    dest[n-1] = '\0';
    return 0;
}

/**
 * @brief      Same as n_copy_if_present, but returns a default value if the function encounters any error
 *
 * @param      dest             destination string
 * @param      src              source string
 * @param[in]  n                max number of chars to copy
 * @param      default_value    default value
 *
 * @return      0, as it always succeeds.
 */
int n_copy_if_present_with_default(char *dest, char *src, int n, char *default_value)
{
    if (n_copy_if_present(dest, src, n) == -1) {
        strncpy(dest, default_value, n-1);
        dest[n-1] = '\0';
    }
    return 0;
}

/**
 * @brief      Moves a pointer to a string forward past any whitespace chars, effectively trimming leading whitespace from the string.
 *
 * @param      current_ptr      pointer to string
 */

void trim_leading_whitespace(char **current_ptr)
{
    while (**current_ptr == ' ') {
        (*current_ptr)++;
    }
}

/**
 * @brief      Do not use this function. It's just here so I remember how to use next_token(). Signed, Jerome.
 *
 * @return     Always 0.
 */
int reference_for_how_to_use_next_token(void)
{
    char str[] = "     This is a really great string, is it not?!";

    int tokens = 0;
    char *next_tok = str;
    char *curr_tok;
    /* Tokenize. Note that ' ,?!' will all be removed. */
    while ((curr_tok = next_token(&next_tok, " ,?!")) != NULL) {
        printf("Token %02d: '%s'\n", tokens++, curr_tok);
    }

    return 0;
}

