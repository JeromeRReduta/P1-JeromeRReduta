#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"
#include "logger.h"

void draw_percbar(char *buf, double frac) {

}

void uid_to_uname(char *name_buf, uid_t uid)
{
    strcpy(name_buf, "(UNKNOWN)");
}

// Lovingly ripped out of lab code
int open_path(char *base, char *extension)
{
    // Case: Invalid base or extension
    if (base == NULL || extension == NULL) {
        return -1;
    }

    // Add 2 for null terminators
    size_t path_size = (strlen(base) + strlen(extension) + 2) * sizeof(char);
    char* path = malloc(path_size);

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

