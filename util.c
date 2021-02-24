#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "util.h"
#include "logger.h"

void draw_percbar(char *buf, double frac) {

}

void uid_to_uname(char *name_buf, uid_t uid)
{
    strcpy(name_buf, "(UNKNOWN)");
}

// Lovingly ripped out of lab code - Note: HAVE to close @ end of func
int open_path(char *base, char *extension)
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

    LOG("Opening path: %s\n", path);
    int path_fd = open(path, O_RDONLY);

    // Once we have our fd, can free path
    free(path);

    return path_fd;
}

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

int pfs_get_aspect(char *proc_dir, char *buf, size_t buf_sz, char *extension)
{
    int hostname_fd = open_path(proc_dir, extension);
    ssize_t read_sz = lineread(hostname_fd, buf, buf_sz);

    close(hostname_fd);

    // Note: Won't pass tests unless this func returns either 0 or 1
    return read_sz > 0 ? 0 : -1;
}

/* Parses proc/cpuinfo for given key string
 * Returns pointer to value of key string if key exists
 * Else returns NULL */
void *copy_cpu_info(int cpu_fd, char* key, char* buf, size_t buf_sz)
{

    /* I was told to add this comment in lab:
     * Max model name size for model buf is 128; we're
     * adding a bit more buffer space
     * 
     * Also forgot I can't instantiate arrays w/ variable length -
     * would have to define global var at top, but would move var
     * too far away from this func. I'll just hard-code 256 instead. */
    char line[256] = {0};
    ssize_t read_sz;

    while ( (read_sz = lineread(cpu_fd, line, 256)) > 0) {
        char* key_name = strstr(line, key);

        // Case: found key_name
        if (key_name != NULL) {
            // Skip : and space after
            key_name = strstr(key_name, ": ") + 2;
            LOG("FOUND KEY:\t'%s'\n", key_name);

            key_name[buf_sz-1] = '\0';

            strcpy(buf, key_name);
            return;
        }
    }
      // Case: key_name not found in file



}

