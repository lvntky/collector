#include "../include/collect.h"
#include "../include/mamalloc.h"

// TODO:
static void scan_region(unsigned int *stack_pointer, unsigned int end_stack);
static void scan_heap(void);
void collector_init(void);
void collector_collect(void);
