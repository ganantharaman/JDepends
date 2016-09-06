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
#include <windows.h>
#include <mem.h>
#include <osapi.h>
#include <hashtabl.h>
#include <cmnmcros.h>

#define STDIO_H
#include <stdhdrs.h>

/*
 *
 *  WARNING, WARNING : THREAD_CHECK all locks and conds. This is not done yet
 *
 */ 

extern enum NUM_ENV_VARS;

char *env_names[] = {
	"classpath",
	"path"
};

/* Global Extern 
*/


/* Internal */
int _os_thread_compare( void * thread, void * id );

DWORD WINAPI _os_worker_thread_start_routine( LPVOID lpParam );

void _os_cache_rundown( void * p );

void _os_threads_rundown( void * p );

void _os_message_rundown( void * msg );

int _os_cache_compare( void * p1, void * p2 );

int _os_message_compare( void * p1, void * p2 );

int _os_event_compare(void * p1, void * p2);

pool_data_t * _os_event_create();

void _os_clean_cache( os_thread_msg_t * arg );



/* One and only global object */
os_globals_t * _gp;

os_globals_t * _os_get_globals()
{  return _gp;  }

/* First ever call */
int  _os_init()
{
   /* TODO : setup env_names **/
   unsigned long err = 0L;
   int status = 0;
   os_thread_t * main_thd = NULL;
   queue_info_t * queue_info;
   pool_info_t pi;
#ifdef __DEBUG__
   char name[ 256 ];
   strcpy( name , "message_list_");
#endif
   if ( !mem_init()) return 0;
   if((_gp = mem_alloc( sizeof(os_globals_t) )) == NULL)
       return 0;
   
   /* Create a unowned mutex */
   lock_init(&_gp->thd_lock);
 	 _gp->thd_list = link_list_create( 
#ifdef __DEBUG__
			 "ThreadList",
#endif
			 LINK_LIST_REGULAR,
			 LINK_DATA_TYPE_PTR,
			 sizeof( void * ),  
			_os_thread_compare,
			_os_threads_rundown
		);
   _gp->env_names = env_names;
#ifdef __DEBUG__
  strcpy(pi.name, "EventPool");
#endif  
   pi.init_count = 2;
   pi.max_count = 100;
   pi.flags  = PF_COND_CHECK;
   pi.ref = NULL;
   pi.pfn_create_data = _os_event_create;
   pi.pfn_cleanup_data = _os_event_destroy;
   pi.pfn_compare_data = _os_event_compare;
   _gp->epool = pool_create(&pi);

   _gp->inited = 0;
   cond_init(&_gp->inited_cond);
   lock_init(&_gp->init_lock);

   if (!_gp->epool)
       TRACE_PRINT("Warning: _os_init : event pool creation failed");

   if ( !dict_create(
#ifdef __DEBUG__  
     "event_thd_dict",
#endif
      HASH_TYPE_INT,
      HASH_TYPE_INT,
      NULL,
      &_gp->event_thd_dict))
         TRACE_PRINT("Warning: _os_init : event_thd_dict creation failed");    

	 if ( _gp->thd_list )	{
		/** Create an entry for self **/
		main_thd = mem_alloc( sizeof( os_thread_t ));
		 if ( main_thd )	{
			main_thd->thread = GetCurrentThread();
			main_thd->id     = GetCurrentThreadId();
      main_thd->name   = "main_thread";
#ifdef __DEBUG__            
			strcat( name, main_thd->name );
#endif
      main_thd->qinfo = queue_create(
#ifdef __DEBUG__
				  name,
#endif
				  _os_message_compare, _os_message_rundown,
          _os_copy_message);
  	  	if (main_thd->qinfo) {
  				link_list_add_data( _gp->thd_list, main_thd );
          _gp->main_thd = main_thd;
           /* create 2 worker thread to clean cache */
				   status = _os_create_thread( NULL, 0, "cache_cleaner_crew",
			                   &_gp->cache_thd, NULL, &err );
           /* with shared queue */ 
           if ( status ) {
                assert(_gp->cache_thd && _gp->cache_thd->qinfo );
                /* increment queue usage count */
                queue_addref(_gp->cache_thd->qinfo);  
                queue_info = _gp->cache_thd->qinfo;
                status = _os_create_thread( NULL, 0, "cache_cleaner_crew",
			                    &_gp->cache_thd, queue_info, &err );
           }
        };
		  }
		}
   return status; 
}




/************* Thread Related OS APIs *****************/

int _os_wait_on_mutex( handle_t mutex, unsigned long * reason, unsigned long * err )
{
  *err = OS_SUCCESS_STATUS;
  if ( mutex != NULL ) {
    if ((*reason = WaitForSingleObject( mutex,INFINITE)) != WAIT_FAILED )
		   return 1;
  }
  *err  = GetLastError();
  return 0;
}

int _os_release_mutex( handle_t mutex, unsigned long * err )
{
   *err = OS_SUCCESS_STATUS; 	 
   if ( ReleaseMutex( mutex ) );
	    return  1;
   *err  = GetLastError();
   return 0;
}

os_thread_t * _os_get_current_thread()
{	
	unsigned long id;
	os_thread_t * thd = NULL;
  os_globals_t * globals = _os_get_globals();
  link_list_t * list  = globals->thd_list;
  lock_acquire(&globals->thd_lock);  
	id = GetCurrentThreadId();

	if ( globals->thd_list)
		thd = (os_thread_t * )link_list_find_data( globals->thd_list, &id );

  lock_release(&globals->thd_lock);      
	return thd;
}

os_thread_t * _os_get_thread_from_name( char * name )
{
	os_thread_t * thd = NULL;
  os_globals_t * globals =  _os_get_globals();
  link_list_t * list  = globals->thd_list;
  /* acquire thd lock */
  lock_acquire(&globals->thd_lock);
	if (list)	{
    /* Iterate thro thd_list and check if the passed in name 
       matches any existing ones  */
		link_list_get_first_data( list, &thd );
		do {
      /* if same , break */
		  if ( thd  && !stricmp( thd->name, name ))
		  	 break;
			else
				thd = NULL;
		}	while (link_list_get_next_data(list, &thd));
	}

  /* release */
  lock_release(&globals->thd_lock);
	return thd;
}


int _os_create_thread( pfn_job_routine_t pfn_start_routine,void * param, char * name,
				os_thread_t ** thd, queue_info_t * queue_info, unsigned long * err ) 
{
   unsigned long id = 0L;
   handle_t thread;
   os_globals_t * globals = _os_get_globals();
   link_list_t * list  = globals->thd_list;
   os_thread_msg_t * msg;
#ifdef __DEBUG__
   static char lst_name[200];
#endif
   *err = OS_SUCCESS_STATUS; 
   *thd = mem_alloc( sizeof( os_thread_t ));

#ifdef __DEBUG__
   strcpy( lst_name, "MessageList_");
#endif

   if ( *thd ) {
	   if ((thread = CreateThread(NULL,0,
			   (LPTHREAD_START_ROUTINE)_os_worker_thread_start_routine,
			     param,CREATE_SUSPENDED,&id )) != NULL)  {

         (*thd)->thread  = thread;
	   	   (*thd)->id      = id;
		     (*thd)->name    = name;
#ifdef __DEBUG__
		  strcat( lst_name, name );
#endif
      if (!queue_info) {
		     (*thd)->qinfo  = queue_create(
#ifdef __DEBUG__
			                            lst_name,
#endif
	                              _os_message_compare, _os_message_rundown,
                                _os_copy_message 
		                          );
      } else {  /* share the message queue with some other thread */
         (*thd)->qinfo = queue_info;
      }   

		  if ((*thd)->qinfo)
		  {
        /* Add the thread to the thread list */ 
	      lock_acquire(&globals->thd_lock);
	      if ( globals->thd_list )
  	      link_list_add_data( globals->thd_list, *thd );
			  lock_release(&globals->thd_lock);

        /* Add Work information message, if there is a given start routine */
        if ( pfn_start_routine ) {
      	  msg = _os_create_thread_message();
          if ( msg ) {
		       msg->_data_type		 = data_type_int;
		       msg->u._int_value	 = TM_EXEC;
		       msg->do_work        = pfn_start_routine;
		       msg->args           = (void *)param;
		       msg->_post_back     = 0;
		       msg->_t_sender      = _os_get_current_thread();
		       msg->_t_receiver    = *thd;
           _os_post_thread_message(msg);
	        } else {
            TRACE_PRINT("Warning : _os_create_thread : _os_create_thread_message failed"); 
          }
        }
        /* start the thread */
			  ResumeThread( thread );
			  return 1;
		  }
	   }
   }
   *err = GetLastError();
   return 0;
}

unsigned long _os_get_error()
{  return GetLastError();  }

void _os_get_error_message( unsigned long err_code, os_cache_t * cache, char ** str )
{
	void * p;
 	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&p,
		0,
		NULL 
 	);
	*str = mem_dup( p );
  if ( *str) {
	  if ( cache )
	   	link_list_add_data( cache, *str );
  }
  if ( p ) LocalFree( p );
}

int _os_set_thread_priority(os_thread_t * thd, unsigned int priority, unsigned long * err )
{
  int thd_priorities[] = {
	   THREAD_PRIORITY_LOWEST,
     THREAD_PRIORITY_LOWEST,
	   THREAD_PRIORITY_LOWEST,
	   THREAD_PRIORITY_BELOW_NORMAL,
	   THREAD_PRIORITY_BELOW_NORMAL,
	   THREAD_PRIORITY_NORMAL,
	   THREAD_PRIORITY_ABOVE_NORMAL,
	   THREAD_PRIORITY_ABOVE_NORMAL,
	   THREAD_PRIORITY_ABOVE_NORMAL,
	   THREAD_PRIORITY_HIGHEST,
	   THREAD_PRIORITY_HIGHEST
  }; 
  assert( thd != NULL );
  *err = OS_SUCCESS_STATUS; 
  if ( SetThreadPriority(thd->thread, thd_priorities[ priority % 11 ]) )
        return 1;
  *err = GetLastError();
  return 0;
}

/* For symmetry */
/* Currently mapped only to JVM priority levels */
/* TODO : modify for other mappings , mapping typically should have a
   bunch of indices which should be mapped to win32 thd priority */
int _os_get_thread_priority(os_thread_t * thd, int * priority, unsigned long * err) 
{
   *err = OS_SUCCESS_STATUS; 
   if (( *priority = GetThreadPriority(thd->thread))
         != THREAD_PRIORITY_ERROR_RETURN ) {
	     /** Priority mappings **/
       switch ( *priority ) {
		     case THREAD_PRIORITY_LOWEST :         *priority = 1;  break;
		     case THREAD_PRIORITY_BELOW_NORMAL :   *priority = 3;  break;
		     case THREAD_PRIORITY_NORMAL :         *priority = 5;  break;
		     case THREAD_PRIORITY_ABOVE_NORMAL :   *priority = 6;  break;
		     case THREAD_PRIORITY_HIGHEST  :       *priority = 10; break;
		     default: 						                 *priority = 5;  break;
		   }
		   return 1;
   }

   *err = GetLastError();
   return 0;
}

/* preparation before suspension */
int _os_prepare_to_suspend_thread( os_thread_t * thd, unsigned long * err )
{
   /* Create a manual-reset,non-signalled event */
   handle_t event = CreateEvent(NULL,TRUE,FALSE,NULL);
   dict_t * dict  =  _os_get_globals()->event_thd_dict;
   *err = OS_SUCCESS_STATUS;

   if ( event ) {
	   /* Set the event state to non-signalled */
     if ( ResetEvent( event ) ) {
         thd->htkey.u.ul = thd->id;
		     thd->htval.u.ul = (unsigned long)event;
         dict_add(dict, thd->htkey, thd->htval);
		     return 1;
	   }
   }

   *err = GetLastError();
   return 0;
}

/* suspend a thread */
int _os_suspend_thread( os_thread_t * thd, unsigned long millisec, unsigned long * err )
{
	handle_t event;
  dict_t * dict =  _os_get_globals()->event_thd_dict;
	thd->htkey.u.ul = thd->id;
	thd->htval	= dict_get(dict,thd->htkey);
	event = (handle_t)thd->htval.u.ul;
  *err  = OS_SUCCESS_STATUS;

	millisec = (millisec == 0) ? INFINITE :
	             millisec;
	if ( event ) {
		if ( thd->id == GetCurrentThreadId() ) {
			if ( WAIT_FAILED != WaitForSingleObject( event,millisec))
	       		return 1;  
		}
	}

	*err  = GetLastError();
  return 0;
}

/* NON Re-entrant */ /* CHANGE THIS */
int _os_suspend_thread_cleanup( os_thread_t * thd, unsigned long * err )
{
	/* When we wake up delete the event */
	handle_t event;
  dict_t * dict =  _os_get_globals()->event_thd_dict;
	thd->htkey.u.ul = thd->id;
	thd->htval	= dict_get(dict,thd->htkey);
	event = (handle_t)thd->htval.u.ul;

  *err  = OS_SUCCESS_STATUS;
	if ( event ) {
		if ( CloseHandle( event ) )	{
       	dict_remove( dict,thd->htkey);
        return 1;	 
		}
	}

  *err  = GetLastError();
  return 0;
}

/* Re-entrant */  /* CHANGE THIS*/
int _os_resume_thread(os_thread_t * thd, unsigned long * err)
{
 	handle_t event;
	thd->htkey.u.ul = thd->id;
	thd->htval	= dict_get(_os_get_globals()->event_thd_dict,thd->htkey);
	event = (handle_t)thd->htval.u.ul;
	*err  = OS_SUCCESS_STATUS;

	/* Set the event state to signalled */
	if ( event &&  SetEvent( event ) )
       return 1;      

	*err  = GetLastError();
  return 0;
}


/* CHANGE THIS*/
int _os_destroy_thread( os_thread_t * thd, unsigned long * err  )
{
	handle_t  thread = NULL;
	int status;
  os_globals_t * globals = _os_get_globals();
  assert( thd != NULL ); 
  *err  = OS_SUCCESS_STATUS;

  lock_acquire(&globals->thd_lock);   
	thread = thd->thread;
	if ( globals->thd_list )
	  link_list_delete_data( globals->thd_list, thd );
	/** avoid using this call , instead use _OS_ExitThread **/
  status =  TerminateThread( thread , 0);
	lock_release(&globals->thd_lock);
	if ( status )	return 1;

  *err  = GetLastError();
  return 0; 
}

void _os_exit_thread()
{
	DWORD exit_code = 0;
	DWORD id = GetCurrentThreadId();
	os_thread_t * thd = NULL;
  os_globals_t * globals = _os_get_globals();

  /** Get the exit code **/
	GetExitCodeThread( GetCurrentThread(), &exit_code );
  lock_acquire(&globals->thd_lock);   

	if ( globals->thd_list ) {
		thd = (os_thread_t *)link_list_find_data( globals->thd_list, &id );
		link_list_delete_data( globals->thd_list, thd );
	}

	lock_release(&globals->thd_lock);
 	/** pass it on **/
	ExitThread( exit_code );
}


int  _os_create_mutex(handle_t * mutex, unsigned long *  err)
{
   *err  = OS_SUCCESS_STATUS;
   /* Create a unowned mutex */
   if ( (*mutex = CreateMutex(NULL,FALSE,NULL)) != NULL)
	      return 1;      
   *err  = GetLastError();
   return 0; 
}

int _os_destroy_mutex(handle_t mutex, unsigned long * err)
{ 
   return _os_handle_close( mutex, err );    
}

void _os_sleep(unsigned long millisec)
{	Sleep( millisec );    }


UINT _os_get_random()
{	return (UINT)rand();   }

/** Memory related OS API implementation **/
/* TODO, to be rewritten to use Win32 Globalalloc etc. 
   WARNING : users shouldn't check Error code  now*/

int _os_malloc( size_t size, void ** mem, unsigned long * err)
{	
  *err  = OS_SUCCESS_STATUS;  
	if ((*mem = malloc( size )) != NULL)
    return 1;
  *err = ERROR_NOT_ENOUGH_MEMORY;
  return 0;
}

int _os_calloc( size_t size, void ** mem, unsigned long * err)
{   
  *err  = OS_SUCCESS_STATUS;  
	if ((*mem = calloc( size ,sizeof( BYTE ))) != NULL)
    return 1;
  *err = ERROR_NOT_ENOUGH_MEMORY;
  return 0;
}

void _os_free( void * p )
{   free( p );  }


int _os_realloc(void * old, size_t size,  void ** mem, unsigned long * err)
{
  *err  = OS_SUCCESS_STATUS;  
	if ((*mem = realloc(old, size)) != NULL ) {
    return 1;
  }
  *err = ERROR_NOT_ENOUGH_MEMORY;
  return 0;
}

/* system exit */
void _os_exit()
{
   unsigned long err   = 0L;
   os_thread_t * this_thd = _os_get_current_thread();
   os_thread_t * thd = NULL;
   os_thread_t **threads;
   handle_t * thread_handles = NULL;
   int k, count = 0;
   os_globals_t * globals = _os_get_globals();

    /* acquire global thread_list's  lock */
    lock_acquire( &globals->thd_lock );
    /* create an array of handles */
    count = 0;
   	threads = mem_calloc( sizeof(os_thread_t *) * globals->thd_list->count);
 	  if ( threads ) {
      /* Iterate through the thread list and send TM_EXIT message */
      link_list_get_first_data(globals->thd_list, &thd);
	    do  {
		    if ( thd == NULL) break;
		    if ( thd != this_thd ) {
			     threads[ count++ ] = thd;
		    }
	    } while (link_list_get_next_data(globals->thd_list, &thd));
	  }
   /* remove list lock */
   lock_release(&globals->thd_lock);
   
   /* Post exit message */
   thread_handles = mem_calloc(sizeof(handle_t) * count);
   for(k = 0; k < count; k++) {
      thread_handles[k] = threads[k]->thread;
      _os_post_exit_thread_message(this_thd, threads[k]);
   }
  
   /* Wait for all the threads to exit */
   WaitForMultipleObjects( count, thread_handles, TRUE, INFINITE );
   
   /* OK. It is possible that new threads have been created now.
      We are not sending the TM_EXIT message to them yet.
      And we are just not bothering about them now. 
      This is obviously a bug and has to be fixed. 
      I've set the time to be tomorrow. For now We just
      go ahead with deleting the list.
     */

   /* lock thd_lock again */
   lock_acquire( &globals->thd_lock );
   

   /* delete the thd_list */
   if ( globals->thd_list ) { 
      link_list_destroy( globals->thd_list ); 
      globals->thd_list = NULL;
   }
    /* release lock */
   lock_release( &globals->thd_lock );
    /* delete dictionary */
   dict_destroy( globals->event_thd_dict );
   /* destroy lock */
   lock_destroy(&globals->thd_lock);
   /* free local allocated memory */ 
   mem_free( threads );
   mem_free( thread_handles );
   globals->event_thd_dict = NULL;
   mem_exit();
}


int _os_open_file( char * file_name, unsigned int mode, handle_t * file, unsigned long * err )
{
	*err = OS_SUCCESS_STATUS;
  /**  Assumes UINT == unsigned long , as it is in win32 **/
	if ( mode == OS_FILE_READ_ONLY )
		mode = GENERIC_READ;
	else if ( mode == OS_FILE_WRITE_ONLY )
		mode = GENERIC_WRITE;
	else
    	mode = GENERIC_WRITE | GENERIC_READ;
	if ( (*file = CreateFile( file_name, mode, 
		     FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		     FILE_ATTRIBUTE_NORMAL, NULL )) != INVALID_HANDLE_VALUE )
      return 1;	
	*err = GetLastError();
	return 0;
}

int _os_read_file( handle_t file, unsigned char ** bytes_to_read, unsigned long count, 
				   unsigned long * actual_bytes_read_count,  unsigned long * err )
{
  *err = OS_SUCCESS_STATUS;
	if ( file ) {
		if ( ReadFile ( file, *bytes_to_read, count, 
  			 actual_bytes_read_count, NULL ))
	 		return 1;
	}
	*err = GetLastError();
	return 0;
}


int _os_write_file( handle_t file, unsigned char *  bytes_to_write, unsigned long count,
					unsigned long * bytes_written_count, unsigned long * err )
{
    *err = OS_SUCCESS_STATUS;
	  if ( file ) {
		  if ( WriteFile( file, bytes_to_write, 
			          count, bytes_written_count, NULL ) )
 	       return 1;
	  }
	  *err = GetLastError();
	  return 0;
}

int _os_close_file( handle_t file, unsigned long * err )
{
    return _os_handle_close( file, err );
}

int _os_get_file_size( handle_t file, unsigned long * file_size, unsigned long * err )
{
   *err = OS_SUCCESS_STATUS;
   if ( (*file_size = GetFileSize( file, NULL )) != 0xffffffff )
        return 1;   
   *err = GetLastError();
   return 0;
}

int _os_load_library( char * lib_name, handle_t * lib, unsigned long * err )
{
	*err = OS_SUCCESS_STATUS;
	if ((*lib = LoadLibrary( lib_name )) != NULL )
		return 1;
 	*err = GetLastError();
	return 0;
}

int _os_free_library( handle_t lib, unsigned long * err )
{
	*err = OS_SUCCESS_STATUS;
	if ( FreeLibrary( lib ) )
		return 1;
  *err = GetLastError();
	return 0;
}

int _os_link_function( char * func_name, handle_t lib, void ** func, unsigned long * err )
{
  *err = OS_SUCCESS_STATUS;
	if ((*func = GetProcAddress( lib, func_name )) != NULL)
		return 1;
  *err = GetLastError();
	return 0;
}

__inline  int  _os_handle_close( handle_t hdl, unsigned long * err )
{
  *err = OS_SUCCESS_STATUS;
	if ( CloseHandle( hdl ) )
		return 1;
	*err  = GetLastError();
  return 0;
}


int _os_get_path_mark( char ** str, os_cache_t * cache )
{
   *str = mem_dup("\\");
   if ( *str ) {
     if ( cache )
		   link_list_add_data( cache, *str ); 
	   return 1;
   }
   return 0;
}

char * _os_get_env_name_from_id(int index )
{
	return ( index < NUM_ENV_VARS ) ? 
     _os_get_globals()->env_names[index] : NULL;
}

int _os_get_env( int index, char ** str, unsigned long * err )
{
  *err  = OS_SUCCESS_STATUS;
  if ( index < NUM_ENV_VARS )
 		*str = getenv( _os_get_globals()->env_names[index] );
	if ( *str )
		return 1;
	*err  = GetLastError();
  return 0;
}

int _os_get_current_dir( char ** dir, os_cache_t * cache, unsigned long * err )
{ 
  *err  = OS_SUCCESS_STATUS;
  *dir  = mem_alloc( MAX_PATH + 1 );
	if ( *dir ) {
		if ( cache )	link_list_add_data( cache, *dir );
		if ( GetCurrentDirectory( MAX_PATH + 1, *dir ))
			return 1;
	}
  *err  = GetLastError();
  return 0;
}

/** Thread Message  related APIs **/
os_thread_msg_t * _os_create_thread_message()
{
   os_thread_msg_t * msg = NULL; 
   msg = mem_calloc(sizeof(os_thread_msg_t));
   /*TODO : Init msg here */
   return msg;
}

int _os_post_exit_thread_message(os_thread_t * sender, os_thread_t * receiver)
{
  os_thread_msg_t * msg = _os_create_thread_message();
  if ( msg ) {
       msg->_data_type		= data_type_int;
       msg->u._int_value	= TM_EXIT;
       msg->_t_sender     = sender;
       msg->_t_receiver   = receiver;
  }
  _os_post_thread_message(msg);
  return 0;
}

int _os_post_execute_thread_message(os_thread_t * receiver, int post_back, 
                                 pfn_job_routine_t job, void * args)
{
     os_thread_msg_t * msg = _os_create_thread_message();
     if ( msg ) {
       msg->_data_type		 = data_type_int;
	     msg->u._int_value	 = TM_EXEC;
	     msg->do_work        = job;
	     msg->args           = args;
	     msg->_post_back     = post_back;
	     msg->_t_sender      = _os_get_current_thread();
	     msg->_t_receiver    = receiver;
       _os_post_thread_message(msg);
     }
     return 0;
}

int _os_post_thread_message(os_thread_msg_t * msg)
{
	register os_thread_t * receiver;
  if ( !msg ) return 0;
  receiver = msg->_t_receiver;
  /* if there is a receiver */
	if ( receiver ) {
     if ( queue_add(receiver->qinfo, msg) ) 	 
        return 1;
	}
  return 0;
}



int _os_get_next_thread_message( os_thread_t * thd, os_thread_msg_t * msg )
{
   assert( thd != NULL);
   assert( msg != NULL);
   memset( msg, 0, sizeof( os_thread_msg_t ));
   msg->_t_sender    = NULL;
   msg->_t_receiver  = NULL;
   
   if ( queue_get( thd->qinfo, msg ) )
        return 1;
   return 0;
}


int _os_cache_init(os_cache_t ** cache )
{
  /** _OS_CACHE is same as link_list_t **/
  if ((*cache = link_list_create(
#ifdef __DEBUG__
	  "cache_list",
#endif
	   LINK_LIST_REGULAR, LINK_DATA_TYPE_PTR, sizeof(void*), 
	   _os_cache_compare, _os_cache_rundown)) != NULL 
   )
	  return 1;
  
  TRACE_PRINT("Warning: _os_cache_init failed on link_list_create");
  return 0;
}

/* Flush cache */
void _os_cache_flush( os_cache_t * cache )
{
	os_thread_msg_t * msg = _os_create_thread_message();
  if ( msg ) {
		msg->_data_type		= data_type_int;
		msg->u._int_value	= TM_EXEC;
		msg->do_work      = _os_clean_cache;
		msg->args         =  cache;
		msg->_post_back   = 0;
		msg->_t_sender    = _os_get_current_thread();
		msg->_t_receiver  = _os_get_globals()->cache_thd;
	}
	_os_post_thread_message(msg);
}

void * _os_copy_message(void * p1, void * p2 )
{
   os_thread_msg_t * from_msg, * to_msg;
   from_msg = (os_thread_msg_t *)p1;
   to_msg   = (os_thread_msg_t *)p2;
   if ( to_msg && from_msg ) {
      to_msg->u._int_value  = from_msg->u._int_value;
      to_msg->u._long_value = from_msg->u._long_value;
      to_msg->u._ptr_value  = from_msg->u._ptr_value;
      to_msg->do_work       = from_msg->do_work;
      to_msg->args          = from_msg->args;
      to_msg->result        = from_msg->result;
      to_msg->_post_back    = from_msg->_post_back;
      to_msg->_data_type    = from_msg->_data_type;
      to_msg->_t_receiver   = from_msg->_t_receiver;
      to_msg->_t_sender     = from_msg->_t_sender;
   }
   return (void *)to_msg;
}


/** LinkList Callbacks  **/
int _os_thread_compare( void * p, void * id )
{
   os_thread_t ** thd = (os_thread_t **)p;
   return  (*(unsigned long *)id) == (*thd)->id;
}

void _os_threads_rundown( void * p )
{
   os_thread_t * thd = *(os_thread_t **)p;
   if ( thd ) {
     /* TerminateThread( thd->thread, 0 ); */
     queue_removeref(thd->qinfo);
     mem_free( thd );
   }
   *(os_thread_t **)p = NULL;
}

int _os_is_exit_message( os_thread_msg_t * msg )
{
   os_thread_t * this_thd = _os_get_current_thread();
   /* Not comparing receiver id to my id. Mainly because we 
      may share the queue with some other thread */
   if ( msg->_t_sender  &&  msg->_data_type == data_type_int )
		  return (msg->u._int_value == TM_EXIT);
   return 0;
}

int _os_message_compare( void * p1, void * p2 )
{
	 os_thread_msg_t * msg = *(os_thread_msg_t **)p1;
	 unsigned long msg_id = *(unsigned long *)p2;
   return (msg->u._int_value == (int)msg_id);
}

void _os_message_rundown( void * p )
{
   os_thread_msg_t * msg  = *(os_thread_msg_t **)p;
   if ( msg ) mem_free ( msg );
   *(os_thread_msg_t **)p = NULL;
}

/* Not supposed to compare */
__inline int _os_cache_compare( void * p1, void * p2 )
{ 
   return  0;
}

__inline void _os_cache_rundown( void * data )
{
   char * str = *(char **)data;
   if ( str ) mem_free( str );
   *(char **)data = NULL;
}

void _os_init_complete()
{
   lock_acquire(&_gp->init_lock);
   _gp->inited = 1;
   cond_broadcast(&_gp->inited_cond);
   lock_release(&_gp->init_lock);
}

    
/* THREADS */

DWORD WINAPI _os_worker_thread_start_routine( LPVOID param )
{ 
  os_thread_msg_t msg, *tmp;
  os_thread_t * this_thread = _os_get_current_thread();
  
  lock_acquire(&_gp->init_lock);
  while(1) {
    if ( _gp->inited ) break;
    cond_wait(&_gp->inited_cond, &_gp->init_lock,0);
  }
  lock_release(&_gp->init_lock);

  do {
    _os_pump_windows_messages();
    if (_os_get_next_thread_message( this_thread, &msg )) {
    	  if (_os_is_exit_message( &msg )) {
    	    /* This is a cool way for stopping other threads waiting on
    	       the same queue. What we do is post the same exit message so that any
    	       other thread waiting on the *same* queue will see this message and exit.
    	       Otherwise the poster has to post 'n' TM_EXITs for 'n' threads using the queue.
    	       This is a simple and scalable way to stop all other threads waiting on the same
    	       queue. This also deals with new threads just created about to wait on the
    	       same queue.
 
 	        unsigned long uc = queue_usagecount(this_thread->qinfo);
 	        if(uc > 0) {
 	          tmp = _os_create_thread_message();
    	        _os_post_thread_message(_os_copy_message(&msg, tmp));
 	        } 	      */
#ifdef __DEBUG__
      		fprintf( stdout, "Stopping thread...\n");
#endif
		     _os_exit_thread();
		     break;
	      }
    	 /* If there is work todo , do work, get result,
	        ,post result back */
	     if (msg.do_work) {
          (* msg.do_work)(&msg);
          if (msg._post_back) {
               tmp = _os_create_thread_message();
	            _os_post_thread_message(_os_copy_message(&msg, tmp));
          }
	     }
    } else {
      TRACE_PRINT("Warning : _os_get_next_thread_message failed");
    }
  }while (1);

  return 1;
}

void _os_clean_cache( os_thread_msg_t * msg )
{
    os_cache_t * cache = (os_cache_t *)msg->args;   
    link_list_destroy( cache );
}

/** Tight loops should use this to process Windows messages **/
void _os_pump_windows_messages()
{
  MSG msg;
  while (PeekMessage(&msg, NULL, 0UL, 0UL, PM_REMOVE)) {
     TranslateMessage( &msg );
	   DispatchMessage( &msg );
  }
}

WORD _os_i86_load_word( BYTE * bytes )
{
  WORD w = 0L;
  w = *bytes;
  w = (w << 8) | *(bytes + 1);
	return w;
}

DWORD _os_i86_load_doubleword( BYTE * bytes )
{
  DWORD dw = 0UL;
  dw = *bytes;
  dw = (dw << 8) | *(bytes + 1);
  dw = (dw << 8) | *(bytes + 2);
  dw = (dw << 8) | *(bytes + 3);
  return dw;
}


/* * * * * --------------------------------- * * * * *
 *       * LOCK IMPLEMENTATION               *       *
 * * * * * --------------------------------- * * * * */


/*
 *  I n i t i a l i z e   a   L o c k
 */
void lock_init(lock_t * lock)
{  
	InitializeCriticalSection( lock );
}

/*
 *  A c q u i r e   a   L o c k
 */
void lock_acquire(lock_t * lock )
{
  EnterCriticalSection( lock );
}

/*
 *  R e l e a s e   a   L o c k
 */
void lock_release( lock_t * lock )
{
  LeaveCriticalSection( lock );
}

/*
 *  D e s t r o y   a   L o c k
 */
void lock_destroy( lock_t * lock )
{
  DeleteCriticalSection( lock );
}




/* * * * * --------------------------------- * * * * *
 *       * CONDITION VARIABLE IMPLEMENTATION *       *
 * * * * * --------------------------------- * * * * */


/*
 *  I n i t i a l i z e   a   C o n d i t i o n   v a r i a b l e
 */
void cond_init(cond_t * cond)
{
  if ( cond ) {
    cond->chunk_size = 16;
    cond->allocated_chunks = 1;
    cond->map = mem_alloc( sizeof( event_thread_map_t ) * 16);
    lock_init(&cond->lock);
    cond->count = 0;
  }  
}

/*
 *  D e s t r o y   a   C o n d i t i o n   v a r i a b l e
 */
void cond_destroy(cond_t * cond)
{
  if ( cond ) {
    mem_free( cond->map );
    lock_destroy(&cond->lock); 
  }
} 


/*
 *  W a i t   o n   a   c o n d i t i o n   v a r i a b l e
 */
void cond_wait(cond_t * cond, lock_t * lock, unsigned long millisec)
{
   os_thread_t * this_thd = _os_get_current_thread();
   handle_t event; 
   long index;
   event_thread_map_t * tmp = NULL;

   event = _os_get_event_from_pool(_os_get_globals()->epool);

   if ( event ) {
     /* Set the event state to non-signalled */
     if ( ResetEvent( event ) ) {
        lock_acquire(&cond->lock);
        index = cond->count - 1;
        if ( index < 0 ) index = 0;
        if ( index >= cond->chunk_size * cond->allocated_chunks) {
            tmp = mem_realloc( cond->map,
                        sizeof( event_thread_map_t ) * 
                        cond->chunk_size * (cond->allocated_chunks + 1));
           if ( tmp ) {
               cond->map = tmp;
               ++cond->allocated_chunks;
           } else {
             TRACE_PRINT("Warning: mem_realloc failed in cond_wait"); 
           }
        }
        cond->map[index].id   = this_thd->id;
        cond->map[index].event = event;
        ++cond->count;
        lock_release(&cond->lock);
	   }

     /* unlock lock */
     if ( lock )
       lock_release(lock);
     millisec = (millisec == 0) ? INFINITE :
	             millisec;
     /* Wait for the event to become signalled */
     WaitForSingleObject(event, millisec);
     /* remove our entry from list */
     lock_acquire(&cond->lock);
     /* reset */
     cond->map[index].id = 0;
     cond->map[index].event  = NULL;
     /* return Event to pool */
     cond->count = cond->count ? --cond->count : 0;
     /* no longer need the event */
    _os_return_event_to_pool(_os_get_globals()->epool, event);
     lock_release(&cond->lock);
     /* Done! Now acquire user lock again and return */
     if ( lock )
       lock_acquire(lock);
	 }
}

/*
 *  S i g n a l   o n e   T h r e a d 
 */
void cond_signal(cond_t * cond)
{
  handle_t event;
  long index;
  if ( cond && cond->map ) {
     lock_acquire(&cond->lock);
     /* If there are threads waiting */
     if ( cond->count ) {
       /* pickup a thread in random and signal it */
       index  = rand() % cond->count;
       event  = cond->map[index].event;
       if ( event )
        SetEvent( event );
     }
     lock_release(&cond->lock);
  }
}

/*
 *  B r o a d c a s t 
 */
void cond_broadcast(cond_t * cond)
{
  handle_t event;
  int i = 0;
  if ( cond && cond->map ) {
     lock_acquire(&cond->lock);
     /* If there are threads waiting */
     if ( cond->count ) {
        /* signal all of them */
        for ( i = 0; i < cond->count; i++) {
          event = cond->map[i].event;
          if ( event )
            SetEvent( event );
        }
     }
     /* release critical section */
     lock_release(&cond->lock);
  }
}

/*
 *  G e t   E v e n t   f r o m   P o o l 
 */
handle_t _os_get_event_from_pool(pool_t * event_pool)
{
  pool_data_t * p;
  if ( event_pool ) {
    p = pool_get_data(event_pool);
    return p->h;
  } else {
    TRACE_PRINT("Warning: _os_get_event_from_pool: NULL event_pool");
    TRACE_PRINT("creating new event");
    return CreateEvent(NULL,TRUE,FALSE,NULL);  
  }
}

pool_data_t * _os_event_create(void * ref, void * pool) 
{
  pool_data_t * p = mem_alloc( sizeof( pool_data_t ));
  /* Create a manual-reset,non-signalled event */
  p->h = CreateEvent(NULL,TRUE,FALSE,NULL);
  return p;
}

void  _os_event_destroy(pool_data_t *p) 
{
  /* Create a manual-reset,non-signalled event */
  CloseHandle(p->h);
  mem_free(p);
}

int _os_event_compare(void * p1, void * p2)
{ 
  pool_data_t * pd1, *pd2;
  int ret;
  pd1 = *((pool_data_t **)p1);
  pd2 = (pool_data_t *)p2;
  ret =  (pd1->h == pd2->h); 
  return ret;
}

/*
 *  R e t u r n   E v e n t   t o   P o o l 
 */
void _os_return_event_to_pool(pool_t * event_pool, handle_t event)
{ 
  /* make sure we reset it */
  pool_data_t  tmp;
  ResetEvent( event );
  if ( event_pool ) {
    tmp.h = event;
    pool_return_data( event_pool, &tmp);
  } else
    CloseHandle( event );
}

/*
 *  C r e a t e   a   Q u e u e
 */
queue_info_t * queue_create( 
#ifdef __DEBUG__
				  char *name,
#endif
          pfn_link_compare_t pfn_compare,
          pfn_link_rundown_t pfn_rundown,
          pfn_copy_data_t  pfn_copy_data 
) 
{
  queue_info_t * qi = mem_alloc( sizeof(queue_info_t));
  assert( qi != NULL);
  lock_init(&qi->queue_lock);
  lock_init(&qi->count_lock);
  cond_init(&qi->queue_cond);
  qi->pfn_copydata = pfn_copy_data;
  qi->queue = link_list_create(
#ifdef __DEBUG__
				  name,
#endif
 	       LINK_LIST_QUEUE, LINK_DATA_TYPE_PTR, sizeof( void * ), 
			   pfn_compare, pfn_rundown 
			);
  assert( qi->queue != NULL);
  qi->usage_count = 1;

  return qi;
}

/*
 *  I n c r e m e n t   U s a g e   C o u n t
 */
void queue_addref(queue_info_t * qi)
{
  lock_acquire(&qi->count_lock);
  ++qi->usage_count;
  lock_release(&qi->count_lock);
}

/*
 *  I n c r e m e n t   U s a g e   C o u n t
 */
unsigned long queue_usagecount(queue_info_t * qi)
{
  unsigned long uc = 0;
  lock_acquire(&qi->count_lock);
  uc = qi->usage_count;
  lock_release(&qi->count_lock);
  
  return uc;
}

/*
 *  D e c r e m e n t   U s a g e   C o u n t
 */
void queue_removeref(queue_info_t * qi)
{
  lock_acquire(&qi->count_lock);
  --qi->usage_count;
  if (qi->usage_count <= 0) {
    lock_release(&qi->count_lock);
    queue_destroy(qi);
    return;
  }
  lock_release(&qi->count_lock);
}

/*
 *  A d d   d a t a   t o   Q u e u e
 */
int queue_add( queue_info_t * qi, void * data)
{
  lock_acquire(&qi->queue_lock);
  /* add the message to receiver's queue */
  if ( qi->queue ) {
    link_list_add_data(qi->queue, data );
    /* broadcast ' msg available' condition */
    cond_broadcast(&qi->queue_cond);
  }
  /* unlock */
	lock_release(&qi->queue_lock);
  return 1;
}

/*
 *  G e t   D a t a   f r o m  Q u e u e
 */
int queue_get(queue_info_t * qi, void * data)
{
  void * tmp;
  lock_acquire(&qi->queue_lock);
  while(1) {
     if (1 == link_list_get_next_data_ex(qi->queue , &tmp )) {
	      if ( tmp ) {
          qi->pfn_copydata(tmp, data);
          link_list_delete_data(qi->queue, tmp);
          break;
        }
     }
     cond_wait( &qi->queue_cond, &qi->queue_lock, 0);
  }
  lock_release(&qi->queue_lock);
  return 1;
}

/*
 *  D e s t r o y   Q u e u e
 */
void queue_destroy( queue_info_t * qi)
{
  assert(qi->usage_count == 0 );

  link_list_destroy(qi->queue); 
  cond_destroy(&qi->queue_cond);
  lock_destroy(&qi->queue_lock);
  lock_destroy(&qi->count_lock);    
  mem_free( qi );
}


/** TODO : Implement Read-Write lock **/