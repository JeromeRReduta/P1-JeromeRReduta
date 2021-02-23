#include <unistd.h>
#include <errno.h>
#include <stdio.h>

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
            return i+1;
        }
    }

    // Case: All sz bytes have been read
    return sz;
}

