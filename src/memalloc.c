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

#define META_SIZE sizeof(block_meta)
void *global_head = NULL;

block_meta *find_free_block(block_meta **last, size_t size)
{
  block_meta *current = global_head;
  // search until current chunk of memory is not free
  // and not big as required size.
  // if the chunk is free and big enough(found the block) or current chunk hit the NULL (not found the block)
  // break the loop.
  while(current && !(current->free && current->size >= size)) {
    *last = current;
    current = current->next;
  }
  return current;
}

// if find_free_block returns NULL
// (can't find any block that matching the criterias)
// request space from OS via sbrk syscall
block_meta *request_block(block_meta *last, size_t size)
{
  block_meta *block;
  block = sbrk(0); // sbrk(0) returns current break of the heap
  void *request = sbrk(size + META_SIZE);
  assert((void *)block == request);
  if(request == ((void *)-1)) {
    return NULL;
  }
  if(last){
    last->next = block;
  }
  block->size = size;
  block->next = NULL;
  block->free = 0;
  block->magic = 0x12345678;

  return block;
}

void *memalloc(size_t size)
{
  block_meta *block;
  if(size == 0) {
    return NULL;
  }
  // first call
  if(global_head == NULL) {
    block = request_block(NULL, size);
    if(block == NULL) {
      //sbrk failed
      return NULL;
    }
    global_head = block;
  } else {
    block_meta *last = global_head;
    block = find_free_block(&last, size);
    if(block == NULL) {
      block = request_block(last, size);
      if(block == NULL) {
	return NULL;
      } else { // found free block
	block->free = 0;
	block->magic = 0x77777777;
      }
    }
  }
  // return next of the current allocated block
  return (block + 1);
}

block_meta *return_last_allocated_chunk_adress(void *ptr)
{
  return (block_meta*) ptr -1;
}

// free need to set -> free
void mem_free(void *ptr)
{
  if(!ptr) {
    return;
  }
  block_meta *last_allocated_chunk = return_last_allocated_chunk_adress(ptr);
  if(last_allocated_chunk != NULL) {
    assert(last_allocated_chunk->free == 0);
  }else {
    last_allocated_chunk->free = 1;
  }
}
// realloc should adjust the size of allocated chunk by malloc
// If passed NULL just act like malloc
// If passed previously allocated pointer
//    free up the space, if request size is bigger than prev allocted size
//    allocate more space, wipe the previously allocated space
//    copy the content to new allocated bigger space
void *mem_realloc(void* ptr, size_t size)
{
  if(ptr == NULL) {
    return memalloc(size);
  }
  block_meta *last_allocated_chunk = return_last_allocated_chunk_adress(ptr);
  if(last_allocated_chunk->size >= size) {
    // we have enough space, nothing to do...
    return ptr;
  }
  void *bigger_chunk;
  bigger_chunk = memalloc(size);
  if(!bigger_chunk) {
    return NULL; // TODO: custom errno
  }
  memcpy(bigger_chunk,last_allocated_chunk, last_allocated_chunk->size);
  mem_free(last_allocated_chunk);
  return bigger_chunk;
}

// calloc - just wipe the memory before return pointer
void *mem_calloc(size_t melem, size_t nelem) {
  size_t size = melem * nelem;
  void *ptr = memalloc(size);
  memset(ptr, 0, size);
  return ptr;
}

// TEST
typedef struct node {
  int data;
  struct node* next;
} node;
node *head_linked = NULL;
void insert_tail(int data) {
  node *temp;
  temp = memalloc(sizeof(node));
  temp->data = data;
  temp->next = NULL;
  if(head_linked == NULL) {
    head_linked = temp;
  }
  else {
    node * current;
    current = head_linked;
    while(current->next != NULL) {
      current = current->next;
    }
    current->next = temp;
  }
}
void print_linked_list() {
  node* current = head_linked;
  while(current->next != NULL) {
    printf("%d --> ", current->data);
    current = current->next;
  }
  printf("NULL");
}
int main(int argc, char **argv)
{
  int val = 0;
  while(val != -1) {
    printf("data: ");
    scanf("%d", &val);
    insert_tail(val);
  }
  print_linked_list();
  return 0;
}
