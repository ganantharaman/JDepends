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
/****************************************************************************/
/*  TTD   :                                                                 */
/*     Modify stream read so that the read data is no longer present in the */
/*     stream. This is called destructive rea d. A property/flag to enforce */
/*     this behavior is helpful                                             */
/*                                                                          */
/****************************************************************************/

#include <cmnmcros.h>
#include <linklist.h>
#include <lock.h>
#include <cond.h>
#include <stream.h> 
#include <mem.h>
#include <pool.h>
#include <stdio.h>

/* Pool of streams */
static pool_t* stream_pool = NULL;

/* Initialize stream */
void stream_init(int use_pool)
{ 
  pool_info_t pi;
#ifdef __DEBUG__
  strcpy(pi.name, "StreamPool");
#endif
  pi.init_count =_STREAM_POOL_SIZE;
  /* maximum count */
  pi.max_count = 10;;
  /* pool policies */
  pi.flags  = 0L;
  /* reference */
  pi.ref = NULL;
  /* initialize data */
  pi.pfn_create_data  = stream_create;
  /* clean up  data  */
  pi.pfn_cleanup_data = stream_destroy; 
  /* compare data    */
  pi.pfn_compare_data = stream_compare;

  /* use the pool if required to do so */
  if ( use_pool )
    stream_pool = pool_create(&pi);

}

int stream_compare(void * p1, void * p2)
{
  pool_data_t * pd1, *pd2;
  int ret;
  pd1 = *((pool_data_t **)p1);
  pd2 = (pool_data_t *)p2;
  ret =  (pd1->p == pd2->p); 
  return ret;
}

void * stream_create(void * ref, void * pool)
{
  pool_data_t * p = mem_calloc( sizeof(pool_data_t));
  stream_t * strm = mem_calloc( sizeof( stream_t ));
  if ( strm ) {
    strm->buffer = mem_alloc( _CACHE_SIZE );
    strm->alloc_size = _CACHE_SIZE;
  	lock_init(&strm->lock);
    strm->read_size = strm->cur_size   = 0;
  }
  if(p) p->p = strm;
  return (void *)p;
}

/* creates and returns a stream */
STREAM stream_open()
{
  STREAM strm = NULL;
  pool_data_t * data;
  
  if ( stream_pool ) {
    data = pool_get_data( stream_pool );
    strm = (STREAM)data->p;
  } else
    strm = mem_calloc( sizeof( stream_t ));

  if ( strm ) {
    strm->buffer = mem_alloc( _CACHE_SIZE );
    strm->alloc_size = _CACHE_SIZE;
  	lock_init(&strm->lock);
    strm->read_size = strm->cur_size   = 0;
  }

  return strm;
}

void stream_flush(STREAM strm)
{
   memset( strm->buffer, 0, strm->cur_size);
   strm->read_size = strm->cur_size = 0;
}

void f_stream_write( STREAM strm, char * fmt, ... )
{
	char * ptr = fmt;
	va_list arglist;
	char buffer[256];
	char * str = NULL;

	buffer[0] = 0;

	va_start(arglist, fmt);
	while( *ptr ){
	 switch( *ptr++ ) {
	 case '%': 
		 switch( *ptr ) {
	     case 's':
	     case 'S':   str = va_arg( arglist, char *); ptr++;
		    	     break;
	     case 'd':
	     case 'D': {
		             int i;
			    	 i = va_arg( arglist, int );
				     sprintf( buffer, "%d", i ); ptr++;
				     break;
				   }
	     case 'l':
	     case 'L': {
		             long l;
			    	 l = va_arg( arglist, long );
				     sprintf( buffer, "%l", l ); ptr++;
				     break;
				   }
	     case 'f': {
		             float f;
			    	 f = va_arg( arglist, float );
				     sprintf( buffer, "%f", f ); ptr++;
				     break;
				   }
	     case 'e': {
		             double d;
			    	 d = va_arg( arglist, double );
				     sprintf( buffer, "%e", d ); ptr++;
    				 break;
				   }
	     default:  {
				     int count = 1;
				     char * p = buffer;
				     --ptr;
				     while ( *ptr && *ptr != '%' )
					   *p++ = *ptr++;
				     *p = '\0';
					 stream_write( strm, buffer, strlen( buffer ));
					 buffer[0] = 0;
				     break;
				   }
		 }
		 break;
	 default:  {
			     int count = 1;
			     char * p = buffer;
			     --ptr;
			     while ( *ptr && *ptr != '%' )
				   *p++ = *ptr++;
			     *p = '\0';
			     break;
			   }
		 
	 }

     if (str || buffer[0] ){
	    stream_write( strm, str?str:buffer,
		      strlen( str?str:buffer ));
		buffer[0] = 0;
	 }
     str = NULL;
	}
	va_end( arglist );
}

void stream_write( STREAM strm, unsigned char * buffer, unsigned32 len )
{
  assert( strm && buffer );
  lock_acquire( &strm->lock );

  /* Expand the stream if necessary */
  if ( (strm->cur_size + len + 2 ) >=  strm->alloc_size )
  {
    char * buffer;
    unsigned32 alloc_len = (len - (len % _CACHE_SIZE)) + _CACHE_SIZE;
	  buffer = mem_realloc( strm->buffer, strm->alloc_size + alloc_len);
	  if (!buffer ) {
		  TRACE_PRINT("Error in Stream buffer allocation");
		  return ;
	  }
	  strm->buffer = buffer;
    strm->alloc_size += alloc_len;
  }
  /* write */
  memcpy( &strm->buffer[ strm->cur_size ], buffer, len);
  strm->cur_size += len;
  lock_release( &strm->lock );
}

int stream_read( STREAM strm, unsigned char * buffer, unsigned32 len )
{
  assert( strm && buffer );
  lock_acquire( &strm->lock );

  if ( strm->read_size >= strm->cur_size ) {
    lock_release( &strm->lock );	   
    return 0;
  }

  /* adjust length */
  if ((strm->read_size + len - 1) > strm->cur_size )
	 len = strm->cur_size - strm->read_size + 1;
  /* read */
  memcpy( buffer, &strm->buffer[ strm->read_size ], len );
  strm->read_size += len;

  lock_release( &strm->lock );
  return 0;
}

void stream_close(STREAM strm)
{
  if ( stream_pool ){
    pool_data_t t;
    t.p = strm;
    pool_return_data( stream_pool, &t );
  } else
   stream_destroy(strm);
}

void stream_destroy(void * p) 
{
  STREAM strm = p ? (STREAM)((pool_data_t *)p)->p : NULL;
  if(strm) {
    mem_free ( strm->buffer ); 
    lock_destroy(&strm->lock);
   /* free stream itself */
    mem_free ( strm );  
  }
}

void stream_reset(void * p) 
{
    STREAM strm = (STREAM)p;
    lock_destroy(&strm->lock);
   /* free stream itself */
    mem_free ( strm );  
}

unsigned char * stream_get_buffer(STREAM strm)
{
  strm->buffer[strm->cur_size] = 0;
  return strm->buffer;
}

unsigned32 stream_get_length(STREAM strm)
{
  return strm->cur_size;
}

void stream_exit() 
{
  /* destroy the pool */
  if ( stream_pool ) {
     pool_destroy( stream_pool ); 
     stream_pool = NULL;
  }
}


