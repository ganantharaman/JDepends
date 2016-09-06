/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser General Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/

#ifndef __MEM_INCLUDE__
#define __MEM_INCLUDE__

#ifdef __cplusplus
extern "C" {
#endif

/* Default size == 2^23 */
enum {
  BLOCK_SIZE=0x00800000, 
  BLOCK_TAG_SIZE=23
};

typedef struct mem_info{
	unsigned long  *  main_block;
	unsigned long  ** avail;
	unsigned long  max_tag_size;
	unsigned long  max_block_size;
	unsigned long  mgt_block_size;
	unsigned long  mem_pool_index;
} mem_info_t;

/* User level APIs */
void * mem_alloc( 
   unsigned long size
);

void * mem_calloc( 
   unsigned long size
);


void  mem_free(
   void * block
);

void * mem_realloc( 
   void * block ,
   unsigned long new_size
);

void * mem_dup(
   void * block
);

int  mem_is_valid_heap_ptr( 
   void * address
);

void mem_dump();

int  mem_init();

void mem_exit();

/* Private APIs */

void * mem_allocate_block( 
  	 unsigned long pool_index, 
     unsigned long tag,  
     unsigned long req_tag, 
     unsigned long req_size 
);

void * get_buddy(
    unsigned long pool_index,
    unsigned long tag,
    void * block
);

int   mem_expand_block(
    unsigned long pool_index,
    unsigned long new_size,
    unsigned long new_tag
);

int  mem_pool_init(
    unsigned long pool,
    mem_info_t * mem_info,
    unsigned long main_block_size ,
    unsigned long main_tag_size
);

void * mem_pool_alloc(
      unsigned long pool_index,
      unsigned long size,
     unsigned long * alloc_block_size,
     unsigned long * alloc_tag_size
);

int  has_same_tag( 
    unsigned int tag, 
    void * block
);


#ifdef __cplusplus
};
#endif

#define VALID(x) \
    ((x) != NULL)

#endif /* __MEM_INCLUDE__ */
