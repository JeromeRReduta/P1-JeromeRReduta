#ifndef _LOAD_AVG_H_
#define _LOAD_AVG_H_

struct load_avg;
void load_avg_init_with(struct load_avg *lavg_ptr, const char *load_avg_line);

#endif