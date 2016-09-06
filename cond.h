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

#ifndef __COND__HEADER__
#define __COND__HEADER__

typedef struct event_thread_map {
   unsigned long id;
   handle_t      event;
} event_thread_map_t;

typedef struct cond {
  int      chunk_size;
  long     allocated_chunks;
  long     count;
  lock_t   lock;
  event_thread_map_t * map;
} cond_t;

void cond_init(cond_t * cond);
void cond_destroy(cond_t * cond);
void cond_wait(cond_t * cond, lock_t * lock, unsigned long millisec);
void cond_signal(cond_t * cond);
void cond_broadcast(cond_t * cond);

#endif /* __COND__HEADER__ */


