/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/

#ifndef __POOL_HDR_INCLUDE__
#define __POOL_HDR_INCLUDE__

enum {
 PF_COND_CHECK = 0x00000001
};

typedef union pool_data{
  void     * p;
  handle_t   h;
} pool_data_t;

typedef struct pool_info  {
#ifdef __DEBUG__
  char  name[256];
#endif
  /* start count */
  unsigned long init_count;
  /* maximum count */
  unsigned long max_count;
  /* pool policies */
  unsigned int  flags;
  /* reference */
  void * ref;
  /* initialize data */
  void * (* pfn_create_data)(void * ref, void * pool);
  /* clean up  data  */
  void (* pfn_cleanup_data)(void *);
  /* compare data    */
  int  (* pfn_compare_data)(void *, void *);  
} pool_info_t;



typedef struct pool {
  /* lock securing list operations */
#ifdef __THREAD__
  lock_t      lock;
  /* condition variable  for 'data available' predicate */
  cond_t      cond; 
#endif
  /* used list - has  a list of pool_data currently in use */
  link_list_t * used_list;
  /* free list - has  a list of pool_data currently free */
  link_list_t * free_list;
  /* current count */
  unsigned long count;
  /* pool data */
  pool_info_t   info;
} pool_t;


int pool_init();
pool_t * pool_create(pool_info_t * pdata);
pool_data_t * pool_get_data( pool_t * pool );
void  pool_return_data( pool_t * pool, pool_data_t * p );
void  pool_destroy_data( pool_t * pool, pool_data_t * p );
void pool_destroy( pool_t * pool );


#endif /* __POOL_HDR_INCLUDE__ */


