#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include <mem.h>
#include <util.h>

#include <unzip.h>

typedef struct token_info{
		char * next;
		char * token;
		char buf[64];
}token_info_t;

int uzip_msg_func( void * G, unsigned char * msg, unsigned long n, int flag );


/*
 * Function : add current working directory to file name           
 * Notes    : Caller is responsible for 'mem_free' ing returned buffer 
 */
char * add_cwd(char * file)
{
	char * fullname = NULL;
	char   buffer[_MAX_PATH];
	int len;

	/* check if file already has a full name */
	if ( file && ( strstr(file, ":\\") == NULL ) ) {
       /* get current directory */
       if ( getcwd( buffer, sizeof(buffer) ) != NULL ) {
		 len = strlen( buffer );
		 /* allocate for full name and assign value */
	     fullname = mem_alloc( strlen( file ) + len + 2);
		 sprintf( fullname, (buffer[len-1] == '\\') ?
			 "%s%s" : "%s\\%s" , buffer, file );
	   }
    }

	return fullname;
}

int uzip_msg_func( void * G, unsigned char * msg, unsigned long n, int flag )
{
  return UzpMessagePrnt( G, msg, n, flag);
}


int is_class_in_jar( char * jar, char * class)
{
  assert ( jar && class );
  jar_contents_t * mf_contents;

  mf_contents = extract_from_jarfile( jar, MANIFEST_FILE);
  assert( mf_contents != NULL );
  grep_buffer( class
  
    
}


jar_contents_t *  extract_from_jarfile(char * zip, char * file)
{
 /*  int UZ_EXP UzpUnzipToMemory(char *zip, char *file, UzpOpts *optflgs,
    UzpCB *UsrFuncts, UzpBuffer *retstr) */
   jar_contents_t * pcontents = NULL;
   UzpOpts opts;
   UzpCB   ufuncs;
   UzpBuffer buf;
   memset( &opts, 0, sizeof( UzpOpts ));
   memset( &ufuncs, 0, sizeof( UzpCB ));
   ufuncs.structlen = sizeof( UzpCB );
   ufuncs.msgfn = uzip_msg_func;
   memset( &buf, 0, sizeof( UzpBuffer ));
   if ( UzpUnzipToMemory( zip, file, &opts, &ufuncs, &buf ) ) {
     if ( pcontents = mem_alloc( sizeof( jar_contents_t )) ){
        pcontents->len =  buf.strlength;
        pcontents->str =  buf.strptr;
     }
   }

   return pcontents;
}





void free_jar_contents( jar_contents_t * contents)
{
   UzpBuffer buf;
   assert( contents != NULL);

   buf.strlength = contents->len;
   buf.strptr = contents->str;

   UzpFreeMemBuffer( &buf );
   mem_free( contents );
}

/*
 *  Function : Checks if a given file is of JAR format
 *  Formal Parameters   : 
 *                file  :  File name
 *  Return              :  1 if file is a JAR
 *                         0 otherwise  
 *  Notes    : -nil-
 */
int is_jar( char * file )
{
	char * p = file ;

	// TODO
	// CHECK to see if the contents are of JAR format
    
	// For now return FALSE
	return 0;
}

/*
 *  Function : Add the given path to the file
 *  Formal Parameters   : 
 *                file  :  File name ( could be of form x.class or a\b\c.class)
 *                path  :  Full path
 *  Return              :  Pointer to a modified path or NULL.
 *  Notes                  file should be big enough to hold the result
 */
char * add_class_path( char * file, char * path )
{
	char * tmp = mem_calloc( strlen( file ) + 1 );
	char *p;
  strcpy( tmp, file);
	if ( tmp ) {
   	   while ((p = strchr( tmp, '/')) != NULL) *p = '\\'; 
       sprintf( file, "%s%s%s", path, (strchr( tmp, '\\')) ? "\\" : "",  tmp );
	   mem_free ( tmp );
	}
	return file;
}


int add_path_from_envvar(char * env_var, char * file_name, char * full_file_name, int size )
{
  char * cpath = NULL;
  void * tmp = NULL;
  char * path;
  char buf[512];
  char * file = buf;
  int name_len, len;
  struct _stat file_info;

  assert( file_name != NULL);
  name_len = strlen( file_name );

  /* check if the file already has full path */
  if ( file_name[1] == ':' ) {
     strcpy( full_file_name, file_name);
     return 0;
  }

  cpath = get_env(env_var);

  while ( path = str_tok(cpath, ";", &tmp)) {
    len = strlen( path ) + name_len  + 2;
    if (len > 512 ) {
      file = mem_calloc( len );
    }
    strcpy( file, file_name );
    add_class_path(file, path);
    /* found file */
    if(!_stat( file,  &file_info )) {
       if ( len > size ) {
         if (len > 512) mem_free( file );
         return len;
       }
       strcpy( full_file_name, file );
       str_tok_flush( &tmp );
       if (len > 512) mem_free( file );
       return 0;
    }
    if (len > 512) mem_free( file );
    file = buf; 
  } 

  return -1;
}

/*
 *  Function : Add extension to file if necessary
 *  Notes    : -nil-
 */
void add_file_ext( char * file, char * ext, char * ret_file)
{
   int len_ext = strlen( ext );
   int len_file = strlen( file );

   if ( len_file  <= len_ext || 
	   strcmp( file + len_file - 1 - len_ext, ext ) == 0 ) 
      strcpy( ret_file, file );
   else
      sprintf( ret_file, "%s%s", file, ext );
}

/*
 *  Function  : Gets the value for a given environment variable
 *  Formal Parameters: 
 *                name - Name of the env var whose value is sought
 *  Return    :   NULL - if No such env variable exist
 *                otherwise, a pointer to env var's value.
 *                which must be freed by caller.
 *  Notes     : -nil-
 */
char * get_env( char * name ) 
{
	char *p,*new_p = NULL;
	if (p = getenv( name )){
		new_p = mem_alloc( strlen(p) + 1 );
		strcpy( new_p, p );
	}

	return new_p;
}


/*
 *  Function  : Check if a given file exists
 *  Formal Parameters: 
 *                file -  Name of the file whose existance is to be checked.
 *  Return    :   1 - if file exists
 *                0 - file does not exist
 *  Notes     : -nil-
 */
int file_exists( char * file )
{
	struct _stat _buf;
	if ( _stat( file , &_buf ) < 0 )
		return 0;
	return 1;
}



/* 
 *  Function : Same as of strtok() call. However input buffer is not modified
 *  Notes    : Caller needs to pass a 'NULL'ed void* as info. this function will
 *             store token_information there.This pointer is non read/write
 *  Arguments  :
 *      buffer : Input buffer pointer
 *      seps   : '\0' terminated list of separator characters
 *      info   :  Address to a void* 
 */
char * str_tok(char * buffer, char * seps, void ** info )
{
  char *p, *q, *token;
	unsigned long length;
  token_info_t * token_info;

	if ( *info ) {
       /* check if we are done */
       if (*info ==  (void *)0xffffffff) return NULL;
       token_info = (token_info_t *)*info;
  } else {  /* First Time */
		token_info = mem_alloc(sizeof(token_info_t));
		if ( !token_info ) {
			fprintf(stdout, "str_tok memory error" );
			return NULL;
		}
        token_info->next = buffer;
		token_info->token = NULL;
	}

	token = p = token_info->next;
	if ( token_info->token ) {
		mem_free( token_info->token );
		token_info->token = NULL;
	}

	while ( *p != '\0' ) {
    q = seps; 
	  while ( *q && *p != *q++ );
	  if ( *p == *--q ) break;
	  ++p;
	}

	if ( *p == '\0' ) { /* Last Time */
		p = token_info->next;
		if (token_info->token)
			mem_free( token_info->token ) ; 
		mem_free( token_info );
    *info = (void *)0xffffffff;
		return *p ? p : NULL;
	}

	length = p - token;
	if ( length > sizeof( token_info->buf )  - 1 ) {
		token_info->token = mem_alloc( length + 1);
		if ( !token_info->token ) {
			mem_free ( token_info );
			fprintf(stdout, "str_tok memory error" );
			return NULL;
		}
		q = token_info->token; 
	} else {
		q = token_info->buf;
	}

	if ( length )
	  memcpy( q, token, length );

	q[length] = '\0';

	token_info->next = p + 1; 

	*info = (void *)token_info;

	return q;
}

void str_tok_flush(void **p)
{
  token_info_t * tinfo = (token_info_t *)(*p);
  if ( tinfo ) {
    if ( tinfo->token ) mem_free( tinfo->token );
    mem_free( tinfo ); 
  }
}


grep_obj_t * grep_create()
{
	return (grep_obj_t_p) mem_calloc( sizeof( grep_obj_t ));
}

void grep_destroy( grep_obj_t * p )
{
  if ( p ) mem_free ( p );
}

int grep( grep_obj_t * obj )
{
  extern int _fmode;

  FILE * fp     = NULL;
  char * buffer = NULL;
  int    code   = 0;
  const char * file_name = obj->file_name;
  const char * string    = obj->string;
  struct _stat file_info;
  size_t line_count = 0,
	     cmp_len,  alloc_size = ~0,
		 temp_size,bytes_read = 0;

  /* sanity checks */
  assert( file_name != NULL );
  assert( string != NULL );

  _fmode = _O_BINARY;

  /* open files */
  if (!( fp =fopen( file_name, "r" ) ))
  {
	  fprintf( stderr, "File  %s could not be opened", file_name );
	  return 0;
  }

  if ( !(obj->fpout = fopen( obj->out_file_name, "a" )))
  {
      fprintf( stderr, "File  out.out could not be opened");
	  goto __exit;
  }
  
  /* Check if statistics are valid: */
  if( _stat( file_name,  &file_info ) )
  {
      fprintf( stderr, "unable to get file %s's info", file_name );
	  goto __exit;
	  
  }

  alloc_size  =  file_info.st_size;  
  
  /* File size > sizeof( size_t ) not supported */
  if ( alloc_size > ( size_t)~0 )
  {
      fprintf( stderr, "File size > %ld not supported",sizeof( size_t ));
	  goto __exit;
  }
 
  if ((buffer = mem_alloc( alloc_size )) == NULL) 
  {
      fprintf( stderr, "mem_alloc(%ld) failed", alloc_size );
      goto __exit; 
  }

  bytes_read = fread( buffer, sizeof( char ), alloc_size, fp);

  if ( !ferror( fp ) )
  {
    if ( bytes_read  )
  	{
      obj->buffer = buffer;
      obj->len = alloc_size;
      obj->fpout = fpout;
      grep_buffer( obj );
	  }
  }

__exit:

  if ( buffer ) mem_free( buffer );
  if ( fpout  ) fclose( fpout  );
  if ( fp     ) fclose( fp     );
  
  return code;

}


void grep_buffer( grep_obj_t * obj )
{
  int    code   = 0;
  const char * file_name = obj->file_name;
  const char * string    = obj->string;
  size_t line_count = 0,
  cmp_len,  alloc_size = ~0,
  temp_size,bytes_read = 0;
  char * p = obj->buffer;
  char * buffer = p;
  alloc_size = temp_size = obj->len;
  /* forever */
  for ( ;; )
  {
		  if ( p >= (buffer + alloc_size)) break;

		  /* found a CRLF */
		  pcrlf = memchr( p, '\r', temp_size );
		  cmp_len =  pcrlf ? pcrlf - p : temp_size;

		  /* increment line_count */
       	  ++ line_count;
		  if ( cmp_len )
		  {
			  /* check this line alone - and print */
		       check_line( obj, p, cmp_len, string, line_count, file_name, fpout );
			   p += cmp_len;
		  }

		  /* point to beyond CRLF */
		  p += 2;
		  temp_size = temp_size - cmp_len - 2;
	   } 
}



void check_line(
 grep_obj_t * obj,
 char * p, int cmp_len,
 const char * string,  int line_count,
 const char * file_name, FILE * fpout
)
{
	char * q;
	char first_char,last_char;
	char print_buf[256] = "";
	int length;

	assert( string != NULL);

	/* needed assignments */
	length     =  strlen( string );
	first_char =  string[0];
	last_char  =  string[ length - 1];

	q = memchr( p, first_char, cmp_len );
    if ( q )
	{ 
       if (( q + length - 1 ) < ( p + cmp_len ))
	   {
		   if ( last_char == *(q + length - 1))
		   {
             /* Is string exactly the same ? */
             if ( memcmp( q, string, length ) == 0 )
			 {
                if ( obj->call_back )
					obj->call_back( p, q, cmp_len );
				else {
       			memset( print_buf, 0, sizeof( print_buf ));
				memcpy( print_buf, q, cmp_len - ( q - p));

				/* print it to the console and file */
	   		    fprintf( stdout, "File : %s, Line : %ld, %s\n", 
	    	    file_name, line_count, print_buf );
            if ( fpout )
     				 fprintf( fpout, "File : %s, Line : %ld, %s\n", 
		    				file_name, line_count, print_buf );
				}
			 }
		   }

		   /* fewer bytes to compare */
		   cmp_len = cmp_len - ( q - p ) - 1;
		   if ( cmp_len > 0 )
		   {
             /* start searching from next char */
		     p = q + 1;
             check_line( obj, p, cmp_len, string, line_count, file_name, fpout ); 
		   }
	   }
	}   
}

	















