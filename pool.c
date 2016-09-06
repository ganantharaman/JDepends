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

#include <linklist.h>
#include <cmnmcros.h>
#include <lock.h>
#include <mem.h>
#include <cond.h>
#include <pool.h>

/** Create a new pool **/
pool_t * pool_create(pool_info_t * pdata)
{
   unsigned long i;
   pool_t * pool = mem_alloc(sizeof(pool_t));
   if ( pool ) {
     memset(&pool->info, 0, sizeof(pool_info_t));
    pool->info = *pdata;
#ifdef __THREAD__     
     lock_init(&pool->lock);
     cond_init(&pool->cond);
#endif
     pool->used_list = link_list_create(
#ifdef __DEBUG__
                                "used_list",
#endif
                    			       LINK_LIST_QUEUE, LINK_DATA_TYPE_PTR, sizeof( PVOID ), 
				                        pool->info.pfn_compare_data,pool->info.pfn_cleanup_data
                                 );
     pool->free_list = link_list_create(
#ifdef __DEBUG__
                                "free_list",
#endif
                    			       LINK_LIST_QUEUE, LINK_DATA_TYPE_PTR, sizeof( PVOID ), 
				                        pool->info.pfn_compare_data,pool->info.pfn_cleanup_data
                                 );

     assert( pool->free_list && pool->used_list );
     pool->count =pool->info.init_count;
     if (pool->count) {
       /* create initial items */
       for (i = 0; i < pool->count; i++)
            link_list_add_data( pool->free_list,
            pool->info.pfn_create_data(pool->info.ref, pool));
     }
     
   }

   return pool;
}

/* Next type in pool */
pool_data_t * pool_get_data( pool_t * pool )
{
  pool_data_t * p;  
  if (!pool) return NULL;
  THREAD_CHECK( lock_acquire(&pool->lock);)

  if ( 1 == link_list_get_next_data_ex( pool->free_list, &p)) {
    link_list_move_node( pool->free_list, pool->used_list, p);
  } else {
    /* count reached maximum? */
    if ( pool->count ==pool->info.max_count ) {
#ifdef __THREAD__
       if (pool->info.flags & PF_COND_CHECK ) {
          for (;;) { 
            /* wait for 'data available' condition */
            if ( 1 == link_list_get_next_data_ex( pool->free_list, &p)) {
               link_list_move_node( pool->free_list, pool->used_list, &p);
               break;
            }
            cond_wait( &pool->cond, &pool->lock, 0 );
          }
       } else 
#endif
        p = NULL; /* no more */
    } else {  /* create a new data */
      p =pool->info.pfn_create_data(pool->info.ref, pool);
      link_list_add_data( pool->free_list, p);       
      ++ pool->count;
    }
  }

  THREAD_CHECK( lock_release(&pool->lock); )
  return p;
}

/* Remove a type */
void  pool_return_data( pool_t * pool, pool_data_t * p )
{
  THREAD_CHECK( lock_acquire(&pool->lock); )
  /* Move node from used to free list */
  link_list_move_node(pool->used_list, pool->free_list, p);
#ifdef __THREAD__  
  if ( pool->count ==pool->info.max_count ) 
      cond_signal(&pool->cond);
  THREAD_CHECK( lock_release(&pool->lock);)
#endif  
}


void  pool_destroy_data( pool_t * pool, pool_data_t * p )
{
  THREAD_CHECK(lock_acquire(&pool->lock);)
  /* Move node from used to free list */
  if ( link_list_find_data(pool->free_list, &p) ) {
    link_list_delete_data(pool->free_list, &p);
    --pool->count;
  }

  THREAD_CHECK(lock_release(&pool->lock);)
}


/* Destroy the pool and its contents */
void pool_destroy( pool_t * pool )
{
#ifdef __THREAD__
  lock_destroy(&pool->lock);
  cond_destroy(&pool->cond);
#endif
  link_list_destroy(pool->used_list);
  link_list_destroy(pool->free_list);
  mem_free( pool );  
}

















