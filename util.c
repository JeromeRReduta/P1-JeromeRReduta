#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "util.h"
#include "logger.h"

int open_path(const char *base, const char *extension);
ssize_t lineread(int fd, char *buf, size_t sz);
char *next_token(char **str_ptr, const char *delim);
int reference_for_how_to_use_next_token(void); // TODO: Delete this when done
void free_string(char **string_ptr);




void draw_percbar(char *buf, double frac) {

}

void uid_to_uname(char *name_buf, uid_t uid)
{
    strcpy(name_buf, "(UNKNOWN)");
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

int copy_if_present(char *dest, char *src)
{
    if (dest == NULL || src == NULL) {
        return -1;
    }

    strcpy(dest, src);
    return 0;
}

void test_copy_if_present()
{

    LOGP("TEST - SRC IS NULL\n");

    char* test0_src = NULL;
    char test0_dest[256];

    strcpy(test0_dest, "Bubba");

    LOG("NULL SRC: '%s'\t test0_dest: '%s'\n", test0_src, test0_dest);

    LOG("RETURN VALUE OF COPY_IF_PRESENT IS %d\n", copy_if_present(test0_dest, test0_src));


    LOGP("TEST - DEST IS NULL\n");

    char* test1_src = "Bubba";
    char* test1_dest = NULL;

    LOG("SRC: '%s'\t DEST: '%s'\n", test1_src, test1_dest);

    LOG("RETURN VALUE OF COPY_IF_PRESENT IS %d\n", copy_if_present(test1_dest, test1_src));




    LOGP("TEST - NEITHER NULL\n");
    char* test2_src = "Bradley";
    char test2_dest[256];

    strcpy(test2_dest, "Bubba");
    LOG("SRC: '%s'\t DEST: '%s'\n", test2_src, test2_dest);

    LOG("RETURN VALUE OF COPY_IF_PRESENT IS %d\n", copy_if_present(test2_dest, test2_src));

    LOG("Value of test2_dest now: '%s'\n", test2_dest);



}