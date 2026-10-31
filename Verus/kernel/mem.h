#ifndef MEM_H
#define MEM_H

void mem_init(unsigned int magic, unsigned int mbi_addr);
int mem_valid(void);
unsigned int mem_lower_kb(void);
unsigned int mem_upper_kb(void);

#endif