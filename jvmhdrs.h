/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*  This code and any software associated is covered by                     */
/*  GNU Lesser Public License (LGPL)                                        */
/*                                                                          */
/*  Author: Gopal Ananthraman                                               */
/*                                                                          */
/*  Desc  :                                                                 */
/*                                                                          */
/****************************************************************************/

#ifdef WIN32  
#include <stdhdrs.h>

#ifdef  JVMOS_H
 #include <jvmwin32.h>
 #undef JVMOS_H
#endif
#ifdef  JVMOSAPI_H
 #include <osapi.h>
 #undef JVMOSAPI_H
#endif
#ifdef  JVMOSGB_H
 /* include osapi globals */ 
 #undef JVMOSGB_H
#endif

#elif defined( __UNIX__ ) 
#include <stdhdrs.h>

#ifdef  JVMOS_H
 #include <jvmunix.h>
 #undef JVMOS_H
#endif
#ifdef  JVMOSAPI_H
 #include <osapi.h>
 #undef JVMOSAPI_H
#endif

/** jvmunxgb.h Must be included only once in the whole project 
 , because of OS specific globals 
**/
/*
#ifdef  JVMOSGB_H
 #include <jvmunxgb.h>             
 #undef JVMOSGB_H
#endif
*/

#endif /* __UNIX__ */


