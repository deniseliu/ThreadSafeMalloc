#include "my_malloc.h"

void * sbrk_lock(intptr_t size){
  pthread_mutex_lock(&lock);
  void * ptr = sbrk(size);
  pthread_mutex_unlock(&lock);
  return ptr;
}
void * allocate_block(size_t size, sbrk_t fp){
  if(size<=0){
    return NULL;
  }
  void * ptr= fp(size+sizeof(block_t));
  if(ptr==(void *)-1){
    return NULL;
  }
  block_t * new_block=ptr;
  new_block->size=size;
  new_block->next=NULL;
  return new_block+1;  
}


block_t* find_bfblock(size_t size, block_t ** list){
  block_t * bf=NULL;
  block_t * curr= *list;
  while(curr!=NULL){
    if(curr->size==size){
      return curr;
    }else if(curr->size>size){
      if(bf==NULL||bf->size>curr->size){
        bf=curr;
      }
    }
    curr=curr->next;
  }
  return bf;
}

void * malloc_helper(block_t * curr, size_t size, block_t ** list, sbrk_t fp){
  if(curr!=NULL){
    size_t block_size= size+sizeof(block_t);
    if(curr->size>=block_size){
      curr->size-=block_size;
      block_t * new_block= (void *)curr+sizeof(block_t)+curr->size;
      new_block->size=size;
      new_block->next=NULL;
      return new_block+1;
    }else{
      block_t * pre= *list ;
      if(*list==curr){
        *list = (*list)->next;
      }else{
        while(pre->next!=curr){
          pre=pre->next;
        }
        pre->next=curr->next;
      }
      return curr+1;
    }
  }else{
    return allocate_block(size,fp);
  }
}

block_t * add_to_list(block_t * block_free, block_t **list){
  if(*list==NULL){
    *list=block_free;
    return NULL;
  }
  else if(block_free< *list){
    block_free->next=*list;
    *list = block_free;
    return NULL;
  }
  else{
    block_t * curr= *list;
    while(curr->next!=NULL&&curr->next<block_free){
      curr=curr->next;
    }
    block_free->next=curr->next;
    curr->next=block_free;
    return curr;
  }
}
void merge_block(block_t * pre,block_t * curr){
  block_t * next=curr->next;
  if(next!=NULL&&curr->next==(void *)curr+curr->size+sizeof(block_t)){
    curr->size+= next->size+sizeof(block_t);
    curr->next=next->next;
  }
  if(pre!=NULL&&curr==(void *)pre+pre->size+sizeof(block_t)){
    pre->size+= curr->size+sizeof(block_t);
    pre->next=curr->next;
  }
  }



void * bf_malloc(size_t size, block_t ** list, sbrk_t fp){
  if(*list==NULL){
    return allocate_block(size,fp);
  }else{
    block_t * curr=find_bfblock(size,list);
    return malloc_helper(curr,size,list,fp);
  }
}


void bf_free(void * ptr,block_t ** list){
  if(ptr==NULL){
    return;
  }
  block_t* block_free= ptr-sizeof(block_t);
  block_t * pre=add_to_list(block_free,list);
  //merge_block();
  merge_block(pre,block_free);
  //printFreeList();
}

void *ts_malloc_lock(size_t size){
  pthread_mutex_lock(&lock);
  void * ptr = bf_malloc(size, &freeList_lock,sbrk);
  pthread_mutex_unlock(&lock);
  return ptr;
}
void ts_free_lock(void *ptr){
  pthread_mutex_lock(&lock);
  bf_free(ptr,&freeList_lock);
  pthread_mutex_unlock(&lock);
}
//Thread Safe malloc/free: non-locking version
void *ts_malloc_nolock(size_t size){
  return bf_malloc(size, &freeList_no_lock,sbrk_lock);
}
void ts_free_nolock(void *ptr){
  bf_free(ptr,&freeList_no_lock);
}



