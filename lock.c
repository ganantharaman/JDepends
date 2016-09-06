
#include <lock.h>

LOCK sys_create_lock() {  
	CRITICAL_SECTION  * cs = malloc( sizeof(CRITICAL_SECTION));
	InitializeCriticalSection( cs );
	return cs;
}


void sys_acquire_lock( LOCK lock )
{
   EnterCriticalSection( lock );
}

void sys_release_lock( LOCK lock )
{
   LeaveCriticalSection( lock );
}

void sys_destroy_lock( LOCK lock )
{
	DeleteCriticalSection( lock );
	free( (void *)lock );
}