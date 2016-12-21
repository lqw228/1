#ifndef _MEM_CHECK_H_
#define _MEM_CHECK_H_

#include <stdlib.h>

// instead of malloc
#define malloc(s) dbg_malloc1(s, __FILE__, __LINE__)

// instead of calloc
#define calloc(c, s) dbg_calloc1(c, s, __FILE__, __LINE__)

// instead of free
#define free(p) dbg_free1(p)

/**
 * allocation memory
 */
void *dbg_malloc1(size_t elem_size, char *filename, size_t line);

/**
 * allocation and zero memory
 */
void *dbg_calloc1(size_t count, size_t elem_size, char *filename, size_t line);

/**
 * deallocate memory
 */
void dbg_free1(void *ptr);

/**
 * show memory leake report
 */
void show_block();

#endif // _MEM_CHECK_H
