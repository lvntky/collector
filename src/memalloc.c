#include <stddef.h>
#include <unistd.h>
#include <assert.h>

/*meta information of heap*/
/*track the size of chunk for free*/
/*whether occupied or not*/
typedef struct block_meta {
  size_t size;
  struct block_meta* next;
  int free;
  int debug_it; // debug purpose only. delete it.
} block_meta;
#define META_SIZE sizeof(struct block_meta) // size of meta information block

void* global_head = NULL; // head of block linkedList

/*check is there any available freespace*/
/*is not, request it from OS*/
block_meta *find_available_block(block_meta **last, size_t size) {
  block_meta* current = global_head;
  while (current && !(current->free && current->size >= size)) {
    *last = current;
    current = current -> next;
      }
  return current;
}

/*if find_available_block returns null and can't find
 any available free space, request it from OS and add tail of LL*/
block_meta *request_space_from_os(block_meta *last, size_t size) {
  block_meta*  block;
  block = sbrk(0); // current adress of heap's end.
  void* request = sbrk(size + META_SIZE);
  assert((void *)block == request); // NOT THREAD SAFE
  if (request == (void *) -1) {
    return NULL; // sbrk failed
      }
  // last should be NULL from first call. 
  if (last) {
    last->next = block;
      }
  block->size = size;
  block->next = NULL;
  block->free = 0; // occupied
  block->debug_it = 0x12345678;
  return block;
}

void *memalloc(size_t size) {
  // TODO: aling pointers??
  block_meta* block;
  if (size <= 0) {
    return NULL;
      }
  // first call
  if (!global_head) {
    block = request_space_from_os(NULL, size);
    if (!block) {
      return NULL;
	}
    global_head = block;
      } else {
    block_meta *last = global_head;
    block = find_available_block(&last, size);
    // failed to find free space
    if (!block) {
      block = request_space_from_os(last, size);
      if (!block) {
	return NULL;
	  }
	} else {
      // found free block
      block->free = 0;
      block->debug_it = 0x7777777;
	}
      }
  return block;
}


int main(int argc, char **argv) {
  memalloc(10);
  return 0;
}
