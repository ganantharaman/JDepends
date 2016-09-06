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

#include <mem.h>
#include <cmnmcros.h>

#ifdef __THREAD__
#include <lock.h>
#else
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#endif

#include <stdio.h>
#include <assert.h>


#if defined(__MEMBENCH_TEST__) && defined(__WIN32__) 
#include <time.h>
#endif

#define IN 
#define OUT

typedef struct TAG_SIZE_TABLE
{
  int   tag;
  unsigned long size;
} TAG_SIZE_TABLE;


/* TODO: Eliminate this table. This is a silly way to optimize */
static TAG_SIZE_TABLE tstable[] = 
{
{0,0},{1,2},{1,2},{2,4},{2,4},{3,8},{3,8},{3,8},{3,8},
{4,16},{4,16},{4,16},{4,16},{4,16},{4,16},{4,16},{4,16}
,{5,32},{5,32},{5,32},{5,32},{5,32},{5,32},{5,32},{5,32}
,{5,32},{5,32},{5,32},{5,32},{5,32},{5,32},{5,32},{5,32}
,{6,64},{6,64},{6,64},{6,64},{6,64},{6,64},{6,64},{6,64}
,{6,64},{6,64},{6,64},{6,64},{6,64},{6,64},{6,64},{6,64}
,{6,64},{6,64},{6,64},{6,64},{6,64},{6,64},{6,64},{6,64}
,{6,64},{6,64},{6,64},{6,64},{6,64},{6,64},{6,64},{6,64}
,{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128}
,{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128}
,{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128}
,{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128}
,{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128}
,{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128}
,{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128}
,{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128},{7,128}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256},{8,256}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
,{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512},{9,512}
};

/* use a global lock */
THREAD_CHECK( lock_t g_mem_lock; )

mem_info_t *g_mem_info = NULL;

/**        ************** B U D D Y   S Y S T E M ******************
  * NOTE: 
  * 1)  Blocks always belong to a pool
  * 2)  There is a always a default pool whose ID is BASE_POOL 
  * 3)  Pools are numbered up from BASE_POOL
  * 4)  Each pool initially allocates a Main block whose size is 
  *     either BLOCK_SIZE or greater.The value depends upon the requested
  *     size.subsequent memory requests will split the Main block
  * 5)  All block management within a pool follows the Buddy system
  * 6)  A new pool comes into existance only if all the existing pools
  *     fail to satisfy a request for a size.The whole main block
  *     within such pools may be reserved or the main block may be free
  *	    and its size is smaller than the requested size
  * 7)  We have always   2 ^ ( juMaxTagSize ) = juMaxBlockSize 
  * 8)  BlockTypeBit = { ReservedBit(1) or  FreeBit(0) }
  * 9)  First bit of a block denotes whether the block is reserved or free
  * 10) For a Block Always 2 ^ (BlockTag)  = BlockSize;
  * 11) First  unsigned long of a Block is BlockTypeBit OR BlockTag ( BlockTypeBit | BlockTag )
  * 12) Second unsigned long of a Block is a ptr to Next block of same BlockSize ( or BlockTag )
  * 13) Third  unsigned long of a Block is a ptr to Prev Block of same BlockSize ( or BlockTag )
  * 14) Fourth unsigned long of a Block is a ptr to the Block's pool
  * 15) The BlockSize is always of the form 2 ^( x )  for   0 <= x <= juMaxBlockSize
  * 16) With the Current design MARK_RESERVED must always be called 
  *     after setting TAG_SIZE
  * 17) Any block overwrites is obviously unacceptable ,Will be extending to 
  *     detect any misuse of blocks for DEBUG version
  * 18) Pool info is maintained by g_mem_info which is reallocated whenever
  *     a new pool needs to be created
  * 19) Last unsigned long of a Block MUST ALWAYS HAVE THE BLOCK's POOL ID
  * 20) Do not touch this code unless you have understood it
  **/

#define BASE_POOL  0
#define MEM_INFO( pool )  (&g_mem_info[pool])

#define MEM_MAINBLOCK( pool )       MEM_INFO( pool )->main_block
#define MEM_AVAIL( pool )           MEM_INFO( pool )->avail
#define MEM_MAXTAGSIZE( pool )      MEM_INFO( pool )->max_tag_size
#define MEM_MAXBLOCKSIZE( pool )    MEM_INFO( pool )->max_block_size
#define MEM_MGTBLOCKSIZE( pool )    MEM_INFO( pool )->mgt_block_size
#define MEM_MEMPOOLINDEX( pool )    MEM_INFO( pool )->mem_pool_index 
#define MEM_FREELIST(pool , x )     MEM_INFO( pool )->avail[(x) - 1] 

#define MEM_BASEBLOCK  MEM_MAINBLOCK( BASE_POOL ) 


#define UL_SIZE   sizeof( unsigned long )
#define HIGH_BIT  (~(unsigned long)0 - (~(unsigned long)0 >> 1))
#define SHED_HIGH_BIT( x ) ((x) << 1 ) >> (unsigned long)1

/* To the first unsigned long of a block */
#define TAG_SIZE( x )			*((unsigned long *)(x)) 
/* Second unsigned long of a block */
#define NEXT_BLOCK( x )			*((unsigned long *)(x) + 1)
/* Third unsigned long of a block */
#define PREV_BLOCK( x )			*((unsigned long *)(x) + 2)
/* Fourth unsigned long of a block */
#define REQ_SIZE( x )           *((unsigned long *)(x) + 3)
/* Fifth unsigned long of a block */
#define MEM_POOL( x )			*((unsigned long *)(x) + 4)

#define MARK_FREE( x )			*((unsigned long *)(x))  = SHED_HIGH_BIT( *((unsigned long *)(x)) )
#define MARK_RESERVED( x )		*((unsigned long *)(x)) |= HIGH_BIT
#define IS_RESERVED_BLOCK( x ) (*((unsigned long *)(x)) & HIGH_BIT)
        

/* Init functions ideally can't throw exceptions since 
   exception context and  frame are assumed 
   to be uninitialized .
   But exception functions get the thread context 
   since JvmGetCurrentThread which is used in
   almost all exception fns, give an existing
   thread control block ptr or create one and give
   it anyway.
*/

int mem_init()
{
  THREAD_CHECK( lock_init(&g_mem_lock); )
	return mem_pool_init( 
		 BASE_POOL,
		 NULL,
		 BLOCK_SIZE,
		 BLOCK_TAG_SIZE );
}

int mem_pool_init(
  IN  unsigned long pool_index,
  IN  mem_info_t * mem_info,
  IN  unsigned long main_block_size,
  IN  unsigned long main_tag_size
)
{
   unsigned long * block = NULL;
   THREAD_CHECK( lock_acquire(&g_mem_lock); )

	/* Allocate pool control block */
   g_mem_info = (mem_info_t *)
	   realloc(mem_info, ( pool_index + 1) * sizeof( mem_info_t )); 
   if ( !VALID(g_mem_info) ) { 
     THREAD_CHECK( lock_release(&g_mem_lock); )
     return 0;
   }

   MEM_MAINBLOCK( pool_index )  = (unsigned long *)calloc( main_block_size, 1 );
   block = MEM_MAINBLOCK( pool_index );

   if (!VALID( block ) ) 
   {
 	  free( g_mem_info );
	  g_mem_info = NULL;
    THREAD_CHECK( lock_release(&g_mem_lock); )
	  return 0;
   }
   
   MEM_AVAIL( pool_index )          = (unsigned long **)calloc( main_tag_size *
                                                          sizeof( unsigned long *), 1);
   MEM_MAXBLOCKSIZE( pool_index )   =  (unsigned long)main_block_size;
   MEM_MGTBLOCKSIZE( pool_index )   =  5;  /* ULONGs */
   MEM_MAXTAGSIZE  ( pool_index )   =  (unsigned long)main_tag_size;
   MEM_MEMPOOLINDEX( pool_index )   =  pool_index;

   /* ReservedFlag(0/1) + size */
   TAG_SIZE  ( block )   = (unsigned long)main_tag_size;
   NEXT_BLOCK( block )   = (unsigned long)0;
   PREV_BLOCK( block )   = (unsigned long)0;
   MEM_POOL  ( block )   = (unsigned long)pool_index;
   REQ_SIZE  ( block )   = 0;
   /* Mark the block as available */
   MARK_FREE ( block );
   /* Update the avail list */
   MEM_FREELIST( pool_index,main_tag_size ) = block;

   THREAD_CHECK( lock_release(&g_mem_lock); )

   return 1;
}


/* NON - Re entrant */
void * mem_alloc(
  IN  unsigned long req_size
)
{
	void * block = NULL;
	register unsigned long current_pool_index = BASE_POOL;
	unsigned long alloc_block_size = 0;
	unsigned long alloc_tag_size = 0;
	register unsigned long last_pool = 0;


	/**
	  * TODO may be add acquire mutex here  for accessing global
	  *    g_mem_info 
	  *
	  **/
	/* Pool index of the BASE_POOL is the
	   index of the last pool */
	last_pool = MEM_MEMPOOLINDEX( current_pool_index );
	
    /* Then try other pools */
	for ( ;current_pool_index <= last_pool ; current_pool_index++ )
	{
       block = mem_pool_alloc(current_pool_index ,req_size,
		               &alloc_block_size, &alloc_tag_size);
       if ( VALID( block ) )
	      return	block;
	}

	/* Try allocating a new pool whose ID will be current_pool_index*/
	if ( !mem_pool_init(
		      current_pool_index ,
		     g_mem_info ,	
             ( alloc_block_size > BLOCK_SIZE ) ?
               alloc_block_size : BLOCK_SIZE,
			 ( alloc_tag_size   > BLOCK_TAG_SIZE ) ? 
               alloc_tag_size   : BLOCK_TAG_SIZE	  
			)
       )
       return NULL;
	
	/* Retry the allocation attempt  */
    block = mem_pool_alloc(current_pool_index ,req_size,
	               &alloc_block_size, &alloc_tag_size);

	/* Found a block */
	if (  VALID( block ) )
 	  MEM_MEMPOOLINDEX( BASE_POOL ) = current_pool_index;

	/* return the result anyway */
	return block;
}

/* NON Re-entrant */
void * mem_pool_alloc(
  IN     unsigned long pool_index,
  IN     unsigned long size,
  IN OUT unsigned long * alloc_block_size,
  IN OUT unsigned long * alloc_tag_size
)
{
  register unsigned long  req_block_size = 1;
  register unsigned long  max_tag;
  register unsigned long  req_tag = 0;
  register unsigned long  index = 0;
  register unsigned long  req_size = size;
  void * p;

#if defined(__MEMBENCH_TEST__) && defined(__WIN32__) 
 LARGE_INTEGER stime,etime;
#endif 
  THREAD_CHECK(lock_acquire(&g_mem_lock);)

  assert( g_mem_info );

  max_tag  = MEM_MAXTAGSIZE( pool_index );
  size   += (UL_SIZE * (MEM_MGTBLOCKSIZE( pool_index )));

  /** This check avoids repetitive 
      loop executions to calculate the next highest
	  power of two that can satisy the request 
	  Assumption is that if *alloc_block_size
	  is not zero then it represents the right 
	  power of two to satisfy requested size.

   **/
  if (!*alloc_block_size )
  {
    if ( size <= 512 )
	  {
      req_block_size = tstable[size].size;
	    req_tag        = tstable[size].tag;
	  }	else {
      for ( ;; )
  	  {
  	    if ( size <= req_block_size )
	 	      break;
	      req_block_size <<= 1;
	      req_tag ++;
	    }
    }

	  *alloc_block_size = req_block_size;
    *alloc_tag_size   = req_tag;
  } else {
	   req_block_size  = *alloc_block_size;
     req_tag         = *alloc_tag_size;
  }

#if defined(__MEMBENCH_TEST__) && defined(__WIN32__) 
  QueryPerformanceCounter(&stime);
#endif
  index = req_tag;
  /* Normal Case */
  for ( ;index <= max_tag ;index++ )
  {
  	if ( MEM_FREELIST(pool_index,index) )
	{
		p = mem_allocate_block( pool_index,index,req_tag,req_size ); 
#if defined(__MEMBENCH_TEST__) && defined(__WIN32__) 
		QueryPerformanceCounter(&etime);
		printf("PoolAlloc Normal = %ld\n", etime.QuadPart-stime.QuadPart);
#endif
    THREAD_CHECK(lock_release(&g_mem_lock);)
		return p;
	}
  }

  /* Try expanding the block */
  if (!mem_expand_block(pool_index, req_block_size,req_tag )) {
    THREAD_CHECK( lock_release(&g_mem_lock); )
	  return NULL;
  }

  /* Try re allocating */
  p =  mem_allocate_block(pool_index, req_tag ,req_tag, req_size );

#if defined(__MEMBENCH_TEST__) && defined(__WIN32__) 
		QueryPerformanceCounter(&etime);
		printf("pool Alloc Expand= %ld\n", etime.QuadPart-stime.QuadPart);
#endif
  THREAD_CHECK(lock_release(&g_mem_lock);)
  return p;
}


/* NON Re-entrant */
void * mem_allocate_block( 
  	IN unsigned long pool_index, 
    IN unsigned long tag,  
    IN unsigned long req_tag, 
    IN unsigned long req_size 
)
{
	register unsigned long *block     = NULL;
	register unsigned long *next_block = NULL;
	register unsigned long *split_block     = NULL;
	register unsigned long  index      = tag;
	void * p;
   /* By default we set the block to the first availbale 
	  block in the tag */
	block     = (unsigned long *)MEM_FREELIST( pool_index,tag );
	assert( block != 0 );
	next_block = (unsigned long *) NEXT_BLOCK( block );
	split_block     = block;
  MEM_FREELIST( pool_index,tag ) 
		          = next_block;

    if (  next_block  )
       PREV_BLOCK( next_block ) =  (unsigned long)0;

	/* Split , if need be */
    for ( ; index - req_tag; )
  	{
	    --index;
		  split_block = (unsigned long *)((unsigned char *)block + ( 1 << index ));
		  TAG_SIZE( split_block )   = index;
		  /* This step is redundant but we still 
		    do it since the freebit could change */
		  MARK_FREE( split_block );

      /* unlink the block */ 
		  next_block			 = (unsigned long *)MEM_FREELIST(pool_index, index );
		  NEXT_BLOCK( split_block ) = (unsigned long)next_block;
		  if ( next_block )
 		    PREV_BLOCK ( next_block ) = (unsigned long)split_block;
   	     PREV_BLOCK( split_block ) = (unsigned long)0;
		  /* link the newly formed split block */
		  MEM_FREELIST( pool_index,index ) = split_block;
	  }

  /* Set the tag size */
  TAG_SIZE( block ) = req_tag;
  /* Reserve the block */
  MARK_RESERVED( block );
	REQ_SIZE( block ) = req_size;
  /* Set the pool ID - reqd. since a block 
   goes back to its pool when it is freed */
  MEM_POOL( block ) = (unsigned long)pool_index;

  p = block + MEM_MGTBLOCKSIZE( pool_index );
  /* return a usable pointer */
  return p;
}

/* NON Re-entrant */
void mem_free( 
  IN void *  block_to_free 
)
{
    register unsigned long * buddy       = NULL;
    register unsigned long * block       = NULL;
    register unsigned long * next_to_buddy = NULL;
    register unsigned long * prev_to_buddy = NULL;
    register unsigned long tag_of_buddy     = 0;
    register unsigned long block_mem_pool   = 0;
	register unsigned long tag			= 0;
#if defined(__MEMBENCH_TEST__) && defined(__WIN32__) 
    LARGE_INTEGER starttime,endtime;
	  QueryPerformanceCounter(&starttime);
#endif

    assert( block_to_free );
  
    block_mem_pool =  *((unsigned long *)block_to_free  - 1);

    assert( (block_mem_pool >= 0) 
	         || (block_mem_pool <= MEM_MEMPOOLINDEX( BASE_POOL )));
    /* Point to our block info */
    block = (unsigned long *)block_to_free;
	  block -= MEM_MGTBLOCKSIZE( block_mem_pool );

	if ( !IS_RESERVED_BLOCK( block ) )
	   return;

    tag = TAG_SIZE( block );
    tag = SHED_HIGH_BIT( tag  );

    assert( tag <= MEM_MAXTAGSIZE( block_mem_pool ) );

	for (;;)
	{
	   buddy = get_buddy( block_mem_pool, tag, block );
      if ( !VALID( buddy ) || IS_RESERVED_BLOCK( buddy ) ) 
		 break;
   	  if (tag != TAG_SIZE( buddy ))
		break;
	
      /* Unlink the buddy from the tag list */
      prev_to_buddy = (unsigned long *)PREV_BLOCK( buddy );
      next_to_buddy = (unsigned long *)NEXT_BLOCK( buddy );
	  if ( prev_to_buddy  )
 	     NEXT_BLOCK( prev_to_buddy ) = (unsigned long)next_to_buddy;
	  if ( next_to_buddy  )
	     PREV_BLOCK( next_to_buddy ) = (unsigned long)prev_to_buddy;
      /* Update the Avail ptr */
	  if ( (unsigned long * )MEM_FREELIST(block_mem_pool,tag ) == buddy )
	     MEM_FREELIST(block_mem_pool,tag ) = (unsigned long)0;

	  tag++;
      if ( buddy < block )
		  block = buddy;
	}

    /* Re set the tag size of the newly formed block */
    TAG_SIZE( block ) = tag;

	  REQ_SIZE( block ) = 0;
    /* Free the block */
    MARK_FREE( block );

    /* Add the newly formed block to the appropriate Tag's list */
    /* We are re-using the buddy ptr */
    buddy  = (unsigned long *)MEM_FREELIST(block_mem_pool,tag );
    NEXT_BLOCK( block ) = (unsigned long)buddy;
    PREV_BLOCK( block ) = 0;
    if ( buddy )
      PREV_BLOCK( buddy ) = (unsigned long)block;
    /* Update the Avail ptr */
    MEM_FREELIST(block_mem_pool,tag ) = block;

#if defined(__MEMBENCH_TEST__) && defined(__WIN32__) 
		QueryPerformanceCounter(&endtime);
		printf("free = %ld\n", endtime.QuadPart-starttime.QuadPart);
#endif

}	

void * get_buddy( 
  IN  unsigned long pool_index,
  IN  unsigned long tag,
  IN  void * block
)
{
	register unsigned char *  buddy      = NULL;
	register unsigned char *  ptr_max     = (unsigned char *)MEM_MAINBLOCK( pool_index ) + 
		                       MEM_MAXBLOCKSIZE( pool_index );
  register unsigned long  tag_size  =  1 << ( tag  + 1);
	register unsigned long  buddy_index    = 0;
	register unsigned long  block_tag_size = 1 << tag;

    /* The original buddy calculation procedure the "xor" method 
	   is not used 
	 */
	if ( tag  <= MEM_MAXTAGSIZE( pool_index )  && tag > 0 )
	{
		buddy_index = (unsigned char *)block - (unsigned char *)MEM_MAINBLOCK( pool_index );
		if ( buddy_index % tag_size ) 
		{   buddy_index -= block_tag_size; }
		else
		{	buddy_index += block_tag_size; }
        buddy = (unsigned char *)MEM_MAINBLOCK( pool_index )  + (unsigned long) buddy_index;
	}

	if (( buddy > ptr_max ) || ( buddy < (unsigned char *)MEM_MAINBLOCK( pool_index ) ))
		buddy = NULL;

    return (void *)buddy;
}

/* Returns TRUE if the blocks tag is the same as the 
   called in tag */
int has_same_tag( 
  IN  unsigned int tag ,
  IN  void * block
)
{
	unsigned int block_tag = TAG_SIZE( block );
	block_tag = SHED_HIGH_BIT( block_tag );
	return block_tag == tag; 
}

/* Expands a given block only when the 
   whole block is marked free 
*/
int mem_expand_block(
  IN  unsigned long pool_index,
  IN  unsigned long size,
  IN  unsigned long tag_size
)
{
	unsigned long index = 1;
	unsigned long *block = MEM_MAINBLOCK ( pool_index );
	unsigned long tag  = TAG_SIZE ( block );

	tag = SHED_HIGH_BIT( tag  );

	assert( size    > 0 );
	assert( tag_size > 0 );

	/* If the full main block is free  we attempt a re allocate
	   to satisfy the request */
	if (( tag  != MEM_MAXTAGSIZE( pool_index )) || 
	( IS_RESERVED_BLOCK( block ) ))
		return 0;
	
    MEM_MAINBLOCK ( pool_index ) = realloc( block, size );
    if ( !MEM_MAINBLOCK ( pool_index ) )
   	   return 0;
    /* Update the new size  memset */
    MEM_MAXBLOCKSIZE( pool_index )  = size;
    MEM_AVAIL( pool_index ) = (unsigned long ** )realloc( MEM_AVAIL( pool_index ), 
		              tag_size * sizeof( unsigned long *));

    if ( !MEM_AVAIL( pool_index ) )
    {
	  free( MEM_MAINBLOCK ( pool_index ) );
	  MEM_MAINBLOCK ( pool_index ) = NULL;
	  return 0;
    }
    MEM_MAXTAGSIZE( pool_index ) = tag_size; 
    memset(MEM_AVAIL( pool_index ),0,tag_size * sizeof( unsigned long *));

    MEM_FREELIST(pool_index,tag_size) = MEM_MAINBLOCK ( pool_index );

    return 1;
}

/* Cleanup */
/* NON-re entrant */
/* Call it once from another thread */
void mem_exit()
{
	unsigned long cur_pool = BASE_POOL;
	unsigned long last_pool = 0;
  THREAD_CHECK(lock_acquire(&g_mem_lock);)

	assert( g_mem_info );
	last_pool = MEM_MEMPOOLINDEX ( BASE_POOL );
	for ( ; cur_pool <= last_pool ; cur_pool++ )
	{
		if ( MEM_MAINBLOCK( cur_pool ) ) 
			free( MEM_MAINBLOCK( cur_pool ));
		if ( MEM_AVAIL( cur_pool ) )
		    free( MEM_AVAIL( cur_pool ));
	}
	free( g_mem_info );
  g_mem_info = NULL;

  THREAD_CHECK(lock_release(&g_mem_lock);)
}


void * mem_calloc(
IN  unsigned long size
)
{
	void * p;
	p = mem_alloc( size );
	if (p)
	  memset( p, 0, size);
	return p;
}

void * mem_dup(
IN void * p )
{
  unsigned long block_mem_pool   = 0, old_req_size;
  unsigned long *block;
  void * new_block;
  /* check if it is a valid 'mem' ptr */
  if ( mem_is_valid_heap_ptr( p )) {
    block_mem_pool =  *((unsigned long *)p  - 1);
    assert( (block_mem_pool >= 0) 
	         || (block_mem_pool <= MEM_MEMPOOLINDEX( BASE_POOL )));
    block = (unsigned long *)p;
	  block -= MEM_MGTBLOCKSIZE( block_mem_pool );
	  if ( !IS_RESERVED_BLOCK( block ) )
	    return NULL;
    old_req_size = REQ_SIZE( block ); 
    new_block = mem_calloc(old_req_size);
    memcpy(new_block, block, old_req_size );  
    mem_free( p );
  } else {
    new_block = mem_calloc(strlen(p) + 1);
    strcpy(new_block, p);
  }
 
  return new_block;
}


void * mem_realloc( 
  IN  void * p,
  IN  unsigned long new_size
)
{
    unsigned long block_mem_pool   = 0;
  	void * new_block       = NULL;
    unsigned long * block       = NULL;
	  unsigned long  old_req_size    = 0;

    if (!p)
		  return mem_alloc( new_size );

    block_mem_pool =  *((unsigned long *)p  - 1);

    assert( (block_mem_pool >= 0) 
	         || (block_mem_pool <= MEM_MEMPOOLINDEX( BASE_POOL )));
    /* Point to our block info */
    block = (unsigned long *)p;
	  block -= MEM_MGTBLOCKSIZE( block_mem_pool );
	  if ( !IS_RESERVED_BLOCK( block ) )
	    return NULL;

    /* CHECK,CHECK : Currently we do not do the optimization where by 
       if the new_size is less than old_req_size then we 
       should just reuse the current block rather than 
       allocate a new one. Also if the new_size is lesser than the
       'actual' allocated size which will be a power of two then we don't have to 
       allocate any thing new at all. */

	  old_req_size = REQ_SIZE( block ); 
	  new_block = mem_calloc( new_size );
    if ( old_req_size <= new_size )
	    memcpy(new_block, p, old_req_size );
    else
      memcpy(new_block, p, new_size);
    
	  mem_free( p );
	  return new_block;
}


int mem_is_valid_heap_ptr( void * address )
{
  /* TODO */
	return 0;
}

/* Dump facility for Debugging
   @TODO: Change fprintfs to equivalent */
void mem_dump()
{
    long pool_index = MEM_MEMPOOLINDEX( BASE_POOL );
    unsigned long req_tag = 0;
    unsigned long index = 1;

    for ( ;pool_index >= 0; pool_index--) {
 	    req_tag = MEM_MAXTAGSIZE( pool_index ); 
      fprintf(stdout,"Pool %ld --\r\n", pool_index);
      for ( index = 1; index <= req_tag; index++ )
      {
	      unsigned long *block = 
           (unsigned long *)MEM_FREELIST( pool_index , index );
	      if ( !block ) continue;
	      while ( block )
	      {
	 	      fprintf( stdout,"2^%ld ,",index);
	 	      block = (unsigned long *)NEXT_BLOCK( block );
	      }
          fprintf( stdout,"\r\n");
          getchar();
	    }
         fprintf( stdout,"\r\n");
    }
}
