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

#ifndef __STRM_HDR_INCLUDE__
#define __STRM_HDR_INCLUDE__

typedef struct stream {
  int pool_index;           /* required to pool the stream */  
  lock_t lock;
  unsigned char *buffer;
  unsigned32 alloc_size, read_size , cur_size;
}stream_t, *STREAM;

enum {
	_STREAM_POOL_SIZE = 20,
	_CACHE_SIZE       = 64
};

unsigned char * stream_get_buffer(STREAM strm);
unsigned32 stream_get_length(STREAM strm);
void   stream_init(int use_pool);
STREAM stream_open();
void   stream_flush( STREAM strm);
void f_stream_write( STREAM strm, char * fmt, ... );
void   stream_write( STREAM strm, unsigned char* buffer, unsigned32 len );
int    stream_read(  STREAM strm, unsigned char* buffer, unsigned32 len );
void   stream_close( STREAM strm );
void   stream_destroy( void * p);
void   stream_reset(void * p);
void   stream_exit();
void * stream_create(void *, void *);
int    stream_compare(void * v1,void * v2);


#endif /* __STRM_HDR_INCLUDE__ */