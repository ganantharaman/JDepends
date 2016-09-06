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
#ifndef __LOCK__HEADER__
#define __LOCK__HEADER__
#include <windows.h>

typedef CRITICAL_SECTION  lock_t;
/* Local ( one process  ) locks */
#define LOCK_STATUS_VALID 1
#define LOCK_STATUS_INVALID 0

void lock_init(lock_t * lock);
void lock_acquire( lock_t * lock );
void lock_release( lock_t * lock );
void lock_destroy( lock_t * lock );

/* TODO : thread apis mapping */

#endif /* __LOCK__HEADER__ */


