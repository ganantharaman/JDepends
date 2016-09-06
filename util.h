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

#ifndef __UTIL_HDR_INCLUDE__
#define __UTIL_HDR_INCLUDE__

#ifdef __cplusplus
extern "C" {
#endif


/*
 *  jar_contents_t structure, 
 *  used in extract_from_jarfile API
 */
#ifndef __IN_UTIL_SOURCE__     /* Defined only in util.c */
typedef struct _UzpBuffer {    /* rxstr */
    unsigned long   strlength;           /* length of string */
    char  *strptr;             /* pointer to string */
} UzpBuffer;
#endif

typedef UzpBuffer jar_contents_t;

enum {
	AP_CLASS_FOUND = 0,  /* Returned, class physically is in a dir from CLASSPATH */
  AP_CLASS_IN_JAR,     /* Returned, Class in JAR, returned file is JAR name */
	AP_INVALID_LEN,      /* Returned, Buffer size is not enough */
	AP_CLASS_NOT_FOUND,  /* Returned, Class not found in CLASSPATH or any specifed ZIPs/JARs */
};


/*
 *  grep_obj_t structure, fill in a file name and string and we will
 *  search it for you and ping you back!
 */
typedef struct grep_obj {
	const char * file_name;
  const char * out_filename;
	const char * string;
  const char * buffer;
  size_t len;
	void (* call_back)( struct grep_obj *, char *,char *, int, size_t );
  void * data; /* For caller to use */
  void * fpout; /* Output file handle */
}grep_obj_t, *grep_obj_t_p;


/*
 *  Add current working directory to file name           
 */
char * add_cwd(char * file);

/*
 *  Checks if a given file is of JAR format
 */
int is_jar( char * file );

/*
 *  Add the given path to the file
 */
char * add_class_path( char * file, char * path );

/*
 *  Attempts to locate a given .class file
 */
long locate_class_file(char * env_var, char * file_name, char * full_file_name, int * psize );

/*
 *  Add extension to file if necessary
 */
void add_file_ext( char * file, char * ext, char * ret_file);

/*
 *  Gets the value for a given environment variable
 */
char * get_env( char * name ); 

/*
 *  Check if a given file exists
 */
int file_exists( char * file );

/* 
 *  Same as of strtok() call. However input buffer is not modified
 */
char * str_tok(char * buffer, char * seps, void ** info );

/* 
 *  Clean up str_tok related buffers. Call this when str_tok calls are complete
 */
void str_tok_flush(void **p);

/* 
 *  Grep for a given string , invoke callback if present
 */
int grep( grep_obj_t_p );

/* 
 *  Check if a given class is in a JAR file 
 */
int is_class_in_jar( char * jar, char * cls);

/* 
 *  Extract a file from a given JAR file , returns NULL on error
 */
jar_contents_t *  extract_from_jarfile(char * zip, char * file);

/* 
 *  Free any resources related to extract_from_jarfile
 */
void free_jar_contents( jar_contents_t * contents);

/*
 *   Grep in buffer
 */
void grep_buffer( grep_obj_t * obj );

/*
 *  Calculates the set bit
 */
unsigned int bit_index( unsigned long val );


#ifdef __cplusplus
}
#endif

#endif /* __UTIL_HDR_INCLUDE__ */






	















