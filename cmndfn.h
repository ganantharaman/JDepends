
/** Must be ANSI 'c' Conformant **/

#ifndef __CMN_MCROS_INCLUDE__
#define __CMN_MCROS_INCLUDE__

#define ASSERT_H
#include <stdhdrs.h>

/** Debug implementation specific **/
#ifdef __DEBUG__

#define ASSERT(x) assert(x)
#define ASSERT_VALID(x)  assert(x != NULL)

/** General purpose tracing and logging macros **/
#ifdef __TRACE__
#define TRACE_PRINT2( x, y, z ) fprintf(stdout, x, y, z);
#define TRACE_PRINT1( x, y )	fprintf(stdout, x, y);
#define TRACE_PRINT0( x )		fprintf(stdout, x );
#define TRACE_PRINT( x )		fprintf( stdout, "%s\n", x);
#define DECLARE_TRACE_DATA( x ) static char __FUNC__[]=x;
#else
#define DECLARE_TRACE_DATA( x )
#define TRACE_PRINT2( x, y, z ) 
#define TRACE_PRINT1( x, y )	
#define TRACE_PRINT0( x )		
#define TRACE_PRINT( x )	
#endif  /** __TRACE__ **/

#ifdef __LOG__

#define DECLARE_LOG_DATA(x) \
   FILE * __log__fp;\
   char __LOGFILE__[]=x;\

#define LOG_WRITE( msg1, msg2 )\
do\
{\
 if((__log__fp = fopen(__LOGFILE__, "a+b")) != NULL){\
  fprintf(__log__fp, msg1, msg2);\
  fprintf(__log__fp,"\r\n");\
  fclose( __log__fp );\
 }\
}while(0);

#define LOG_WRITE1( msg1, msg2, msg3 )\
do\
{\
 if((__log__fp = fopen(__LOGFILE__, "a+b")) != NULL){\
  fprintf(__log__fp, msg1, msg2, msg3);\
  fprintf(__log__fp,"\r\n");\
  fclose( __log__fp );\
 }\
}while(0);

#else 
#define DECLARE_LOG_HEADER()
#define DECLARE_LOG_DATA(x) 
#define LOG_WRITELN( msg )
#define LOG_WRITE( msg )
#endif /** __LOG__ **/

#else  /** no __DEBUG__ **/

#define DECLARE_TRACE_DATA( x )
#define TRACE_PRINT2( x, y, z ) 
#define TRACE_PRINT1( x, y )	
#define TRACE_PRINT0( x )		
#define TRACE_PRINT( x )		
#define DECLARE_LOG_HEADER()
#define DECLARE_LOG_DATA(x) 
#define LOG_WRITELN( msg )
#define LOG_WRITE( msg )


#ifdef ASSERT
 #undef  ASSERT
 #define ASSERT(x)	 ((void)0)
#endif
#ifdef ASSERT_VALID
 #undef  ASSERT_VALID
 #define ASSERT_VALID(x) ((void)0)
#endif

#endif /** __DEBUG__ **/

#endif /** __CMN_MCROS_INCLUDE__ **/


