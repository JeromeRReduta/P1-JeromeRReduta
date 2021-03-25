#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "os_searcher.h"
#include "string_extractor.h"
#include "logger.h"
#include "util.h"





void destroy_line_and_token(char **line_ptr, char **token_ptr);
void add_n_symbols_to_buf(int n, char *symbol, char *buf);

int open_path(const char *base, const char *extension);
ssize_t lineread(int fd, char *buf, size_t sz);
char *next_token(char **str_ptr, const char *delim);
int reference_for_how_to_use_next_token(void); // TODO: Delete this when done
void free_string(char **string_ptr);


void destroy_line_and_token(char **line_ptr, char **token_ptr)
{
    free_string(line_ptr);
    free_string(token_ptr);
}


void add_n_symbols_to_buf(int n, char *symbol, char *buf)
{
    for (int i = 0; i < n; i++) {
        strcat(buf, symbol);
    }
}


// Note: Figure out how to find uname from uid again
void uid_to_uname(char *name_buf, uid_t uid)
{
    char uid_str[256];
    sprintf(uid_str, "%d", uid);

    char* uname_token = search_for_uname(uid_str);
    n_copy_if_present_with_default(name_buf, uname_token, 16, uid_str);

    free_string(&uname_token);
}

// Lovingly ripped out of lab code - Note: HAVE to close @ end of func
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

    //LOG("Opening path: %s\n", path);
    int path_fd = open(path, O_RDONLY);

    // Once we have our fd, can free path
    free(path);

    return path_fd;
}

// Also ripped out of lab code - reads one line from file
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
 * Demonstrates string tokenization in C using the strspn(3) and strcspn(3)
 * functions. Unlike strtok(3), this implementation is thread safe. The code
 * is based on the following newsgroup post:
 *
 * https://groups.google.com/forum/message/raw?msg=comp.lang.c/ff0xFqRPH_Y/Cen0mgciXn8J
 */


/**
 * Retrieves the next token from a string.
 *
 * Parameters:
 * - str_ptr: maintains context in the string, i.e., where the next token in the
 *   string will be. If the function returns token N, then str_ptr will be
 *   updated to point to token N+1. To initialize, declare a char * that points
 *   to the string being tokenized. The pointer will be updated after each
 *   successive call to next_token.
 *
 * - delim: the set of characters to use as delimiters
 *
 * Returns: char pointer to the next token in the string.
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

// TODO: Remove once done using as reference
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

void free_string(char **string_ptr)
{
    LOGP("STARTING FREE_STRING\n");
    LOG("Value of string ptr: '%s'\n", string_ptr != NULL ? *string_ptr : "NULL");

    if (string_ptr != NULL && *string_ptr != NULL) {
        free(*string_ptr);
        *string_ptr = NULL;
    }

    LOG("Value of string ptr now: '%s'\n", string_ptr != NULL ? *string_ptr : "NULL");
}

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

int n_copy_if_present_with_default(char *dest, char *src, int n, char *default_value)
{
    if (n_copy_if_present(dest, src, n) == -1) {
        strncpy(dest, default_value, n-1);
        dest[n-1] = '\0';
    }
    return 0;

}

void test_n_copy_if_present()
{

    LOGP("TEST - SRC IS NULL\n");

    char* test0_src = NULL;
    char test0_dest[256];

    strcpy(test0_dest, "Bubba");

    LOG("NULL SRC: '%s'\t test0_dest: '%s'\n", test0_src, test0_dest);

    LOG("RETURN VALUE OF n_copy_if_present IS %d\n", n_copy_if_present(test0_dest, test0_src, 256));


    LOGP("TEST - DEST IS NULL\n");

    char* test1_src = "Bubba";
    char* test1_dest = NULL;

    LOG("SRC: '%s'\t DEST: '%s'\n", test1_src, test1_dest);

    LOG("RETURN VALUE OF n_copy_if_present IS %d\n", n_copy_if_present(test1_dest, test1_src, 256));




    LOGP("TEST - NEITHER NULL\n");
    char* test2_src = "Bradley";
    char test2_dest[256];

    strcpy(test2_dest, "Bubba");
    LOG("SRC: '%s'\t DEST: '%s'\n", test2_src, test2_dest);

    LOG("RETURN VALUE OF n_copy_if_present IS %d\n", n_copy_if_present(test2_dest, test2_src, 256));

    LOG("Value of test2_dest now: '%s'\n", test2_dest);

    LOGP("TEST - TRUNCATE - SHOULD BE AB\\0\n");
    char* test3_src = "ABC";
    char test3_dest[256];

    n_copy_if_present(test3_dest, test3_src, 2);

    LOG("test3_dest is now '%s'\n", test3_dest);


    LOGP("TEST - DEFAULT VALUE\n");
    char* test_default_src = NULL;
    char test_default_dest[256] = {0};

    LOG("RETURN VALUE OF n_copy_if_present w/ default value IS %d\n", n_copy_if_present_with_default(test_default_dest, test_default_src, 256, "Placeholder"));
    LOG("DEST IS NOW: '%s'\n", test_default_dest);


}