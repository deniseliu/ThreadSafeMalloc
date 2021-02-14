#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include<pthread.h>
typedef struct block{
  size_t size;
  struct block * next;
} block_t;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
block_t * freeList_lock=NULL;
__thread block_t * freeList_no_lock = NULL;
typedef void* (*sbrk_t) (intptr_t);
unsigned long data_segment=0;
unsigned long free_space_segment=0;
void * sbrk_lock(intptr_t size);
void * allocate_block(size_t size, sbrk_t fp);
block_t* find_bfblock(size_t size, block_t** list);
void * malloc_helper(block_t * curr, size_t size, block_t ** list, sbrk_t fp);
block_t* add_to_list(block_t * block_free, block_t ** list);
void merge_block(block_t* pre,block_t * curr);
//void merge_block();
void *bf_malloc(size_t size, block_t ** list, sbrk_t fp);
void bf_free(void *ptr, block_t ** list);
unsigned long get_data_segment_size(); 
unsigned long get_data_segment_free_space_size();
//Thread Safe malloc/free: locking version 
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);
//Thread Safe malloc/free: non-locking version
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr); 
