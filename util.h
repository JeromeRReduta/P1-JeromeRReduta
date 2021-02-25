#ifndef _UTIL_H_
#define _UTIL_H_

#include <sys/types.h>

int open_path(char *base, char *extension);
/**
 * Reads a line from a file descriptor and copies its contents into a buffer.
 * 
 * Note: Only including in util because I can't figure out how to import this func from lineread.c and lineread.h
 * TODO: Ask how to import this and ask about this error that shows up in my terminal:
 * 
 * [jrreduta@KevinM P1-JeromeRReduta]$ make
 * cc -g -Wall -fPIC -DLOGGER=1 -lm -lncurses  inspector.o procfs.o display.o util.o -o inspector
 * /usr/bin/ld: procfs.o: in function `pfs_hostname':
 * /home/jrreduta/P1-JeromeRReduta/procfs.c:10: undefined reference to `lineread'

 *
 * @param fd File descriptor to read
 * @param buf Destination buffer to copy line contents
 * @param sz Maximum number of bytes read from the file descriptor
 *
 * @return number of bytes read, 0 if EOF is reached, -1 on error
 */
ssize_t lineread(int fd, char *buf, size_t sz);

void draw_percbar(char *buf, double frac);

void uid_to_uname(char *name_buf, uid_t uid);




int pfs_get_aspect(char *proc_dir, char *buf, size_t buf_sz, char *extension);
int copy_cpu_info(int cpu_fd, char* key, char* buf, size_t buf_sz);



#endif
