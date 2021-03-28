#ifndef _UTIL_H_
#define _UTIL_H_

extern void draw_percbar(char *buf, double frac); // Added this so test wouldn't have warning :D
void destroy_line_and_token(char **line_ptr, char **token_ptr);
void uid_to_uname(char *name_buf, uid_t uid);
int open_path(const char *base, const char *extension);
ssize_t lineread(int fd, char *buf, size_t sz);
char *next_token(char **str_ptr, const char *delim);
void free_string(char **string_ptr);
int n_copy_if_present(char *dest, char *src, int n);
int n_copy_if_present_with_default(char *dest, char *src, int n, char *default_value);
void add_n_symbols_to_buf(int n, char *symbol, char *buf);
void trim_leading_whitespace(char **current_ptr);

#endif