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

#ifndef  __W32OS_API_INCLUDE__
#define  __W32OS_API_INCLUDE__

#include <winbase.h>
#include <linklist.h>
#include <hashtabl.h>

#define  handle_t    HANDLE

#include <lock.h>
#include <cond.h>
#include <pool.h>

typedef  link_list_t os_cache_t;
typedef void * (*pfn_copy_data_t)(void *, void *);

enum {
	TM_SUSPEND,
    TM_EXEC,
	TM_EXIT,
	TM_QUIT
};

enum {
	data_type_ptr,
	data_type_int,
	data_type_long
};

enum {
	ENV_CLASSPATH=0,
    ENV_PATH,
	NUM_ENV_VARS
};

typedef struct queue_info {
  lock_t         queue_lock;
  link_list_t  * queue;
  cond_t         queue_cond;
  lock_t         count_lock; 
  unsigned long  usage_count;
  pfn_copy_data_t pfn_copydata; 
} queue_info_t;

typedef struct os_thread
{
  handle_t      thread; 
  unsigned long id; 
  char *        name; 
  queue_info_t * qinfo; 
  hash_type_t   htkey; 
  hash_type_t   htval; 
} os_thread_t;

/* TODO Add Mutex pool */
typedef struct os_globals {
   pool_t *     epool;
   cond_t       inited_cond;
   int          inited;
   link_list_t * thd_list;
   os_thread_t * cache_thd;
   os_thread_t * main_thd;
#ifdef __THREAD__
   lock_t       init_lock; 
   lock_t       thd_lock;
#endif
   char ** env_names;
   /* We retain this now.*/
   dict_t *  event_thd_dict;
}os_globals_t;




typedef struct os_thread_msg
{
	union {
		int    _int_value;
		long   _long_value;
		void * _ptr_value;
	}u;
  void (* do_work)(struct os_thread_msg *);
  int        _data_type;
 	void *  args;
	void *  result;
	int         _post_back;
	os_thread_t * _t_receiver;
	os_thread_t * _t_sender;
} os_thread_msg_t;

typedef void  (*pfn_job_routine_t)(os_thread_msg_t *);


#define OS_SUCCESS_STATUS  ERROR_SUCCESS
#define OS_FAILED_STATUS   1

#define OS_FILE_READ_ONLY  1
#define OS_FILE_WRITE_ONLY 2
#define OS_FILE_READ_WRITE 3


/* Init */

os_globals_t * _os_get_globals();

int _os_init();

void _os_init_complete();


/* Mutex Related */

int  _os_create_mutex(handle_t * mutex, unsigned long *  err);

int _os_wait_on_mutex( handle_t mutex, unsigned long * reason, unsigned long * err );

int _os_release_mutex( handle_t mutex, unsigned long * err );

int _os_destroy_mutex(handle_t mutex, unsigned long * err);


/* Thread */

int _os_set_thread_priority(os_thread_t * thd, unsigned int priority, unsigned long * err );

int _os_get_thread_priority(os_thread_t * thd, int * priority, unsigned long * err);

int _os_create_thread( pfn_job_routine_t pfn_start_routine,void * param, char * name,
				os_thread_t ** thd, queue_info_t * queue_info, unsigned long * err ); 

int _os_prepare_to_suspend_thread( os_thread_t * thd, unsigned long * err );

int _os_suspend_thread( os_thread_t * thd, unsigned long millisec, unsigned long * err );

int _os_suspend_thread_cleanup( os_thread_t * thd, unsigned long * err );

int _os_resume_thread(os_thread_t * thd, unsigned long * err);

int _os_destroy_thread( os_thread_t * thd, unsigned long * err  );

os_thread_t * _os_get_current_thread();

os_thread_t * _os_get_thread_from_name( char * name );

char * _os_get_env_name_from_id(  int id );

void _os_exit_thread();

void _os_sleep(unsigned long millisec);


/* Thread Message */

int _os_post_execute_thread_message(os_thread_t * receiver, int post_back, 
                                 pfn_job_routine_t job, void * args);

int _os_post_thread_message(os_thread_msg_t * msg);

int _os_get_next_thread_message( os_thread_t * thd, os_thread_msg_t * msg );

os_thread_msg_t * _os_create_thread_message();

void * _os_copy_message(void * p1, void * p2 );

int _os_is_exit_message( os_thread_msg_t * msg );


/* Memory allocation */

int _os_malloc( size_t size, void ** mem, unsigned long * err);

int _os_calloc( size_t size, void ** mem, unsigned long * err);

int _os_realloc(void * old, size_t size,  void ** mem, unsigned long * err);

void _os_free( void * p );


/* File I/O */

int _os_open_file( char * file_name, unsigned int mode, handle_t * file, unsigned long * err );

int _os_read_file( handle_t file, unsigned char ** bytes_to_read, unsigned long count, 
				   unsigned long * actual_bytes_read_count,  unsigned long * err );

int _os_write_file( handle_t file, unsigned char *  bytes_to_write, unsigned long count,
					unsigned long * bytes_written_count, unsigned long * err );

int _os_close_file( handle_t file, unsigned long * err );

int _os_get_file_size( handle_t file, unsigned long * file_size, unsigned long * err );


/* Library */

int _os_load_library( char * lib_name, handle_t * lib, unsigned long * err );

int _os_free_library( handle_t lib, unsigned long * err );

int _os_link_function( char * func_name, handle_t lib, void ** func, unsigned long * err );


/* Environment */

int _os_get_path_mark( char ** str, os_cache_t * cache );

int _os_get_env( int index, char ** str, unsigned long * err );

char * _os_get_env_name_from_id(int index );

int _os_get_current_dir( char ** dir, os_cache_t * cache, unsigned long * err );


/* Cache */

int _os_cache_init(os_cache_t ** cache );

void _os_cache_flush( os_cache_t * cache );

/* Queue */

queue_info_t * queue_create( 
#ifdef __DEBUG__
				  char *name,
#endif
          pfn_link_compare_t pfn_compare,
          pfn_link_rundown_t pfn_rundown,
          pfn_copy_data_t  pfn_copy_data 
); 

void queue_addref(queue_info_t * qi);

void queue_removeref(queue_info_t * qi);

unsigned long queue_usagecount();

int queue_add( queue_info_t * qi, void * data);

int queue_get(queue_info_t * qi, void * data);

void queue_destroy( queue_info_t * qi);


/* Miscellaneous */

int  _os_handle_close( handle_t hdl, unsigned long * err );

UINT _os_get_random();

void _os_get_error_message( unsigned long err_code, os_cache_t * cache, char ** str );

unsigned long _os_get_error();

handle_t _os_get_event_from_pool(pool_t * event_pool);

void _os_return_event_to_pool(pool_t * event_pool, handle_t event);

void  _os_event_destroy(pool_data_t *p) ;

void _os_pump_windows_messages();

int _os_post_exit_thread_message(os_thread_t * sender, os_thread_t * receiver);



/* Exit */

void _os_exit();


#endif /* __W32OS_API_INCLUDE__ */




