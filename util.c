#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>



#include "util.h"
#include "logger.h"

double get_safe_percent(double frac);
void get_number_display(double safe_percent, char* number_display);



void draw_percbar(char *buf, double frac) {

    
    double safe_percent = get_safe_percent(frac);
    
    LOG("FRAC AND safe_percent:\n"
        "\tFRAC:\t%f\n"
        "\tsafe_percent:%f\n",
        frac, safe_percent);

    /* idk why I need to add 0.1 at the end, but 
    adding it means that 4.4% has 0 #s, and 4.5%
    has 1 */
    int num_of_hashes = safe_percent * 20 / 100 + 0.1;

    LOG("NUM OF HASHES:\t%d\n", num_of_hashes);
    buf[0] = '[';
    for (int i = 1; i < num_of_hashes + 1; i++) {
        buf[i] = '#';
    }

    for (int i = num_of_hashes + 1; i < 21; i++) {
        buf[i] = '-';
    }

    buf[21] = ']';
    buf[22] = ' ';

    // Ik the number won't be larger than 10 chars
    char number_display[10];

    get_number_display(safe_percent, number_display);

    LOG("CURRENT BUFFER:\t|%s|\n", buf);
    snprintf((buf + 23), strlen(number_display) + 1, "%s", number_display);



    LOG("CURRENT BUFFER:\t|%s|\n", buf);


}

double get_safe_percent(double frac) 
{
    // Error checking - safe_percent is = 0 if frac is invalid, = frac otherwise
    
    // Case: frac too low (or -0) or NaN
    if (frac <= 0.0 || isnan(frac) != 0) {
        return 0.0;
    }
    // Case: frac too high
    if (frac > 1.0) {
        return 100.0;
    }
    // Case: frac is between 0 and 100
    return frac * 100 + 0.0001; // Secret offset for numbers that end in .000999 c:
}

void get_number_display(double safe_percent, char* number_display)
{

    char safe_percent_str[256];
    
    snprintf(safe_percent_str, 256, "%f", safe_percent);

    

    char *decimal = strstr(safe_percent_str, ".");

    *(decimal + 2) = '%';
    *(decimal + 3) = '\0';
    LOG("CURRENT safe_percent_str:\t%s\n", safe_percent_str);

    strcpy(number_display, safe_percent_str);

}

void uid_to_uname(char *name_buf, uid_t uid)
{

    char uid_str[256];

    snprintf(uid_str, strlen(uid_str), "%i", uid);


    int passwd_fd = open_path("/etc", "passwd");


    char line[256] = {0};
    ssize_t read_sz;


    while ( (read_sz = lineread(passwd_fd, line, 256)) > 0) {
        char* head = line;
        char* num_search;


        for (int i = 0; i < 3; i++) {
            num_search = strsep(&head, ":");

        }
        LOG("COMPARING:\n"
            "\tUID:\t%i\n"
            "\tNUM_SEARCH\t%s\n",
            uid, num_search);

        if (uid == atoi(num_search)) {
            LOG("%s\n", "FOUND MATCH");

            char name[256];
            strcpy(name, head);

            LOG("\nFOUND NAME:\t|%s|\n", name);

            char* name_ptr = name;

            name_ptr = strsep(&head, ":");
        }
    }






close(passwd_fd);

/*
    while ( (read_sz = lineread(passwd_fd, line, 256)) > 0) {
        
        char* num_search = strstr(line, )
    }

    while ( (read_sz = lineread(mem_fd, line, 256)) > 0) {
        char* mem_avail_search = strstr(line, "MemAvailable") + '\0';
        char* mem_total_search = strstr(line,"MemTotal") + '\0';

        // Case: found key_name

        if (mem_avail_search != NULL) {
            mem_avail_search = strstr(mem_avail_search, ":") + 1;
            char* txt;

            mem_avail = strtod(mem_avail_search, &txt)/kb_to_mb;
            LOG("FOUND MEM_AVAIL:\t |%s|\n", mem_avail_search);
            // mem_avail[?] = '\0' Set ? to index of "k" in "kb"
        }

        if (mem_total_search != NULL) {
            mem_total_search = strstr(mem_total_search, ":") + 1;
            char* txt;

            mem_total = strtod(mem_total_search, &txt)/kb_to_mb;
            LOG("FOUND mem_total:\t |%s|\n", mem_total_search);
            // mem_total[?] = '\0' Set ? to index of "k" in "kb"
        }

      

    }

    LOG("VALUES:\n"
          "\tmem_avail:\t|%f|\n"
          "\tmem_total:\t|%f|\n",
          mem_avail, mem_total);
    
      // Case: key_name not found in file

      if (mem_avail == -1 || mem_total == -1) {
          LOG("ONE OR MORE VALUES NULL:\n"
          "\tmem_avail:\t%f\n"
          "\tmem_total:\t%f\n",
          mem_avail, mem_total);

          return -1;
      }

    
    strcpy(name_buf, "(UNKNOWN)");
    */
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
void copy_cpu_info(int cpu_fd, char* key, char* buf, size_t buf_sz)
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
            break;
        }
    }
      // Case: key_name not found in file



}




