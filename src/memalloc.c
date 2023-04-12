#include <stddef.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

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
	block->magic = 0x7777777;
      }
    }
  }
  return (block + 1);
}

// TEST
typedef  struct node {
  int data;
  struct node * next;
} node;
node *ll_head = NULL;

void insert_tail (int data) {
  node *temp, *current;
  temp = memalloc(sizeof(node));
  temp ->data = data;
  temp -> next = NULL;
  if(ll_head == NULL) {
    ll_head = temp;
  }
  else {
    current = ll_head;
    while(current->next != NULL) {
      current = current -> next;
    }
    current -> next = temp;
  }
}

void print_list() {
  node* current = ll_head;
  while(current != NULL) {
    printf("%d --> ", current->data);
    current = current->next;
  }
  printf("NULL");
}

// DRIVER
int main(int argc, char **argv)
{
  int input;
  while(input != -1) {
    printf("data: ");
    scanf("%d", &input);
    insert_tail(input);
  }
  print_list();
  return 0;
}
