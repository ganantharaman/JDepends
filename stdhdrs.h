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
#ifdef WIN32  
/* Common includes */
#ifdef MALLOC_H
 #include <malloc.h>
 #undef MALLOC_H
#endif
#ifdef STDLIB_H
 #include <stdlib.h>
 #undef STDLIB_H
#endif
#ifdef STDARG_H
 #include <stdarg.h>
 #undef STDARG_H
#endif
#ifdef STDIO_H
 #include <stdio.h>
 #undef STDIO_H
#endif
#ifdef ASSERT_H
 #include <assert.h>
 #undef ASSERT_H
#endif
#ifdef STRING_H
 #include <string.h>
 #undef STRING_H
#endif
#ifdef SETJMP_H
 #include <setjmp.h>
 #undef SETJMP_H
#endif

/** jvmw32gb.h Must be included only once in the whole project 
 , because of OS specific globals 
**/
#elif defined( __UNIX__ ) 

/* Common includes */
#ifdef MALLOC_H
 #include <malloc.h>
 #undef MALLOC_H
#endif
#ifdef STDLIB_H
 #include <stdlib.h>
 #undef STDLIB_H
#endif
#ifdef STDARG_H
 #include <stdarg.h>
 #undef STDARG_H
#endif
#ifdef STDIO_H
 #include <stdio.h>
 #undef STDIO_H
#endif
#ifdef ASSERT_H
 #include <assert.h>
 #undef ASSERT_H
#endif
#ifdef STRING_H
 #include <string.h>
 #undef STRING_H
#endif
#ifdef SETJMP_H
 #include <setjmp.h>
 #undef SETJMP_H
#endif
#endif /* __UNIX__ */


