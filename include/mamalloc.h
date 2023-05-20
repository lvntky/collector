#ifndef __MEMALLOC_H__
#define __MEMALLOC_H__
#include <stddef.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct block_meta
{
  size_t size;
  struct block_meta *next;
  int free;
  // debug
  int magic;
} block_meta;

block_meta *find_free_block(block_meta **last, size_t size);
block_meta *request_block(block_meta *last, size_t size);
void *memalloc(size_t size);
block_meta *return_last_allocated_chunk_adress(void *ptr);
void mem_free(void *ptr);
void *mem_realloc(void* ptr, size_t size);
void *mem_calloc(size_t melem, size_t nelem);

#endif
